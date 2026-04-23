cmake_minimum_required(VERSION 3.15)
project(libjctool VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# hidapi dependency
# Try the system package first; fall back to FetchContent if not found.
# ---------------------------------------------------------------------------
find_package(hidapi QUIET)

if(NOT hidapi_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        hidapi
        GIT_REPOSITORY https://github.com/libusb/hidapi.git
        GIT_TAG        hidapi-0.14.0
    )
    FetchContent_MakeAvailable(hidapi)
endif()

# ---------------------------------------------------------------------------
# libjctool static library
# ---------------------------------------------------------------------------
add_library(jctool STATIC
    Calibration.cpp
    Calibration.h
    JoyConDevice.cpp
    JoyConDevice.h
    JoyConProtocol.cpp
    JoyConProtocol.h
    Spi.cpp
    Spi.h
    StickUtils.cpp
    StickUtils.h
    Types.h
)

target_include_directories(jctool
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/jctool>
)

# ---------------------------------------------------------------------------
# Link hidapi and import its include directories
# ---------------------------------------------------------------------------

# Link the correct hidapi target depending on platform / FetchContent
if (TARGET hidapi::hidapi)
    target_link_libraries(jctool PUBLIC hidapi::hidapi)
elseif (TARGET hidapi-win32)
    target_link_libraries(jctool PUBLIC hidapi-win32)
elseif (TARGET hidapi)
    target_link_libraries(jctool PUBLIC hidapi)
endif()

# Pull hidapi include dirs from whichever target was linked
# This fixes: "Cannot open include file: 'hidapi/hidapi.h'"
foreach(HID_TARGET hidapi::hidapi hidapi-win32 hidapi)
    if (TARGET ${HID_TARGET})
        get_target_property(HID_INCLUDES ${HID_TARGET} INTERFACE_INCLUDE_DIRECTORIES)
        if (HID_INCLUDES)
            target_include_directories(jctool PUBLIC ${HID_INCLUDES})
        endif()
    endif()
endforeach()

# ---------------------------------------------------------------------------
# Compiler warnings
# ---------------------------------------------------------------------------
if(MSVC)
    target_compile_options(jctool PRIVATE /W4 /WX)
else()
    target_compile_options(jctool PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# ---------------------------------------------------------------------------
# Install rules
# ---------------------------------------------------------------------------
install(TARGETS jctool
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES
    Calibration.h
    JoyConDevice.h
    JoyConProtocol.h
    Spi.h
    StickUtils.h
    Types.h
    DESTINATION include/jctool
)
