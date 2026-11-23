#pragma once

#include "cppns/archive/Archive.h"

inline class CPrinter : public CSOArchive {

    // Line Buffer writes to stdout upon destruction to avoid the loss of output
    struct SLineBuffer {
        CString str;
        ~SLineBuffer() {
            if (!str.isEmpty())
                fwrite(str.array(), sizeof(CString::TType), str.getSize(), stdout);
        }
    };

    static SLineBuffer& buffer() {
        // Thread local buffer
        thread_local SLineBuffer instance;
        return instance;
    }

protected:

    virtual size_t write(const CString& inValue) override {
        buffer().str += inValue;
        return 0;
    }

public:

    static size_t flush() {
        auto& [str] = buffer();
        const size_t n = fwrite(str.array(), sizeof(CString::TType), str.getSize(), stdout);
        str.clear();
        return n;
    }

} print;

inline COArchive& EOL(COArchive& inArchive) {
    inArchive << LINE_ENDING;
    dynamic_cast<CPrinter&>(inArchive).flush();
    return inArchive;
}