/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for directory paths in the configuration
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "directory_paths.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test directory paths in configuration",
          "[config][directory_paths]") {

  REQUIRE(config::getDataFolder().string() != "");
  REQUIRE(config::getDataFolder().string() != "@data_path@");
}
