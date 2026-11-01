#include <iostream>

#include "cppns/archive/FileArchive.h"
#include "cppns/archive/HashArchive.h"
#include "cppns/archive/PathArchive.h"

cppns_main() {

    CPathArchive path(CString{gExecutablePath});

    assert(!path.get().isEmpty(), "Path is empty!");

    std::cout << "File Listing:" << std::endl;
    for (auto files = path.list(); auto& file : files) {
        std::cout << (std::string)file << std::endl;
    }
    std::cout << std::endl;

    path << "input.dat";

    std::cout << (std::string)path.get() << std::endl;
    std::cout << (std::string)path.getFilename() << std::endl;

    {
        CFileArchive<File::OpenType::BINARY_WRITE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    // If emulated, paths won't resolve the same, so skip this test
    //TODO: fix patharchive to work in scenarios such as these or display a better error
    // (emulation, a chroot, a sandbox, or a listing permission you lack)
#if IS_EMULATED
    // Test case sensitivity. By default, we should be case-insensitive
    /*CString str = path.get();
    str.toUpper();

    assert(path.exists(), "Path does not exist!");
    std::cout << (std::string)str << std::endl;

    // Path should be corrected to the real case values
    CPathArchive path2 = str;
    std::cout << (std::string)path2 << std::endl;
    assert(path.get() == path2.get());*/
#endif

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

        std::cout << v << std::endl;
        std::cout << (std::string)s << std::endl;
        std::cout << (std::string)s2 << std::endl;
        std::cout << (b ? "true" : "false") << std::endl;

        assert(v == 5, "Not all read values are correct!");
        assert(s == "test" && s2 == "test200" && b == true, "Not all read values are correct!");
        assert(s2 == "test200", "Not all read values are correct!");
        assert(b == true, "Not all read values are correct!");

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

        std::cout << "Hash1: " << hasher.get() << std::endl;
        std::cout << "Hash2: " << hasher2.get() << std::endl;

        assert(hasher.get() != 0 && hasher2.get() != 0, "Hashes should not be zero!");
        assert(hasher.get() == hasher2.get(), "Hashes should be equal!");
    }

    path.previous();

    path << "input2.dat";

    std::cout << (std::string)path.get() << std::endl;
    std::cout << (std::string)path.getFilename() << std::endl;

    {
        CFileArchive<File::OpenType::WRITE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    assert(path.exists(), "Path does not exist!");

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

        std::cout << v << std::endl;
        std::cout << (std::string)s << std::endl;
        std::cout << (std::string)s2 << std::endl;
        std::cout << (b ? "true" : "false") << std::endl;

        assert(v == 5, "Not all read values are correct!");
        assert(s == "test", "Not all read values are correct!");
        assert(s2 == "test200", "Not all read values are correct!");
        assert(b == true, "Not all read values are correct!");

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

        std::cout << "Hash1: " << hasher.get() << std::endl;
        std::cout << "Hash2: " << hasher2.get() << std::endl;

        assert(hasher.get() != 0 && hasher2.get() != 0, "Hashes should not be zero!");
        assert(hasher.get() == hasher2.get(), "Hashes should be equal!");
    }

    path.previous();

    path << "input3.dat";

    std::cout << (std::string)path.get() << std::endl;
    std::cout << (std::string)path.getFilename() << std::endl;

    {
        CFileArchive<File::OpenType::WRITE, File::LineEnding::NATIVE, File::Format::UTF16LE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    assert(path.exists(), "Path does not exist!");

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

        std::cout << v << std::endl;
        std::cout << (std::string)s << std::endl;
        std::cout << (std::string)s2 << std::endl;
        std::cout << (b ? "true" : "false") << std::endl;

        assert(v == 5, "Not all read values are correct!");
        assert(s == "test", "Not all read values are correct!");
        assert(s2 == "test200", "Not all read values are correct!");
        assert(b == true, "Not all read values are correct!");

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

        std::cout << "Hash1: " << hasher.get() << std::endl;
        std::cout << "Hash2: " << hasher2.get() << std::endl;

        assert(hasher.get() != 0 && hasher2.get() != 0, "Hashes should not be zero!");
        assert(hasher.get() == hasher2.get(), "Hashes should be equal!");
    }

    path.previous();

    path << "input4.dat";

    std::cout << (std::string)path.get() << std::endl;
    std::cout << (std::string)path.getFilename() << std::endl;

    {
        CFileArchive<File::OpenType::WRITE, File::LineEnding::NATIVE, File::Format::UTF32LE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    assert(path.exists(), "Path does not exist!");

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

        std::cout << v << std::endl;
        std::cout << (std::string)s << std::endl;
        std::cout << (std::string)s2 << std::endl;
        std::cout << (b ? "true" : "false") << std::endl;

        assert(v == 5, "Not all read values are correct!");
        assert(s == "test", "Not all read values are correct!");
        assert(s2 == "test200", "Not all read values are correct!");
        assert(b == true, "Not all read values are correct!");

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

        std::cout << "Hash1: " << hasher.get() << std::endl;
        std::cout << "Hash2: " << hasher2.get() << std::endl;

        assert(hasher.get() != 0 && hasher2.get() != 0, "Hashes should not be zero!");
        assert(hasher.get() == hasher2.get(), "Hashes should be equal!");
    }

    path.previous();

    path << "input5.dat";

    std::cout << (std::string)path.get() << std::endl;
    std::cout << (std::string)path.getFilename() << std::endl;

    {
        CFileArchive<File::OpenType::WRITE, File::LineEnding::NATIVE, File::Format::UTF16BE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    assert(path.exists(), "Path does not exist!");

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

        std::cout << v << std::endl;
        std::cout << (std::string)s << std::endl;
        std::cout << (std::string)s2 << std::endl;
        std::cout << (b ? "true" : "false") << std::endl;

        assert(v == 5, "Not all read values are correct!");
        assert(s == "test", "Not all read values are correct!");
        assert(s2 == "test200", "Not all read values are correct!");
        assert(b == true, "Not all read values are correct!");

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

        std::cout << "Hash1: " << hasher.get() << std::endl;
        std::cout << "Hash2: " << hasher2.get() << std::endl;

        assert(hasher.get() != 0 && hasher2.get() != 0, "Hashes should not be zero!");
        assert(hasher.get() == hasher2.get(), "Hashes should be equal!");
    }

    path.previous();

    path << "input6.dat";

    std::cout << (std::string)path.get() << std::endl;
    std::cout << (std::string)path.getFilename() << std::endl;

    {
        CFileArchive<File::OpenType::WRITE, File::LineEnding::NATIVE, File::Format::UTF32BE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    assert(path.exists(), "Path does not exist!");

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

        std::cout << v << std::endl;
        std::cout << (std::string)s << std::endl;
        std::cout << (std::string)s2 << std::endl;
        std::cout << (b ? "true" : "false") << std::endl;

        assert(v == 5, "Not all read values are correct!");
        assert(s == "test", "Not all read values are correct!");
        assert(s2 == "test200", "Not all read values are correct!");
        assert(b == true, "Not all read values are correct!");

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

        std::cout << "Hash1: " << hasher.get() << std::endl;
        std::cout << "Hash2: " << hasher2.get() << std::endl;

        assert(hasher.get() != 0 && hasher2.get() != 0, "Hashes should not be zero!");
        assert(hasher.get() == hasher2.get(), "Hashes should be equal!");
    }

    return 0;
}
