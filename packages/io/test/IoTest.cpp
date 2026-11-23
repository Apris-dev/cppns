#include <cstdio>
#include <functional>

#include "cppns/io/Printer.h"

#include "cppns/archive/PathArchive.h"
#include "cppns/archive/FileArchive.h"
#include "cppns/util/Threading.h"

#if USING_WINDOWS
    #include <io.h>
#else
    #include <unistd.h>
#endif

// Redirects the process' stdout to inPath for the duration of inFunction, then restores it
static void captureStdout(const CPathArchive& inPath, const std::function<void()>& inFunction) {
    fflush(stdout);

#if USING_WINDOWS
    const int saved = _dup(_fileno(stdout));
#else
    const int saved = dup(fileno(stdout));
#endif

    Assert::True(saved != -1, "Failed to duplicate stdout before redirecting it!");
    // Binary mode avoids the text-mode newline translation corrupting our EOL bytes on Windows
    Assert::True(freopen(inPath.get().array(), "wb", stdout) != nullptr, "Failed to redirect stdout to the capture file!");
    inFunction();

    fflush(stdout);

#if USING_WINDOWS
    _dup2(saved, _fileno(stdout));
    _close(saved);
#else
    dup2(saved, fileno(stdout));
    close(saved);
#endif
}

// Reads the whole captured file
static CString readCapturedFile(const CPathArchive& inPath) {
    CFileArchive<File::OpenType::BINARY_READ> file(inPath);
    CString out;
    file >> out;
    return out;
}

cppns_main() {

    CPathArchive capturePath("~");
    capturePath << "io_test_capture.txt";

    // Fragments composed with '<<' should land on a single line, only ending at EOL
    {
        captureStdout(capturePath, [] {
            print << "Permissions: ";
            print << "r";
            print << "w";
            print << "x" << EOL;
        });

        Assert::True(readCapturedFile(capturePath) == CString("Permissions: rwx") + LINE_ENDING, "Composed fragments should only produce one EOL!");
    }

    // Arithmetic/bool types should still print
    {
        captureStdout(capturePath, [] {
            print << 42 << EOL;
        });
        Assert::True(readCapturedFile(capturePath) == CString("42") + LINE_ENDING, "Arithmetic types should still print!");

        captureStdout(capturePath, [] {
            print << true << EOL;
        });
        Assert::True(readCapturedFile(capturePath) == CString("true") + LINE_ENDING, "bool should still print!");
    }

    // An empty fragment shouldn't disturb the rest of the line
    {
        captureStdout(capturePath, [] {
            print << CString() << "ok" << EOL;
        });
        Assert::True(readCapturedFile(capturePath) == CString("ok") + LINE_ENDING, "An empty CString fragment should not affect the line!");
    }

    // The buffer must fully clear after a flush, or the next line inherits leftover content
    {
        captureStdout(capturePath, [] {
            print << "first" << EOL;
            print << "second" << EOL;
        });
        Assert::True(readCapturedFile(capturePath) == CString("first") + LINE_ENDING + CString("second") + LINE_ENDING, "The line buffer must be cleared after flushing at EOL!");
    }

    // A thread that ends mid-line should still print
    {
        captureStdout(capturePath, [] {
            // No EOL - the thread ends with a pending partial line
            CThread thread([] {
                print << "from worker thread";
            });
            thread.stop();
        });

        Assert::True(readCapturedFile(capturePath) == "from worker thread", "A thread ending mid-line should still flush its buffer!");
    }

    return 0;
}
