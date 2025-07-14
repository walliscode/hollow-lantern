add_subdirectory(readers)

# message to check if readers target is available


if(TARGET readers)
  message(STATUS "Target 'readers' is available.")
else()
  message(WARNING "Target 'readers' is not available.")
endif()
