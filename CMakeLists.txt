cmake_minimum_required(VERSION 3.13)

set(BOREALIS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/borealis)
set(BOREALIS_LIBRARY ${BOREALIS_DIR}/library)

include(${BOREALIS_LIBRARY}/cmake/commonOption.cmake)

cmake_dependent_option(BUILTIN_NSP "Built in NSP forwarder" OFF "PLATFORM_SWITCH" OFF)

include(${BOREALIS_LIBRARY}/cmake/toolchain.cmake)

project(SimpleModDownloader)
set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_REVISION "0")
set(VERSION_BUILD "0")
set(PROJECT_AUTHOR "PoloNX")
set(PROJECT_ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/icon/icon-256.jpg)
set(PROJECT_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources)
set(APP_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

find_package(Threads REQUIRED)
list(APPEND APP_PLATFORM_LIB ${CMAKE_THREAD_LIBS_INIT})

find_package(CURL REQUIRED)
list(APPEND APP_PLATFORM_LIB CURL::libcurl)

find_package(ZLIB REQUIRED)
list(APPEND APP_PLATFORM_LIB ZLIB::ZLIB)

list(APPEND APP_PLATFORM_OPTION
   -DBUILD_PACKAGE_NAME=${PACKAGE_NAME}
   -DBUILD_VERSION_MAJOR=${VERSION_MAJOR}
   -DBUILD_VERSION_MINOR=${VERSION_MINOR}
   -DBUILD_VERSION_REVISION=${VERSION_REVISION}
)

set(BRLS_RESOURCES_DIR ".")

file(GLOB_RECURSE MAIN_SRC "source/**.cpp")
if (PLATFORM_SWITCH)
    list(APPEND MAIN_SRC ${BOREALIS_LIBRARY}/lib/platforms/switch/switch_wrapper.c)
endif()

add_subdirectory(lib)

program_target(${PROJECT_NAME} "${MAIN_SRC}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

if (PLATFORM_SWITCH)
    set(BUILD_FONT_DIR ${CMAKE_BINARY_DIR}/resources/font)
    add_custom_target(${PROJECT_NAME}.nro
        DEPENDS ${PROJECT_NAME}
        COMMAND ${NX_NACPTOOL_EXE} --create ${PROJECT_NAME} ${PROJECT_AUTHOR} ${APP_VERSION} ${PROJECT_NAME}.nacp
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_RESOURCES} ${CMAKE_BINARY_DIR}/resources
        COMMAND ${CMAKE_COMMAND} -E remove -f ${BUILD_FONT_DIR}/*.txt ${BUILD_FONT_DIR}/switch_font.ttf
            ${BUILD_FONT_DIR}/keymap*.ttf
        COMMAND ${NX_ELF2NRO_EXE} ${PROJECT_NAME}.elf ${PROJECT_NAME}.nro --icon=${PROJECT_ICON}
            --nacp=${PROJECT_NAME}.nacp --romfsdir=${CMAKE_BINARY_DIR}/resources
        ALL
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE include ${APP_PLATFORM_INCLUDE})
target_compile_options(${PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections -Wunused-variable ${APP_PLATFORM_OPTION})
target_link_libraries(${PROJECT_NAME} PRIVATE borealis z ${APP_PLATFORM_LIB})
target_link_options(${PROJECT_NAME} PRIVATE ${APP_PLATFORM_LINK_OPTION})