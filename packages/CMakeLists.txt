set(PACKAGES
        util
        memory
        container
        archive
        dependencyGraph
)

foreach(PACKAGE IN LISTS PACKAGES)
    add_subdirectory(${PACKAGE})
endforeach ()

# Enable testing for each package if requested
# We do this after creating the packages so the tests dont need to be ordered
if (CPPNS_TEST)
    add_subdirectory(testing)
    foreach(PACKAGE IN LISTS PACKAGES)
        if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE}/test")
            block()
                set(CURRENT_SCOPE_PACKAGE cppns-${PACKAGE})
                add_subdirectory(${PACKAGE}/test)
            endblock()
        endif ()
    endforeach ()
endif ()