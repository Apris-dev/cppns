add_interface_package(SimplePtr
        include/sptr/Memory.h
        include/sptr/Shared.h
        include/sptr/Unique.h
)

link_package(INTERFACE SimpleUtils)

# Automatically call init on a type when created
package_option(SIMPLEPTR_INIT "Enable the auto init ability" On)

# Automatically call a destroy function on the type when destroyed, release will overwrite this behavior
package_option(SIMPLEPTR_DESTROY "Enable the auto destroy ability" On)

# Initially disabled unless for testing, Release can be dangerous if used unknowingly, should only be enabled if you know what it does
# Allows you to specify a release function in the memory's types, which removes all deletion capability from the pointer,
# giving the ability to self-release the memory.  Can cause memory leaks if used improperly.
package_option(SIMPLEPTR_RELEASE "Enable the release auto function ability" ${SIMPLECPP_TEST})