if(ENABLE_FORMAT) 
    include(Format)
endif()

if(ENABLE_CPPCHECK)
    include(CppCheck)
endif()

if(ENABLE_DOXYGEN)
    include(Doxygen)
endif()

add_subdirectory(core)
add_subdirectory(capi)
add_subdirectory(console_app)
