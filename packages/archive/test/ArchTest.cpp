#include <iostream>

#include "cppns/archive/FileArchive.h"
#include "cppns/archive/PathArchive.h"

cppns_main() {

    CPathArchive path(gExecutablePath);

    assert(!path.get().empty(), "Path is empty!");

    path << "input.dat";

    std::cout << path.get() << std::endl;
    std::cout << path.getFilename() << std::endl;

    {
        CFileArchive<File::OpenType::BINARY_WRITE> fileArchive(path);

        const size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;
    }

    assert(path.exists(), "Path does not exist!");

    {
        CFileArchive<File::OpenType::BINARY_READ> fileArchive(path);

        size_t v;
        fileArchive >> v;
        std::string s;
        fileArchive >> s;
        std::string s2;
        fileArchive >> s2;
        bool b;
        fileArchive >> b;

        std::cout << v << std::endl;
        std::cout << s << std::endl;
        std::cout << s2 << std::endl;
        std::cout << (b ? "true" : "false") << std::endl;

        assert(v == 5 && s == "test" && s2 == "test200" && b == true, "Not all read values are correct!");

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
        CFileArchive<File::OpenType::READWRITE> fileArchive(path);

        constexpr size_t v = 5;
        fileArchive << v;
        fileArchive << "test";
        fileArchive << "test200";
        fileArchive << true;

        assert(fileArchive.getLines() == 4, "Number of read lines should be 4!");
    }

    assert(path.exists(), "Path does not exist!");

    {
        CFileArchive<File::OpenType::READ> fileArchive(path);

        size_t v;
        fileArchive >> v;
        std::string s;
        fileArchive >> s;
        std::string s2;
        fileArchive >> s2;
        bool b;
        fileArchive >> b;

        std::cout << v << std::endl;
        std::cout << s << std::endl;
        std::cout << s2 << std::endl;
        std::cout << (b ? "true" : "false") << std::endl;

        assert(v == 5 && s == "test" && s2 == "test200" && b == true, "Not all read values are correct!");

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
