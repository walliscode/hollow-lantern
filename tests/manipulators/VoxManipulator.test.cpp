/////////////////////////////////////////////////
/// @file
/// @brief Units tests for VoxManipulator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "VoxManipulator.h"
#include "VoxReader.h"
#include "catch2/catch_test_macros.hpp"
#include <iostream>

TEST_CASE("VoxManipulator provides VoxData object", "[VoxManipulator]") {

  // construct a VoxReader object
  hollow_lantern::VoxReader vox_reader;
  bool testing = true;
  // provide vox data object
  auto result = vox_reader.ProvideVoxData("simple_cube", testing);
  if (!result.has_value()) {
    FAIL(result.error());
  }
  REQUIRE(result.has_value());
  REQUIRE(result->name == "simple_cube");
  std::cout << "[DEBUG] VoxData size: " << result->size.x << "x"
            << result->size.y << "x" << result->size.z << std::endl;
  // cast to VoxData
  hollow_lantern::ModelData model_data = result.value();
  // count the number of voxels in the VoxData with is_visible set to true
  int visible_voxel_count = 0;
  for (size_t x = 0; x < model_data.size.x; ++x) {
    for (size_t y = 0; y < model_data.size.y; ++y) {
      for (size_t z = 0; z < model_data.size.z; ++z) {
        if (model_data.voxel_data[x][y][z].is_visible) {
          visible_voxel_count++;
        }
      }
    }
  }
  REQUIRE(visible_voxel_count == 1000);
  REQUIRE(model_data.triangles.empty());
  // create a VoxManipulator object
  hollow_lantern::VoxManipulator manipulator;

  // manipulate the VoxData
  manipulator.HollowAndMesh(model_data);

  // check if the voxel data is hollowed out
  int hollowed_voxel_count = 0;
  for (size_t x = 0; x < model_data.size.x; ++x) {
    for (size_t y = 0; y < model_data.size.y; ++y) {
      for (size_t z = 0; z < model_data.size.z; ++z) {
        if (model_data.voxel_data[x][y][z].is_internal_voxel) {
          std::cout << "[DEBUG] Hollowed voxel at (" << x << ", " << y << ", "
                    << z << ")" << std::endl;
          hollowed_voxel_count++;
        }
      }
    }
  }
  // check if the number of visible voxels is less than the original
  REQUIRE(hollowed_voxel_count == 512);
}
