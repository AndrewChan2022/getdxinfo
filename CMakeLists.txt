cmake_minimum_required(VERSION 3.10)

project(getdxinfo VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(getdxinfo
    src/main.cpp
)

# if(MSVC)
#     # Force static CRT
#     foreach(flag_var 
#         CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO 
#         CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_MINSIZEREL
#         CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO 
#         CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_MINSIZEREL)
#         string(REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
#     endforeach()
# endif()

# if(MSVC)
#     # For static CRT, set MSVC runtime library
#     set_property(TARGET getdxinfo 
#                  PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# endif()

if(MSVC)
    set_property(TARGET getdxinfo PROPERTY
                 MSVC_RUNTIME_LIBRARY "MultiThreaded") # Release / static
    # Optional: Debug static CRT
    # set_property(TARGET getdxinfo PROPERTY
    #              MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
endif()

# ---- DX11 headers/libs ----
# These come from Windows SDK
target_link_libraries(getdxinfo
    d3d11
    dxgi
)

# Optional but recommended
target_compile_definitions(getdxinfo PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)
