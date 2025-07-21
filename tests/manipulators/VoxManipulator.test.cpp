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

  // alternate between layer [9] and layer [0] being colored

  for (size_t mask_indx = 0; mask_indx < model_data.masks.size(); ++mask_indx) {
    const auto &mask = model_data.masks[mask_indx];
    // print out mask data

    for (size_t dim1 = 0; dim1 < mask.data.size(); ++dim1) {
      for (size_t dim2 = 0; dim2 < mask.data[dim1].size(); ++dim2) {
        for (size_t dim3 = 0; dim3 < mask.data[dim1][dim2].size(); ++dim3) {
          if (mask.data[dim1][dim2][dim3].has_value()) {
            std::cout << "[DEBUG] Mask[" << mask_indx << "] at (" << dim1
                      << ", " << dim2 << ", " << dim3 << ") has color "
                      << mask.data[dim1][dim2][dim3]->toInteger() << std::endl;
          } else {
            std::cout << "[DEBUG] Mask[" << mask_indx << "] at (" << dim1
                      << ", " << dim2 << ", " << dim3
                      << ") is not colored (nullopt)" << std::endl;
          }
        }
      }
    }
    for (size_t dim1 = 0; dim1 < mask.data.size(); ++dim1) {
      for (size_t dim2 = 0; dim2 < mask.data[dim1].size(); ++dim2) {
        for (size_t dim3 = 0; dim3 < mask.data[dim1][dim2].size(); ++dim3) {
          if (mask_indx % 2 == 0 && dim1 == 9) {

            // if mask index is odd and sum of dimensions is even, color it
            REQUIRE(mask.data[dim1][dim2][dim3].has_value());
          } else if (mask_indx % 2 == 1 && dim1 == 0) {
            // if mask index is even and sum of dimensions is odd, color it
            REQUIRE(mask.data[dim1][dim2][dim3].has_value());
          } else {
            // if mask index is even and sum of dimensions is odd, not colored
            REQUIRE(!mask.data[dim1][dim2][dim3].has_value());
          }
        }
      }
    }
  }
}
