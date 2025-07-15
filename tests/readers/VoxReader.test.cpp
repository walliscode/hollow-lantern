/////////////////////////////////////////////////
/// @file
/// @brief units tests for VoxReader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "VoxReader.h"
#include "directory_paths.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("VoxReader provides VoxData object", "[VoxReader]") {
  // default construct a VoxReader object
  hollow_lantern::VoxReader reader;

  // set testing variable to true
  bool testing = true;
  // when provided a false file name, it returns a string
  auto result = reader.ProvideVoxData("non_existent_file", testing);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error() == "Vox file 'non_existent_file.vox' does not exist.");

  // when provided a .vox file extension but doens't have the correct "VOX "
  // header, returns a string
  result = reader.ProvideVoxData("invalid_header", testing);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error() ==
          "Vox file 'invalid_header.vox' has an invalid header.");

  // when provided a .vox file with the correct "VOX " header but no MAIN chunk,
  // returns a string
  result = reader.ProvideVoxData("no_main_chunk", testing);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error() ==
          "Vox file 'no_main_chunk.vox' does not contain a MAIN chunk.");

  // final file for full sucesses testing
  result = reader.ProvideVoxData("chr_knight", testing);
  REQUIRE(result.has_value());
  REQUIRE(result->name == "chr_knight");
  REQUIRE(result->voxels.size() > 0);
  // Check if the voxels have valid positions and colors
  for (const auto &voxel : result->voxels) {
    REQUIRE(voxel.first.x >= 0);
    REQUIRE(voxel.first.y >= 0);
    REQUIRE(voxel.first.z >= 0);
    REQUIRE(((voxel.second.r >= 0) && (voxel.second.r <= 255)));
    REQUIRE(((voxel.second.g >= 0) && (voxel.second.g <= 255)));
    REQUIRE(((voxel.second.b >= 0) && (voxel.second.b <= 255)));
    REQUIRE(((voxel.second.a >= 0) && (voxel.second.a <= 255)));
  }
}
