cmake_minimum_required(VERSION 3.15)
project(libjctool VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# hidapi dependency
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
# Link hidapi (support all known target names)
# ---------------------------------------------------------------------------

set(HIDAPI_TARGET "")

foreach(candidate
    hidapi::hidapi
    hidapi::hidapi_winapi
    hidapi-win32
    hidapi
)
    if(TARGET ${candidate})
        set(HIDAPI_TARGET ${candidate})
        break()
    endif()
endforeach()

if(NOT HIDAPI_TARGET)
    message(FATAL_ERROR "Could not find a valid hidapi target")
endif()

target_link_libraries(jctool PUBLIC ${HIDAPI_TARGET})

# Import hidapi include dirs
get_target_property(HID_INCLUDES ${HIDAPI_TARGET} INTERFACE_INCLUDE_DIRECTORIES)
if(HID_INCLUDES)
    target_include_directories(jctool PUBLIC ${HID_INCLUDES})
endif()

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
