cmake_minimum_required(VERSION 3.10)

project(getdxinfo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(getdxinfo
    src/main.cpp
)

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
