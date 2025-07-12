add_library(test_readers
VoxReader.test.cpp)

target_link_libraries(test_readers
PRIVATE
readers)


if(TARGET readers)
  message(STATUS "Target 'readers' is available to test_readers.")
else()
  message(WARNING "Target 'readers' is not available to test_readers.")
endif()
