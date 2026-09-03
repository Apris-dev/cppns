block()
    _add_package_impl(cppns-pch INTERFACE)
    target_precompile_headers(cppns-pch INTERFACE
            include/cppns/Common.h
            include/cppns/PlatformDefinition.h
            include/cppns/Types.h
            include/cppns/Templates.h
    )
endblock()