add_executable(hollow-lantern-tests
  hollow-lantern.test.cpp
)


target_include_directories(hollow-lantern-tests
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hollow-lantern-tests
  PRIVATE
  Catch2::Catch2WithMain

)
