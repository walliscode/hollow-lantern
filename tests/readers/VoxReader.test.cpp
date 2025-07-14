/////////////////////////////////////////////////
/// @file
/// @brief units tests for VoxReader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <catch2/catch_test_macros.hpp>

#include "VoxReader.h"
hollow_lantern::VoxReader vox_reader;

TEST_CASE("VoxReader: Default Name", "[VoxReader]") {
  // Check if the default name is set correctly
  REQUIRE(vox_reader.name == "test");
}
