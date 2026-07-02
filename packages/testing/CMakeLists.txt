# TestTrueMain block
block()
    add_shared_package(TestTrueMain
            TestTrueMain.h
            TestTrueMain.cpp
    )

    link_package(PUBLIC cppns-util)
endblock()

# TestLib block
block()
    add_shared_package(TestLib
            TestLib.h
            TestLib.cpp
    )

    link_package(PUBLIC TestTrueMain)
endblock()

# TestBootstrap block
block()
    add_package_executable(TestBootstrap
            TestExec.cpp
    )

    link_package(PUBLIC TestTrueMain)
endblock()