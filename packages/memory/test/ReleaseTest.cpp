
#include <iostream>

#include "cppns/memory/Memory.h"

cppns_main() {

    struct Releaser {

        ~Releaser() {
            std::cout << "Called Releaser Destructor" << std::endl;
        }

        void release() const {
            std::cout << "Removed Releaser" << std::endl;
            delete this;
        }
    };

    {
        TShared<Releaser> thing = nullptr;
        std::cout << "Created thing" << std::endl;

        {
            TShared<Releaser> thing2{};
            std::cout << "Created thing 2" << std::endl;
            thing = thing2;
            std::cout << "Set thing" << std::endl;
        }
        std::cout << "'Destroyed' thing 2" << std::endl;
        {
            TShared<Releaser> thing3 = thing;
            std::cout << "Created thing 3" << std::endl;
        }
        std::cout << "'Destroyed' thing 3" << std::endl;

    }
    // Its okay that 'Removed Releaser' happens before this, because the scope ends before this does
    std::cout << "'Destroyed' thing" << std::endl << std::endl << std::endl;

    {
        TUnique<Releaser> thing = nullptr;
        {
            TUnique<Releaser> thing2{};
            std::cout << "Created unique thing" << std::endl;
            thing = std::move(thing2);
            std::cout << "Moved unique thing" << std::endl;
        }
        std::cout << "'Destroyed' unique thing 2" << std::endl;
    }
    std::cout << "'Destroyed' unique thing" << std::endl;

    return 0;
}