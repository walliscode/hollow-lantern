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

  // cast to VoxData
  hollow_lantern::ModelData vox_data = result.value();
  REQUIRE(vox_data.voxels.size() == 1000);

  // create a VoxManipulator object
  hollow_lantern::VoxManipulator manipulator;

  // hollow out the VoxData
  manipulator.HollowOut(vox_data);
  // the outer shell of a 10x10x10 cube 1 voxel thick should have 488 voxels
  REQUIRE(vox_data.hollow_voxels.size() == 488);

  // create a vector of the expected voxel positions of a hollowed-out cube
  std::vector<glm::vec3> expected_hollowed_out_cube_positions = {
      {0, 0, 0},
      {0, 0, 1},
      {0, 0, 2},
      {0, 0, 3},
      {0, 0, 4},
      {0, 0, 5},
      {0, 0, 6},
      {0, 0, 7},
      {0, 0, 8},
      {0, 0, 9},
      // ... (other positions)
      {9, 9, 1},
      {9, 9, 2},
      {9, 9, 3},
      {9, 9, 4},
      {9, 9, 5},
      {9, 9, 6},
      {9, 9, 7},
      {9, 9, 8},
      {9, 9, 9}};

  // check if the hollowed-out VoxData contains the expected voxel positions
  for (const auto &expected_position : expected_hollowed_out_cube_positions) {

    auto it = std::find_if(
        vox_data.hollow_voxels.begin(), vox_data.hollow_voxels.end(),
        [&expected_position](const auto &voxel) {
          std::cout << "Checking voxel at position: " << voxel.position.x
                    << ", " << voxel.position.y << ", " << voxel.position.z
                    << std::endl;
          return voxel.position == expected_position;
        });
    REQUIRE(it != vox_data.hollow_voxels.end());
  }
  auto complex_shape_result =
      vox_reader.ProvideVoxData("complex_shape_1", testing);
  if (!complex_shape_result.has_value()) {
    FAIL(complex_shape_result.error());
  }
  REQUIRE(complex_shape_result.has_value());
  REQUIRE(complex_shape_result->name == "complex_shape_1");

  // cast to VoxData
  hollow_lantern::ModelData complex_shape_vox_data =
      complex_shape_result.value();
  REQUIRE(complex_shape_vox_data.voxels.size() == 940);
  // hollow out the complex shape VoxData
  manipulator.HollowOut(complex_shape_vox_data);

  REQUIRE(complex_shape_vox_data.hollow_voxels.size() == 470);
}
