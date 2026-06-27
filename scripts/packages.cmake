# The bootstrapper gives package executables the ability to dynamically load all dlls before launching
option(USE_BOOTSTRAPPER "" OFF)

function(define_project
        PROJECT_NAME
        CXX_VERSION
)
    # Keep Packages as child of target, SimpleCPP is project do some other stuff like versioning and CXX version etc here
    # CXX VS C Project

    # Create a library using this project
    # This library has no code and links all other packges in the project to it
    add_library(${PROJECT_NAME} INTERFACE)

    # Set properties of project library
    set_target_properties(${PROJECT_NAME} PROPERTIES PACKAGES "")

    # Set CXX Version
    set_target_properties(${PROJECT_NAME} PROPERTIES
            CXX_STANDARD ${CXX_VERSION}
            CXX_STANDARD_REQUIRED ON
    )

    message(STATUS "${PROJECT_NAME}: Using compiler ${CMAKE_CXX_COMPILER_ID} on ${CMAKE_SYSTEM}!")

    # Set the project of the current scope
    set(CURRENT_SCOPE_PROJECT ${PROJECT_NAME} PARENT_SCOPE)
endfunction()

function(_ensure_project_scope)
    # Ensure we are in project scope
    if (NOT DEFINED CURRENT_SCOPE_PROJECT)
        message(FATAL_ERROR "${MESSAGE}")
    endif ()
endfunction()

function(project_message MESSAGE)
    if (ARGC GREATER 1)
        set(TYPE ${MESSAGE})
        set(MESSAGE ${ARGV1})
    else ()
        set(TYPE STATUS)
    endif ()
    _ensure_project_scope("Cannot print project message outside of project scope")
    message(${TYPE} "${CURRENT_SCOPE_PROJECT}: ${MESSAGE}")
endfunction()

function(_ensure_package_scope MESSAGE)
    # Ensure we are in package scope
    if (NOT DEFINED CURRENT_SCOPE_PACKAGE)
        project_message(FATAL_ERROR ${MESSAGE})
    endif ()
endfunction()

function(_ensure_not_package_scope MESSAGE)
    # Ensure we are in package scope
    if (DEFINED CURRENT_SCOPE_PACKAGE)
        project_message(FATAL_ERROR ${MESSAGE})
    endif ()
endfunction()

function(package_message MESSAGE)
    if (ARGC GREATER 1)
        set(TYPE ${MESSAGE})
        set(MESSAGE ${ARGV1})
    else ()
        set(TYPE STATUS)
    endif ()
    _ensure_package_scope("Cannot print package message outside of package scope")
    message(${TYPE} "${CURRENT_SCOPE_PROJECT} - ${CURRENT_SCOPE_PACKAGE}: ${MESSAGE}")
endfunction()

function(_ensure_test_scope MESSAGE)
    # Ensure we are in test scope
    if (NOT DEFINED CURRENT_SCOPE_TEST)
        project_message(FATAL_ERROR ${MESSAGE})
    endif ()
endfunction()

function(_ensure_is_package LIBRARY_NAME MESSAGE)
    # Get Packages
    get_target_property(PROJECT_PACKAGES ${CURRENT_SCOPE_PROJECT} PACKAGES)

    # Ensure target package is part of the same project
    if (NOT ${LIBRARY_NAME} IN_LIST PROJECT_PACKAGES)
        project_message(FATAL_ERROR ${MESSAGE})
    endif ()
endfunction()

function(_ensure_is_not_package LIBRARY_NAME MESSAGE)
    # Get Packages
    get_target_property(PROJECT_PACKAGES ${CURRENT_SCOPE_PROJECT} PACKAGES)

    # Ensure target package is part of the same project
    if (${LIBRARY_NAME} IN_LIST PROJECT_PACKAGES)
        project_message(FATAL_ERROR ${MESSAGE})
    endif ()
endfunction()

