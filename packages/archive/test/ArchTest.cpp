#include <iostream>

#include "cppns/archive/FileArchive.h"
#include "cppns/archive/HashArchive.h"
#include "cppns/archive/PathArchive.h"

cppns_main() {

    CPathArchive path(CString{gExecutablePath});

    assert(!path.get().empty(), "Path is empty!");

    path << "input.dat";

    std::cout << path.get() << std::endl;
    std::cout << path.getFilename() << std::endl;

    {
        CFileArchive<File::OpenType::WRITE, File::Format::BINARY> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    assert(path.exists(), "Path does not exist!");

    {
        CFileArchive<File::OpenType::READ, File::Format::BINARY> fileArchive(path);

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

    std::cout << path.get() << std::endl;
    std::cout << path.getFilename() << std::endl;

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

    std::cout << path.get() << std::endl;
    std::cout << path.getFilename() << std::endl;

    {
        CFileArchive<File::OpenType::WRITE, File::Format::UTF16> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    assert(path.exists(), "Path does not exist!");

    {
        CFileArchive<File::OpenType::READ, File::Format::UTF16> fileArchive(path);

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

    std::cout << path.get() << std::endl;
    std::cout << path.getFilename() << std::endl;

    {
        CFileArchive<File::OpenType::WRITE, File::Format::UTF32> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    assert(path.exists(), "Path does not exist!");

    {
        CFileArchive<File::OpenType::READ, File::Format::UTF32> fileArchive(path);

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
