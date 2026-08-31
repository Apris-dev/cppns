add_package_test(TestSequenced
        TestShared.h
        TestSequenced.cpp
)

link_package_test(cppns-archive)
link_package_test(cppns-memory)

add_package_test(TestAssociative
        TestShared.h
        TestAssociative.cpp
)

link_package_test(cppns-archive)
link_package_test(cppns-memory)

add_package_test(TestSelfAssociative
        TestShared.h
        TestSelfAssociative.cpp
)

link_package_test(cppns-archive)
link_package_test(cppns-memory)

add_package_test(TestItr
        TestItr.cpp
)