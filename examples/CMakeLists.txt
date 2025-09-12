list(APPEND examples
  minimal
  await
  select_echo
  select_pipe
  epoll_pipe
  asleep
)


foreach (t IN LISTS examples) 
  # Test help
  add_executable(${t} 
    ${t}.c
  )
  target_link_libraries(${t} pcaio clog)
  target_include_directories(${t} PUBLIC "${PROJECT_BINARY_DIR}")

  add_custom_target(${t}_exec 
    COMMAND ./${t}
    DEPENDS ${t}
  )
  add_custom_target(${t}_debug 
    COMMAND gdb 
      --command=${PROJECT_SOURCE_DIR}/examples/${t}.gdb
      ./${t} 
    DEPENDS ${t}
  )

  add_custom_target(${t}_profile
    COMMAND "valgrind" ${VALGRIND_FLAGS} ./${t}
    DEPENDS ${t}
  )
endforeach ()
