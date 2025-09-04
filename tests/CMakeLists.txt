# Testing
include(CTest)
list(APPEND testrules
  api
  queueT
  ringT
)


list(TRANSFORM testrules PREPEND test-)
set(profilerules ${testrules})
list(TRANSFORM profilerules APPEND -profile)
add_custom_target(profile_tests DEPENDS ${profilerules})


# Valgrind, Profiling
set(VALGRIND_FLAGS
  -s
  --tool=memcheck 
  --leak-check=yes 
  --show-reachable=yes 
  --num-callers=20 
  --track-fds=yes 
)


foreach (t IN LISTS testrules) 
    # Test help
  add_executable(${t} ${t}.c 
    # $<TARGET_OBJECTS:helpers>
  )
  target_include_directories(${t} PUBLIC 
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}"
  )
  target_link_libraries(${t} m pcaio clog)
  add_test(NAME ${t} COMMAND ${t})

  add_custom_target(${t}-exec 
    COMMAND ctest --verbose -R "^${t}$"
    DEPENDS ${t} ${CMAKE_PROJECT_NAME}
  )
  add_custom_target(${t}-debug 
    COMMAND gdb --command=${PROJECT_SOURCE_DIR}/tests/${t}.gdb ./${t}
    DEPENDS ${t} ${CMAKE_PROJECT_NAME}
  )
  add_custom_target(${t}-profile
    COMMAND "valgrind" ${VALGRIND_FLAGS} ${PROJECT_BINARY_DIR}/tests/${t}
    DEPENDS ${t} ${CMAKE_PROJECT_NAME}
  )
endforeach()
