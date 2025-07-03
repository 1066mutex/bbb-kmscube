cmake_minimum_required(VERSION 3.16.5)
project(gfx-ex VERSION 1.0.0)

# configure default build type if one is not provided.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

#include(cross.cmake)
#include(docs/Docs.cmake)

# add subdirectories
#add_subdirectory(Etha0_server)

message(STATUS "Generator .............. ${CMAKE_GENERATOR}")
message(STATUS "Build Type ............. ${CMAKE_BUILD_TYPE}")
message(STATUS "C flags,   Debug configuration:                   ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "C flags,   Release configuration:                 ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "C flags,   Release configuration with Debug info: ${CMAKE_C_FLAGS_RELWITHDEBINFO}")
message(STATUS "C flags,   minimal Release configuration:         ${CMAKE_C_FLAGS_MINSIZEREL}")

message(STATUS "C++ flags, Debug configuration:                   ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C++ flags, Release configuration:                 ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "C++ flags, Release configuration with Debug info: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message(STATUS "C++ flags, minimal Release configuration:         ${CMAKE_CXX_FLAGS_MINSIZEREL}")
message(STATUS "cmake toolchain file configuration:               ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "SDK sysroot_target configuration:                 ${CMAKE_SYSROOT}") 
message(STATUS "cmake C flags configuration:                      ${CC}")
message(STATUS "linker                                            ${CMAKE_CXX_LINK_EXECUTABLE} ") 
# print the current set of compiler flags.
message(STATUS "C++ compiler flags: ${CMAKE_CXX_FLAGS}")
# QPCPP framework directory
# NOTE:
# Adjust this path for a different relative path
# between your project and the QPCPP folder
#
set(QPCPP_DIR qpcpp)
#set(QPCPP_DIR ../../qpcpp)

# --------------------------------------------------------------------
# QP Port directory
#
# NOTE:
# For POSIX hosts (Linux, MacOS), you can choose:
# - the single-threaded QP/C++ port (win32-qv) or
# - the multithreaded QP/C++ port (win32).
#
set(QP_PORT_DIR ${QPCPP_DIR}/ports/posix) # multithreaded build
#set(QP_PORT_DIR ${QPCPP_DIR}/ports/posix-qv) # single threaded debugging


# TODO: use a conditional statement to select which on to use
# during build time

# ------------------------------------------------------------------------------
# paths
# ------------------------------------------------------------------------------

#**
# set(PATH_APP       app)
# set(PATH_BSP       bsp)
# set(PATH_CUBE      cube)
# set(PATH_UTILS     utils)
# set(PATH_RAW_PTHREADS raw_pthreads)

# Add all .cpp files in the cube directory
file(GLOB CUBE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(APPEND SOURCES ${CUBE_SOURCES})

add_definitions(-DSG_RESOURCE_PATH="/usr/share/fcl-gfx-test/resources/")

set(Source_Files
    # APP
    main.c
    drm-atomic.c
    common.c
    cube-tex.c
    cube-smooth.c
    drm-common.c
    drm-legacy.c    
    esTransform.c
    frame-512x512-NV12.c
    frame-512x512-RGBA.c
)


# QPCPP configuration with QSPY...
if(QSPY)
    add_compile_definitions(Q_SPY)

    set(Qspy_Files
        ${QPCPP_DIR}/src/qs/qs.cpp
        ${QPCPP_DIR}/ports/posix/qs_port.cpp
        ${QPCPP_DIR}/src/qs/qs_rx.cpp
        ${QPCPP_DIR}/src/qs/qs_fp.cpp
        ${QPCPP_DIR}/src/qs/qutest.cpp
        ${QPCPP_DIR}/src/qs/qs_64bit.cpp
        ${QPCPP_DIR}/src/qs/qstamp.cpp
    )
    set(ALL_FILES ## add qspy sources
        ${Source_Files}
        ${Qspy_Files}
        ${platform_files}
    )
else()
    set(ALL_FILES 
        ${Source_Files}
        ${platform_files}
    )

endif()

# verbose configuration
if(VERBOSE_)
   add_compile_definitions(VERBOSE)
endif()

# verbose configuration
if(SIMULATE_)
   add_compile_definitions(SIMULATE)
endif()

unset(USE_MY_LIB CACHE) # <---- this is important!!. 

add_executable(${PROJECT_NAME}
    ${ALL_FILES}
)

find_package(PkgConfig REQUIRED)

set(ENV{PKG_CONFIG_DIR} "")
set(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_SYSROOT}/usr/lib/pkgconfig:${CMAKE_SYSROOT}/usr/share/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} ${CMAKE_SYSROOT})

pkg_check_modules(DRM REQUIRED libdrm)
pkg_check_modules(EGL REQUIRED egl)
pkg_check_modules(GBM REQUIRED gbm)


target_link_libraries(${PROJECT_NAME}
    ${DRM_LIBRARIES} 
    ${EGL_LIBRARIES} 
    ${GBM_LIBRARIES} 
    GLESv2 
    ts
    libzmq.so
)

# ! add include dir here.
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${DRM_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${GBM_INCLUDE_DIRS}

)

# using IPO (INTERPROCEDURAL_OPTIMIZATION) to catch UB, ODR, and ABI issues
set_property(TARGET gfx-ex PROPERTY INTERPROCEDURAL_OPTIMIZATION)


#------------------------------------------------------------------------
# post build steps.
#------------------------------------------------------------------------
set(CMAKE_DEFAULT_TARGET ${PROJECT_NAME})


# install(TARGETS ${PROJECT_NAME} DESTINATION ./bin)

# install(DIRECTORY resources DESTINATION ./share/${PROJECT_NAME})