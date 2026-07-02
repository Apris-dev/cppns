add_test(TestSequenced
        TestShared.h
        TestSequenced.cpp
)

link_test(cppns-archive)
link_test(cppns-memory)

add_test(TestAssociative
        TestShared.h
        TestAssociative.cpp
)

link_test(cppns-archive)
link_test(cppns-memory)

add_test(TestSelfAssociative
        TestShared.h
        TestSelfAssociative.cpp
)

link_test(cppns-archive)
link_test(cppns-memory)

add_test(ReleaseTest
        TestShared.h
        ReleaseTest.cpp
)

link_test(cppns-memory)

add_test(TestItr
        TestItr.cpp
)