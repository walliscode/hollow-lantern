add_executable(hollow-lantern
  main.cpp

)

target_link_libraries(hollow-lantern
  PRIVATE
  SFML::Graphics
  readers
  manipulators
)

