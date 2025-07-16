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

namespace hollow_lantern {

enum class RotationAxis { X_AXIS, Y_AXIS, Z_AXIS };
/////////////////////////////////////////////////
/// @class Projector
/// @brief Responsible for projecting 3D models onto 2D planes
///
/////////////////////////////////////////////////
class Projector {

private:
  /////////////////////////////////////////////////
  /// @brief Only keep voxels that are visible from the camera
  ///
  /// This will step through intervals of the x,y and z axis using the ray
  /// resolution. We start the x and y from 0.0 and Z from the model's maximum Z
  /// value. Once a voxel is hit in the z direction we keep it and move on to
  /// the next voxel in the x and y direction. This will ensure that we only
  /// keep voxels that are visible from the camera.
  ///
  /// @param model_data instance of ModelData containing the 3D model data
  /////////////////////////////////////////////////
  void CullUsingRayCasting(std::vector<Voxel> &rotated_data,
                           ModelData &model_data,
                           const float ray_resolution = 1.0f) const;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor for the Projector class
  /////////////////////////////////////////////////
  Projector() = default;

  /////////////////////////////////////////////////
  /// @brief Provide a set of 2D projections from 3D model data
  ///
  /// @param model_data 3D model data
  /// @param rotation_intervals How many intervals to rotate the model
  /// @param rotation_axis Axis to rotate around.
  /// @param tilt_angle Initial tilt angle of the model before rotation
  /////////////////////////////////////////////////
  void ProjectOntoPlanes(ModelData &model_data, const size_t rotation_intervals,
                         const RotationAxis rotation_axis,
                         const glm::vec3 &tilt_angle) const;
};
} // namespace hollow_lantern
