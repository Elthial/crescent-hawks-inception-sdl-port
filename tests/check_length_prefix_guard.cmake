# Sol: subprocesses must stop at the explicit unsupported native-stack guard,
# not succeed, consume guessed payload lengths, or fail for unrelated reasons.
foreach(prefix_read IN ITEMS -1 0 1)
  execute_process(COMMAND "${TEST_PROGRAM}" "${prefix_read}"
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error
    TIMEOUT 5)
  if("${result}" STREQUAL "0" OR "${result}" MATCHES "[Tt]imeout|[Tt]ime.*out" OR
      NOT "${error}" MATCHES "Unsupported native asset length-prefix stack state")
    message(FATAL_ERROR "Prefix ${prefix_read}: unexpected result ${result}: ${output}${error}")
  endif()
endforeach()
