#include "cppns/archive/FileArchive.h"
#include "cppns/archive/HashArchive.h"
#include "cppns/archive/PathArchive.h"

#include "cppns/io/Printer.h"

cppns_main() {

    // Print our working directory
    {
        CPathArchive path(".");
        print << path << EOL;

        // Checks perms
        print << "Working directory permissions: ";
        Path::Permissions perms = path.getPermissions();
        print << (perms & Path::Permissions::READ ? "r" : "_");
        print << (perms & Path::Permissions::WRITE ? "w" : "_");
        print << (perms & Path::Permissions::EXECUTE ? "x" : "_") << EOL;
    }

    // Print root
    {
        CPathArchive path("/");
        print << path << EOL;

        // Checks perms
        print << "Root directory permissions: ";
        Path::Permissions perms = path.getPermissions();
        print << (perms & Path::Permissions::READ ? "r" : "_");
        print << (perms & Path::Permissions::WRITE ? "w" : "_");
        print << (perms & Path::Permissions::EXECUTE ? "x" : "_") << EOL;
    }

    CPathArchive path("~");

    // Executable directory should have reliable permissions to test for
    Path::Permissions perms = path.getPermissions();
    Assert::True(perms & Path::Permissions::READ, "Working directory does not have read permissions");
    Assert::True(perms & Path::Permissions::WRITE, "Working directory does not have write permissions");
    Assert::True(perms & Path::Permissions::EXECUTE, "Working directory does not have execute permissions");

    // Test to go up and down a directory
    {
        CString fullPath = path.get();
        CString current0 = path.top();
        path << "..";
        CString current1 = path.top();
        path << "..";
        path << current1;
        path << current0;

        print << fullPath << EOL;
        print << path.get() << EOL;

        Assert::True(fullPath == path.get(), "Failed going up a directory and going back!");
    }

    Assert::True(!path.get().isEmpty(), "Path is empty!");

    print << "File Listing:" << EOL;
    for (auto files = path.list(); auto& file : files) {
        print << file << EOL;
    }
    print << EOL;

    path << "input.dat";

    print << path.get() << EOL;
    print << path.getFilename() << EOL;

    {
        CFileArchive<File::OpenType::BINARY_WRITE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    // If emulated, paths won't resolve the same, so skip this test

    //TODO: List of issues that occur with searching directories:
    // chroot - Changes root directory, directories inside chroot are direct children of root '/'.  This means searching will likely fail unless exec is inside chroot
    // emulation - Normally ran with root mappings, similar to chroot
    // sandbox (container) - Directories that are not in the sandbox cannot be searched, sandboxes can deny permissions to list directories
    // no read perms - cannot list directories, should message
    // no write perms - can list and search, but cannot write, should message

    // Test case sensitivity. By default, we should be case-insensitive
    CString str = path.get();
    str.toUpper();

    Assert::True(path.exists(), "Path does not exist!");
    print << str << EOL;

    // Path should be corrected to the real case values
    CPathArchive path2 = str;
    print << path2 << EOL;
    Assert::True(path.get() == path2.get());

    {
        CFileArchive<File::OpenType::BINARY_READ> fileArchive(path);

        size_t v;
        fileArchive >> v;
        CString s;
        fileArchive >> s;
        CString s2;
        fileArchive >> s2;
        bool b;
        fileArchive >> b;

        print << v << EOL;
        print << s << EOL;
        print << s2 << EOL;
        print << (b ? "true" : "false") << EOL;

        Assert::True(v == 5, "Not all read values are correct!");
        Assert::True(s == "test" && s2 == "test200" && b == true, "Not all read values are correct!");
        Assert::True(s2 == "test200", "Not all read values are correct!");
        Assert::True(b == true, "Not all read values are correct!");

        CHashArchive hasher;
        hasher << v;
        hasher << s;
        hasher << s2;
        hasher << b;

        CHashArchive hasher2;
        hasher2 << v;
        hasher2 << s;
        hasher2 << s2;
        hasher2 << b;

        print << "Hash1: " << hasher.get() << EOL;
        print << "Hash2: " << hasher2.get() << EOL;

        Assert::True(hasher.get() != 0 && hasher2.get() != 0, "Hashes should not be zero!");
        Assert::True(hasher.get() == hasher2.get(), "Hashes should be equal!");
    }

    path.previous();

    path << "input2.dat";

    print << path.get() << EOL;
    print << path.getFilename() << EOL;

    {
        CFileArchive<File::OpenType::WRITE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    Assert::True(path.exists(), "Path does not exist!");

    {
        CFileArchive<File::OpenType::READ> fileArchive(path);

        size_t v;
        fileArchive >> v;
        CString s;
        fileArchive >> s;
        CString s2;
        fileArchive >> s2;
        bool b;
        fileArchive >> b;

        print << v << EOL;
        print << s << EOL;
        print << s2 << EOL;
        print << (b ? "true" : "false") << EOL;

        Assert::True(v == 5, "Not all read values are correct!");
        Assert::True(s == "test", "Not all read values are correct!");
        Assert::True(s2 == "test200", "Not all read values are correct!");
        Assert::True(b == true, "Not all read values are correct!");

        CHashArchive hasher;
        hasher << v;
        hasher << s;
        hasher << s2;
        hasher << b;

        CHashArchive hasher2;
        hasher2 << v;
        hasher2 << s;
        hasher2 << s2;
        hasher2 << b;

        print << "Hash1: " << hasher.get() << EOL;
        print << "Hash2: " << hasher2.get() << EOL;

        Assert::True(hasher.get() != 0 && hasher2.get() != 0, "Hashes should not be zero!");
        Assert::True(hasher.get() == hasher2.get(), "Hashes should be equal!");
    }

    path.previous();

    path << "input3.dat";

    print << path.get() << EOL;
    print << path.getFilename() << EOL;

    {
        CFileArchive<File::OpenType::WRITE, File::LineEnding::NATIVE, File::Format::UTF16LE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    Assert::True(path.exists(), "Path does not exist!");

    {
        CFileArchive<File::OpenType::READ> fileArchive(path);

        size_t v;
        fileArchive >> v;
        CString s;
        fileArchive >> s;
        CString s2;
        fileArchive >> s2;
        bool b;
        fileArchive >> b;

        print << v << EOL;
        print << s << EOL;
        print << s2 << EOL;
        print << (b ? "true" : "false") << EOL;

        Assert::True(v == 5, "Not all read values are correct!");
        Assert::True(s == "test", "Not all read values are correct!");
        Assert::True(s2 == "test200", "Not all read values are correct!");
        Assert::True(b == true, "Not all read values are correct!");

        CHashArchive hasher;
        hasher << v;
        hasher << s;
        hasher << s2;
        hasher << b;

        CHashArchive hasher2;
        hasher2 << v;
        hasher2 << s;
        hasher2 << s2;
        hasher2 << b;

        print << "Hash1: " << hasher.get() << EOL;
        print << "Hash2: " << hasher2.get() << EOL;

        Assert::True(hasher.get() != 0 && hasher2.get() != 0, "Hashes should not be zero!");
        Assert::True(hasher.get() == hasher2.get(), "Hashes should be equal!");
    }

    path.previous();

    path << "input4.dat";

    print << path.get() << EOL;
    print << path.getFilename() << EOL;

    {
        CFileArchive<File::OpenType::WRITE, File::LineEnding::NATIVE, File::Format::UTF32LE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    Assert::True(path.exists(), "Path does not exist!");

    {
        CFileArchive<File::OpenType::READ> fileArchive(path);

        size_t v;
        fileArchive >> v;
        CString s;
        fileArchive >> s;
        CString s2;
        fileArchive >> s2;
        bool b;
        fileArchive >> b;

        print << v << EOL;
        print << s << EOL;
        print << s2 << EOL;
        print << (b ? "true" : "false") << EOL;

        Assert::True(v == 5, "Not all read values are correct!");
        Assert::True(s == "test", "Not all read values are correct!");
        Assert::True(s2 == "test200", "Not all read values are correct!");
        Assert::True(b == true, "Not all read values are correct!");

        CHashArchive hasher;
        hasher << v;
        hasher << s;
        hasher << s2;
        hasher << b;

        CHashArchive hasher2;
        hasher2 << v;
        hasher2 << s;
        hasher2 << s2;
        hasher2 << b;

        print << "Hash1: " << hasher.get() << EOL;
        print << "Hash2: " << hasher2.get() << EOL;

        Assert::True(hasher.get() != 0 && hasher2.get() != 0, "Hashes should not be zero!");
        Assert::True(hasher.get() == hasher2.get(), "Hashes should be equal!");
    }

    path.previous();

    path << "input5.dat";

    print << path.get() << EOL;
    print << path.getFilename() << EOL;

    {
        CFileArchive<File::OpenType::WRITE, File::LineEnding::NATIVE, File::Format::UTF16BE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    Assert::True(path.exists(), "Path does not exist!");

    {
        CFileArchive<File::OpenType::READ> fileArchive(path);

        size_t v;
        fileArchive >> v;
        CString s;
        fileArchive >> s;
        CString s2;
        fileArchive >> s2;
        bool b;
        fileArchive >> b;

        print << v << EOL;
        print << s << EOL;
        print << s2 << EOL;
        print << (b ? "true" : "false") << EOL;

        Assert::True(v == 5, "Not all read values are correct!");
        Assert::True(s == "test", "Not all read values are correct!");
        Assert::True(s2 == "test200", "Not all read values are correct!");
        Assert::True(b == true, "Not all read values are correct!");

        CHashArchive hasher;
        hasher << v;
        hasher << s;
        hasher << s2;
        hasher << b;

        CHashArchive hasher2;
        hasher2 << v;
        hasher2 << s;
        hasher2 << s2;
        hasher2 << b;

        print << "Hash1: " << hasher.get() << EOL;
        print << "Hash2: " << hasher2.get() << EOL;

        Assert::True(hasher.get() != 0 && hasher2.get() != 0, "Hashes should not be zero!");
        Assert::True(hasher.get() == hasher2.get(), "Hashes should be equal!");
    }

    path.previous();

    path << "input6.dat";

    print << path.get() << EOL;
    print << path.getFilename() << EOL;

    {
        CFileArchive<File::OpenType::WRITE, File::LineEnding::NATIVE, File::Format::UTF32BE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    Assert::True(path.exists(), "Path does not exist!");

    {
        CFileArchive<File::OpenType::READ> fileArchive(path);

        size_t v;
        fileArchive >> v;
        CString s;
        fileArchive >> s;
        CString s2;
        fileArchive >> s2;
        bool b;
        fileArchive >> b;

        print << v << EOL;
        print << s << EOL;
        print << s2 << EOL;
        print << (b ? "true" : "false") << EOL;

        Assert::True(v == 5, "Not all read values are correct!");
        Assert::True(s == "test", "Not all read values are correct!");
        Assert::True(s2 == "test200", "Not all read values are correct!");
        Assert::True(b == true, "Not all read values are correct!");

        CHashArchive hasher;
        hasher << v;
        hasher << s;
        hasher << s2;
        hasher << b;

        CHashArchive hasher2;
        hasher2 << v;
        hasher2 << s;
        hasher2 << s2;
        hasher2 << b;

        print << "Hash1: " << hasher.get() << EOL;
        print << "Hash2: " << hasher2.get() << EOL;

        Assert::True(hasher.get() != 0 && hasher2.get() != 0, "Hashes should not be zero!");
        Assert::True(hasher.get() == hasher2.get(), "Hashes should be equal!");
    }

    return 0;
}
