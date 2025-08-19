list(APPEND examples
  minimal
)


foreach (t IN LISTS examples) 
  # Test help
  add_executable(${t} 
    ${t}.c
  )
  target_link_libraries(${t} PRIVATE pcaio PUBLIC clog)
  target_include_directories(${t} PUBLIC "${PROJECT_BINARY_DIR}")

  add_custom_target(${t}_exec 
    COMMAND ./${t} $$CLI_ARGS
    DEPENDS ${t}
  )
  add_custom_target(${t}_debug 
    COMMAND gdb 
    DEPENDS ${t}
      --command=${PROJECT_SOURCE_DIR}/examples/${t}.gdb
      ./${t} $$CLI_ARGS
  )

  add_custom_target(${t}_profile
    COMMAND "valgrind" ${VALGRIND_FLAGS} ./${t} $$CLI_ARGS
    DEPENDS ${t}
  )
endforeach ()
