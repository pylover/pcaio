cmake_minimum_required(VERSION 3.7)
project(pcaio 
    VERSION 5.2.1
    LANGUAGES C
)
include(CMakeDependentOption)


# GCC and it's flags
# set(CMAKE_C_COMPILER gcc)
# set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fms-extensions")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE=1")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")


# Build type and debug symbols
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Include debug symbols" FORCE)
endif ()
if (CMAKE_BUILD_TYPE!=Release)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
endif ()


# Menu config
add_custom_target(menu COMMAND ccmake ${PROJECT_BINARY_DIR})
add_custom_target(fresh COMMAND cmake ${PROJECT_SOURCE_DIR} --fresh)


# public interface
include_directories(include)


# shared library 
add_subdirectory(pcaio)
add_library(pcaio SHARED
  $<TARGET_OBJECTS:threadpool>
  $<TARGET_OBJECTS:task>
  $<TARGET_OBJECTS:context>
  $<TARGET_OBJECTS:worker>
  $<TARGET_OBJECTS:master>
  $<TARGET_OBJECTS:pcaio_>

  # builtin modules
  $<TARGET_OBJECTS:modio>
  $<TARGET_OBJECTS:modselect>
  $<TARGET_OBJECTS:modepoll>
)
target_link_libraries(pcaio PUBLIC m pthread)


# Install
install(TARGETS pcaio DESTINATION "lib")
install(FILES 
  include/pcaio/pcaio.h 
  include/pcaio/modio.h 
  include/pcaio/modepoll.h 
  include/pcaio/modselect.h 
  include/pcaio/queueT.h 
  include/pcaio/queueT.c 
  include/pcaio/ringT.h 
  include/pcaio/ringT.c 
  DESTINATION "include/pcaio"
)


# Uninstall
add_custom_target("uninstall" COMMENT "Uninstall installed files")
add_custom_command(
  TARGET "uninstall"
  POST_BUILD
  COMMENT "Uninstall files with install_manifest.txt"
  COMMAND xargs rm -vf < install_manifest.txt || echo Nothing in
            install_manifest.txt to be uninstalled!
)


# Lint
set(PRETTYC_FLAGS
    --recursive
    --verbose=0
    --repository=.
    --extensions=c,h,in
    --linelength=80
    --headers=h,in
    --includeorder=standardcfirst
    --root=.
    #--quiet
    ${PROJECT_SOURCE_DIR}
)
add_custom_target(lint
    COMMAND prettyc
    ${PRETTYC_FLAGS}
)


# CPack
set(CPACK_PACKAGE_FILE_NAME "libpcaio-${PROJECT_VERSION}")
set(CPACK_SET_DESTDIR true)
set(CPACK_PACKAGE_NAME libpcaio)
set(CPACK_PACKAGE_CONTACT "Vahid Mardani <vahid.mardani@gmail.com>")
set(CPACK_GENERATOR DEB)
include(CPack)


# Valgrind, Profiling
set(VALGRIND_FLAGS
    -s
    --tool=memcheck 
    --leak-check=yes 
    --show-reachable=yes 
    --num-callers=20 
    --track-fds=yes 
)


# Testing
option(PCAIO_BUILD_TESTS "Build tests/*.c" ON)
if (PCAIO_BUILD_TESTS)
    enable_testing()
    # list(APPEND CMAKE_CTEST_ARGUMENTS "--verbose")
    add_subdirectory(tests)
    list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
endif()


# Examples
option(PCAIO_BUILD_EXAMPLES "Build examples/*.c" ON)
if (PCAIO_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
