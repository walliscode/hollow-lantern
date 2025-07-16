/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the Projector class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Projector.h"
#include "VoxManipulator.h"
#include "VoxReader.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Projector projects 3D models onto 2D planes", "[Projector]") {

  // Load in simple cube model data

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
  hollow_lantern::ModelData model_data = result.value();

  // create a VoxManipulator object
  hollow_lantern::VoxManipulator manipulator;

  // hollow out the VoxData
  manipulator.HollowOut(model_data);

  // create a Projector object
  hollow_lantern::Projector projector;

  // project the hollowed-out voxels onto the required 2D planes
  // specify rotation intervals, axis, and tilt angle
  projector.ProjectOntoPlanes(
      model_data, 1, hollow_lantern::RotationAxis::Y_AXIS, glm::vec3(0, 0, 0));

  REQUIRE(model_data.projected_data.size() == 1);
  REQUIRE(model_data.projected_data[0].getVertexCount() == 100);

  projector.ProjectOntoPlanes(
      model_data, 7, hollow_lantern::RotationAxis::Y_AXIS, glm::vec3(0, 0, 0));

  REQUIRE(model_data.projected_data.size() == 7);
}
