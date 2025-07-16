///////////////////////////////////////////////
/// @file
/// @brief Implementation of the Projector class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Projector.h"
#include "ModelData.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include <iostream> // [DEBUG] Add for debug output
#include <unordered_set>

namespace hollow_lantern {
/////////////////////////////////////////////////
void Projector::ProjectOntoPlanes(ModelData &model_data,
                                  const size_t rotation_intervals,
                                  const RotationAxis rotation_axis,
                                  const glm::vec3 &tilt_angle) const {

  // set up RotationAxis
  glm::vec3 rotation_axis_vector(0.0f, 0.0f, 0.0f);
  switch (rotation_axis) {
  case RotationAxis::X_AXIS:
    rotation_axis_vector = glm::vec3(1.0f, 0.0f, 0.0f);
    break;
  case RotationAxis::Y_AXIS:
    rotation_axis_vector = glm::vec3(0.0f, 1.0f, 0.0f);
    break;
  case RotationAxis::Z_AXIS:
    rotation_axis_vector = glm::vec3(0.0f, 0.0f, 1.0f);
    break;
  }

  // clear the projected data
  model_data.projected_data.clear();
  std::cout << "[DEBUG] Cleared projected_data.\n";

  // compute center of model
  glm::vec3 centre(0.0f);
  for (const auto &voxel : model_data.voxels) {
    centre += voxel.position;
  }
  centre /= static_cast<float>(model_data.voxels.size());
  std::cout << "[DEBUG] Computed model centre: (" << centre.x << ", "
            << centre.y << ", " << centre.z << ").\n";

  // create a tilt matrix
  glm::mat4 tilt_x = glm::rotate(glm::mat4(1.0f), glm::radians(tilt_angle.x),
                                 glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 tilt_y = glm::rotate(glm::mat4(1.0f), glm::radians(tilt_angle.y),
                                 glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 tilt_z = glm::rotate(glm::mat4(1.0f), glm::radians(tilt_angle.z),
                                 glm::vec3(0.0f, 0.0f, 1.0f));
  glm::mat4 tilt = tilt_x * tilt_y * tilt_z;
  std::cout << "[DEBUG] Created tilt matrix.\n";

  for (size_t i = rotation_intervals; i > 0; --i) {
    // create a rotation matrix
    float angle = static_cast<float>(i) *
                  (360.0f / static_cast<float>(rotation_intervals));
    glm::mat4 rotation =
        glm::rotate(glm::mat4(1.0f), glm::radians(angle), rotation_axis_vector);

    std::cout << "[DEBUG] Rotation interval " << i << ", angle: " << angle
              << " degrees.\n";

    // create an overall model matrix
    // 1. translate to the centre of the model
    // 2. apply rotation
    // 3. apply tilt
    // 4. translate away from the centre of the model
    glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), +centre) *
                             rotation * tilt *
                             glm::translate(glm::mat4(1.0f), -centre);

    std::cout
        << "[DEBUG] Constructed model matrix for this rotation interval.\n";

    std::vector<Voxel> rotated_data;
    for (const auto &voxel : model_data.hollow_voxels) {
      // apply the model matrix to each voxel
      glm::vec4 transformed_position =
          model_matrix * glm::vec4(voxel.position, 1.0f);
      rotated_data.emplace_back(
          Voxel{glm::vec3(transformed_position), voxel.color});
    }
    std::cout << "[DEBUG] Rotated all voxels for this interval. Total: "
              << rotated_data.size() << ".\n";

    // Cull voxels that are not visible from the camera
    CullUsingRayCasting(rotated_data, model_data, 1.0f);
  }
}

/////////////////////////////////////////////////
void Projector::CullUsingRayCasting(std::vector<Voxel> &rotated_data,
                                    ModelData &model_data,
                                    const float ray_resolution) const {

  std::cout << "[DEBUG] Starting ray casting culling.\n";
  std::unordered_set<Voxel> kept_voxels;

  int rays_cast = 0;
  int voxels_kept = 0;

  for (float x = 0.0f; x < model_data.model_size.x; x += ray_resolution) {
    for (float y = 0.0f; y < model_data.model_size.y; y += ray_resolution) {
      for (float z = model_data.model_size.z; z >= 0.0f; z -= ray_resolution) {
        glm::vec3 ray_start(x, y, z);
        ++rays_cast;
        bool hit_voxel = false;
        for (const auto &voxel : rotated_data) {
          if (glm::distance(ray_start, voxel.position) < ray_resolution) {
            if (kept_voxels.find(voxel) == kept_voxels.end()) {
              ++voxels_kept;
              std::cout << "[DEBUG] Ray at (" << x << ", " << y << ", " << z
                        << ") hit voxel at (" << voxel.position.x << ", "
                        << voxel.position.y << ", " << voxel.position.z
                        << ").\n";
            }
            kept_voxels.insert(voxel); // will not insert duplicates
            hit_voxel = true;
            break; // stop checking further voxels in this z direction
          }
        }
        if (hit_voxel)
          break;
      }
    }
  }

  std::cout << "[DEBUG] Ray casting complete. Rays cast: " << rays_cast
            << ". Voxels kept: " << voxels_kept << ".\n";

  // convert to a sf::VertexArray of points
  sf::VertexArray points(sf::PrimitiveType::Points);
  for (const auto &voxel : kept_voxels) {
    points.append(sf::Vertex(sf::Vector2f(voxel.position.x, voxel.position.y),
                             voxel.color));
  }
  std::cout
      << "[DEBUG] Converted kept voxels to sf::VertexArray. Vertex count: "
      << points.getVertexCount() << ".\n";
  model_data.projected_data.push_back(points);
}
} // namespace hollow_lantern
