/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the Projector class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "ModelData.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
namespace hollow_lantern {

class Projector {

private:
  /////////////////////////////////////////////////
  /// @brief Generate model matrices for all rotation specified
  ///
  /// @param model_data ModelData instance containing the model to project
  /// @param tilt Tilt vector specifying the tilt of the model
  /// @param rotation_positions Vector of rotation positions
  /////////////////////////////////////////////////
  std::vector<glm::mat4>
  GenerateModelMatrices(ModelData &model_data, const glm::vec3 &tilt,
                        const std::vector<glm::vec3> &rotation_positions) const;

  /////////////////////////////////////////////////
  /// @brief Culls rotated triangles that are not visible
  ///
  /// @param triangles Vector of rotated triangles to cull
  /////////////////////////////////////////////////
  void ImplementBackFaceCulling(std::vector<Triangle> &triangles) const;

  void
  ImplementCullingWithDirections(std::vector<Triangle> &triangles,
                                 const glm::vec3 &rotation_direction) const;
  /////////////////////////////////////////////////
  /// @brief Turns 3D triangles into a 2D vertex array of type triangles
  ///
  /// @param triangles Triangles to project onto a vertex array
  /////////////////////////////////////////////////
  sf::VertexArray
  ProjectOntoVertexArray(const std::vector<Triangle> &triangles) const;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor for the Projector class
  /////////////////////////////////////////////////
  Projector() = default;

  /////////////////////////////////////////////////
  /// @brief Generate vertex arrays for intervals of rotations
  ///
  /// The model is tilted and then rotated about an axis at the intervals
  /// specified. A snapshot of the model is taken at each interval.
  ///
  /// @param model_data ModelData instance containing the model to project
  /////////////////////////////////////////////////
  void BasicProjection(ModelData &model_data, const glm::vec3 &tilt_angle,
                       const size_t intervals,
                       const glm::vec3 &rotation_axis) const;

  void FixedAngleProjection(ModelData &model_data, const glm::vec3 &rotation);
};
} // namespace hollow_lantern