# Set target properties and other sorts of variables
macro (_set_target_defaults TARGET_NAME)
    # Ensure target is using CXX
    set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)

    # Get cpp version from project and set target properties
    get_target_property(CXX_VERSION ${CURRENT_SCOPE_PROJECT} CXX_STANDARD)

    # Set default presets to hidden for MSVC-like behavior
    set_target_properties(${TARGET_NAME} PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            C_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN YES
            CXX_STANDARD ${CXX_VERSION}
            CXX_STANDARD_REQUIRED ON
    )

    # This tells other libraries that this target has been linked downstream to enable cross-package compatibility without hard requirements
    string(TOUPPER ${CURRENT_SCOPE_PROJECT} UPPER_PROJECT_NAME)
    target_compile_definitions(${TARGET_NAME} INTERFACE USING_${UPPER_PROJECT_NAME})

    # If config is using the predefined cmake 'Debug' type, add a definition USING_DEBUG
    target_compile_definitions(${TARGET_NAME} INTERFACE
            $<$<CONFIG:Debug>:USING_DEBUG>
    )

    # Get the top level source directory, can be used to determine the executable directory from a build environment
    if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        string(REPLACE "/" "\\\\" DEBUG_ROOT_DIR "${CMAKE_SOURCE_DIR}")
        string(REPLACE "/" "\\\\" DEBUG_BINARY_ROOT_DIR "${CMAKE_BINARY_DIR}")
    endif()
    target_compile_definitions(${TARGET_NAME} INTERFACE DEBUG_ROOT_DIR="${DEBUG_ROOT_DIR}")

    # Get the top level binary directory, can be used to determine the where the binary is and where the source is.
    target_compile_definitions(${TARGET_NAME} INTERFACE DEBUG_BINARY_ROOT_DIR="${DEBUG_BINARY_ROOT_DIR}")
endmacro()

# Function for creating packages
macro(_add_package_impl TARGET_NAME LIBRARY_TYPE)
    _ensure_project_scope("Package ${TARGET_NAME} was not created in the scope of a project!")
    _ensure_not_package_scope("Package ${TARGET_NAME} was created in the scope of another package!")
    _ensure_is_not_package(${TARGET_NAME} "Package ${TARGET_NAME} already exists!")

    # Create library with headers and sources
    add_library(${TARGET_NAME} ${LIBRARY_TYPE}
            ${ARGN}
    )

    # This tells other libraries that this target has been linked downstream to enable cross-package compatibility without hard requirements
    string(TOUPPER ${TARGET_NAME} UPPER_TARGET_NAME)
    target_compile_definitions(${TARGET_NAME} INTERFACE USING_${UPPER_TARGET_NAME})

    # Set defaults for targets
    _set_target_defaults(${TARGET_NAME})

    # Add to list of project packages
    # Prepend to ensure that the lowest dependency is at the bottom
    set_property(TARGET ${CURRENT_SCOPE_PROJECT} APPEND PROPERTY PACKAGES ${TARGET_NAME})

    # Set the package of the current scope
    set(CURRENT_SCOPE_PACKAGE ${TARGET_NAME} PARENT_SCOPE)
endmacro()

function(add_interface_package TARGET_NAME)
    _add_package_impl(${TARGET_NAME} INTERFACE ${ARGN})

    # Include Headers for this package
    target_include_directories(${TARGET_NAME} INTERFACE
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
endfunction()

function(add_static_package TARGET_NAME)
    _add_package_impl(${TARGET_NAME} STATIC ${ARGN})

    # Include Headers for this package
    target_include_directories(${TARGET_NAME} PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
endfunction()

function(add_shared_package TARGET_NAME)
    _add_package_impl(${TARGET_NAME} SHARED ${ARGN})

    # Include Headers for this package
    target_include_directories(${TARGET_NAME} PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
endfunction()

# Create an executable for a package.
function(add_package_executable TARGET_NAME)
    _ensure_project_scope("Executable ${TARGET_NAME} was not created in the scope of a project!")
    _ensure_not_package_scope("Executable ${TARGET_NAME} was created in the scope of another package!")
    _ensure_is_not_package(${TARGET_NAME} "Executable ${TARGET_NAME} already exists!")

    # MinGW doesn't export symbols from executables even with ENABLE_EXPORTS set to ON, it is unsupported
    if (USE_BOOTSTRAPPER)
        if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND CMAKE_SYSTEM_NAME MATCHES "Windows")
            project_message(WARNING "Cannot use Bootstrapper with MinGW, either use MSVC or disable the bootstrapper in the future")
        else ()
            # Create the bootstrapper for the executable
            configure_file(
                    ${CMAKE_CURRENT_SOURCE_DIR}/Bootstrapper.c.in
                    ${CMAKE_BINARY_DIR}/Bootstrapper.c
                    @ONLY
            )

            # Add exec with bootstrapper
            add_executable(${TARGET_NAME}
                    ${ARGN}
                    ${CMAKE_BINARY_DIR}/Bootstrapper.c
            )
            set_target_properties(${TARGET_NAME} PROPERTIES
                    ENABLE_EXPORTS ON
            )
        endif ()
    endif ()

    # If target was not created above, make a simple executable
    if (NOT TARGET ${TARGET_NAME})
        # Add exec with original files
        add_executable(${TARGET_NAME}
                ${ARGN}
        )
    endif ()

    # Set defaults for targets
    _set_target_defaults(${TARGET_NAME})

    # Add to list of project packages
    # Prepend to ensure that the lowest dependency is at the bottom
    set_property(TARGET ${CURRENT_SCOPE_PROJECT} APPEND PROPERTY PACKAGES ${TARGET_NAME})

    # Set the package of the current scope
    set(CURRENT_SCOPE_PACKAGE ${TARGET_NAME} PARENT_SCOPE)
endfunction()

# Quickly link two packages together with safety
function(link_package LINK_LIBRARY_TYPE LINK_LIBRARY_NAME)
    _ensure_package_scope("Attempted to link package while not in the scope of a package!")
    _ensure_is_package(${LINK_LIBRARY_NAME} "Cannot link ${CURRENT_SCOPE_PACKAGE} to ${LINK_LIBRARY_NAME}, Package ${LINK_LIBRARY_NAME} does not exist!")

    target_link_libraries(${CURRENT_SCOPE_PACKAGE} ${LINK_LIBRARY_TYPE} ${LINK_LIBRARY_NAME})
endfunction()

# Quickly adds a test for packages
function(add_test TEST_NAME)
    _ensure_package_scope("Tried to add test ${TEST_NAME} while not inside package scope.")

    # Add test as executable and link to package
    add_executable(${CURRENT_SCOPE_PACKAGE}-${TEST_NAME}
            ${ARGN}
    )
    target_link_libraries(${CURRENT_SCOPE_PACKAGE}-${TEST_NAME} ${CURRENT_SCOPE_PACKAGE})

    # Set the test of the current scope
    set(CURRENT_SCOPE_TEST ${TEST_NAME} PARENT_SCOPE)
endfunction()

# Quickly link a test to another package
function(link_test LIBRARY_NAME)
    _ensure_test_scope("Tried to link test to library ${LIBRARY_NAME} while not inside test scope.")
    _ensure_is_package(${LIBRARY_NAME} "Cannot link test ${CURRENT_SCOPE_TEST} to ${LIBRARY_NAME}, Package ${LIBRARY_NAME} does not exist!")

    target_link_libraries(${CURRENT_SCOPE_PACKAGE}-${CURRENT_SCOPE_TEST} ${LIBRARY_NAME})

    # Add test directory of the target package as an include directory
    if (EXISTS "${CMAKE_SOURCE_DIR}/packages/${LIBRARY_NAME}/test")
        target_include_directories(${CURRENT_SCOPE_PACKAGE}-${CURRENT_SCOPE_TEST} PRIVATE "${CMAKE_SOURCE_DIR}/packages/${LIBRARY_NAME}/test")
    endif()

endfunction()

# Quickly add an option with a message to a package
function(package_option OPTION_NAME OPTION_DESCRIPTION OPTION_DEFAULT)
    _ensure_package_scope("Attempted to create a package option while not in the scope of a package!")

    option(${OPTION_NAME} ${OPTION_DESCRIPTION} ${OPTION_DEFAULT})

    if(${OPTION_NAME})
        package_message(STATUS "${OPTION_NAME} Is On")
        target_compile_definitions(${CURRENT_SCOPE_PACKAGE} INTERFACE ${OPTION_NAME})
    endif()
endfunction()