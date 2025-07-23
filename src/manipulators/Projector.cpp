///////////////////////////////////////////////////
/// @file
/// @brief Implementation of the Projector class (with debug statements)
/////////////////////////////////////////////////

#include "Projector.h"
#include "ModelData.h"
#include "glm/ext/matrix_transform.hpp"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <array>
#include <cmath>
#include <iostream>
#include <vector>

namespace hollow_lantern {

/////////////////////////////////////////////////
void Projector::BasicProjection(ModelData &model_data,
                                const glm::vec3 &tilt_angle,
                                const size_t intervals,
                                const glm::vec3 &rotation_axis) const {

  std::cout << "[DEBUG] Starting BasicProjection with intervals=" << intervals
            << ", tilt_angle=(" << tilt_angle.x << ", " << tilt_angle.y << ", "
            << tilt_angle.z << ")"
            << ", rotation_axis=(" << rotation_axis.x << ", " << rotation_axis.y
            << ", " << rotation_axis.z << ")" << std::endl;

  std::vector<glm::vec3> rotation_positions;
  if (rotation_axis.x != 0.0f) {
    std::cout << "[DEBUG] Using X-axis rotation" << std::endl;
    for (size_t i = 0; i < intervals; ++i) {
      float angle =
          static_cast<float>(i) * (360.0f / static_cast<float>(intervals));
      rotation_positions.emplace_back(angle, 0.0f, 0.0f);
      std::cout << "[DEBUG] Rotation position: (" << angle << ", 0, 0)"
                << std::endl;
    }
  } else if (rotation_axis.y != 0.0f) {
    std::cout << "[DEBUG] Using Y-axis rotation" << std::endl;
    for (size_t i = 0; i < intervals; ++i) {
      float angle =
          static_cast<float>(i) * (360.0f / static_cast<float>(intervals));
      rotation_positions.emplace_back(0.0f, angle, 0.0f);
      std::cout << "[DEBUG] Rotation position: (0, " << angle << ", 0)"
                << std::endl;
    }
  } else if (rotation_axis.z != 0.0f) {
    std::cout << "[DEBUG] Using Z-axis rotation" << std::endl;
    for (size_t i = 0; i < intervals; ++i) {
      float angle =
          static_cast<float>(i) * (360.0f / static_cast<float>(intervals));
      rotation_positions.emplace_back(0.0f, 0.0f, angle);
      std::cout << "[DEBUG] Rotation position: (0, 0, " << angle << ")"
                << std::endl;
    }
  } else {
    std::cout << "[DEBUG] No rotation axis specified, defaulting to Y-axis"
              << std::endl;
    for (size_t i = 0; i < intervals; ++i) {
      float angle =
          static_cast<float>(i) * (360.0f / static_cast<float>(intervals));
      rotation_positions.emplace_back(0.0f, angle, 0.0f);
      std::cout << "[DEBUG] Rotation position: (0, " << angle << ", 0)"
                << std::endl;
    }
  }

  std::vector<glm::mat4> model_matrices =
      GenerateModelMatrices(model_data, tilt_angle, rotation_positions);

  std::cout << "[DEBUG] Generated " << model_matrices.size()
            << " model matrices" << std::endl;

  for (size_t mat_idx = 0; mat_idx < model_matrices.size(); ++mat_idx) {
    const auto &model_matric = model_matrices[mat_idx];
    std::cout << "[DEBUG] Processing model matrix #" << mat_idx << std::endl;
    std::vector<Triangle> triangles = model_data.triangles;

    for (size_t tri_idx = 0; tri_idx < triangles.size(); ++tri_idx) {
      for (size_t vert_idx = 0; vert_idx < triangles[tri_idx].vertices.size();
           ++vert_idx) {
        glm::vec4 transformed_vertex =
            model_matric *
            glm::vec4(triangles[tri_idx].vertices[vert_idx], 1.0f);
        triangles[tri_idx].vertices[vert_idx] = glm::vec3(
            transformed_vertex.x, transformed_vertex.y, transformed_vertex.z);
      }
    }

    std::cout << "[DEBUG] Before back face culling: " << triangles.size()
              << " triangles" << std::endl;
    // ImplementBackFaceCulling(triangles);
    ImplementCullingWithDirections(triangles, model_matric);
    std::cout << "[DEBUG] After back face culling: " << triangles.size()
              << " triangles" << std::endl;

    sf::VertexArray projected_data = ProjectOntoVertexArray(triangles);
    std::cout << "[DEBUG] Projected data has "
              << projected_data.getVertexCount() << " vertices" << std::endl;

    model_data.projected_data.push_back(projected_data);
  }
}

/////////////////////////////////////////////////
void Projector::FixedAngleProjection(ModelData &model_data,
                                     const glm::vec3 &rotation) {

  // translate the model to the origin first
  glm::vec3 model_size{static_cast<float>(model_data.size.x),
                       static_cast<float>(model_data.size.y),
                       static_cast<float>(model_data.size.z)};
  glm::vec3 model_center = model_size * 0.5f;
  glm::mat4 translate_to_origin =
      glm::translate(glm::mat4(1.0f), -model_center);
  // create a one time rotation matrix and apply it to all triangles
  glm::mat4 rotation_matrix =
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x),
                  glm::vec3(1.0f, 0.0f, 0.0f)) *
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y),
                  glm::vec3(0.0f, 1.0f, 0.0f)) *
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z),
                  glm::vec3(0.0f, 0.0f, 1.0f));

  // create an an overall model matrix that translates to origin, rotates and
  // then translates back
  glm::mat4 model_matrix =
      -translate_to_origin * rotation_matrix * translate_to_origin;

  std::cout << "[DEBUG] FixedAngleProjection with rotation: (" << rotation.x
            << ", " << rotation.y << ", " << rotation.z << ")" << std::endl;
  std::vector<Triangle> triangles = model_data.triangles;
  for (size_t tri_idx = 0; tri_idx < triangles.size(); ++tri_idx) {
    for (size_t vert_idx = 0; vert_idx < triangles[tri_idx].vertices.size();
         ++vert_idx) {
      glm::vec4 transformed_vertex =
          model_matrix * glm::vec4(triangles[tri_idx].vertices[vert_idx], 1.0f);
      triangles[tri_idx].vertices[vert_idx] = glm::vec3(
          transformed_vertex.x, transformed_vertex.y, transformed_vertex.z);
    }
  }
  std::cout << "[DEBUG] Before back face culling: " << triangles.size()
            << " triangles" << std::endl;
  // ImplementBackFaceCulling(triangles);

  ImplementCullingWithDirections(triangles, rotation_matrix);
  std::cout << "[DEBUG] After back face culling: " << triangles.size()
            << " triangles" << std::endl;
  sf::VertexArray projected_data = ProjectOntoVertexArray(triangles);

  std::cout << "[DEBUG] Projected data has " << projected_data.getVertexCount()
            << " vertices" << std::endl;
  model_data.projected_data.push_back(projected_data);
}
/////////////////////////////////////////////////
std::vector<glm::mat4> Projector::GenerateModelMatrices(
    ModelData &model_data, const glm::vec3 &tilt,
    const std::vector<glm::vec3> &rotation_positions) const {

  std::vector<glm::mat4> model_matrices;
  glm::vec3 model_size{static_cast<float>(model_data.size.x),
                       static_cast<float>(model_data.size.y),
                       static_cast<float>(model_data.size.z)};
  glm::vec3 model_center = model_size * 0.5f;

  glm::mat4 translate_to_origin =
      glm::translate(glm::mat4(1.0f), -model_center);
  glm::mat4 tilt_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(tilt.x),
                                      glm::vec3(1.0f, 0.0f, 0.0f));

  for (size_t idx = 0; idx < rotation_positions.size(); ++idx) {
    const auto &rotation_position = rotation_positions[idx];
    glm::mat4 rotation_matrix =
        glm::rotate(glm::mat4(1.0f), glm::radians(rotation_position.x),
                    glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(rotation_position.y),
                    glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(rotation_position.z),
                    glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 model_matrix =
        rotation_matrix * tilt_matrix * translate_to_origin;
    model_matrices.push_back(model_matrix);
    std::cout << "[DEBUG] Generated model matrix #" << idx << std::endl;
  }
  return model_matrices;
}

/////////////////////////////////////////////////
void Projector::ImplementBackFaceCulling(
    std::vector<Triangle> &triangles) const {

  size_t culled_count = 0;
  for (auto it = triangles.begin(); it != triangles.end();) {
    const auto &v0 = it->vertices[0];
    const auto &v1 = it->vertices[1];
    const auto &v2 = it->vertices[2];

    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

    // Debug print normal

    if (normal.z > 0.0f) {
      it = triangles.erase(it);
      ++culled_count;

    } else {
      ++it; // Only increment if not erasing!
    }
  }
  std::cout << "[DEBUG] Culled total " << culled_count << " triangles"
            << std::endl;
}

/////////////////////////////////////////////////
void Projector::ImplementCullingWithDirections(
    std::vector<Triangle> &triangles, const glm::mat4 &rotation) const {
  size_t culled_count = 0;

  // create vectors to represent the masks for each direction
  std::array<glm::vec3, 6> face_vectors{
      (glm::vec3(1.0f, 0.0f, 0.0f)),  // X_POSITIVE
      (glm::vec3(-1.0f, 0.0f, 0.0f)), // X_NEGATIVE
      (glm::vec3(0.0f, 1.0f, 0.0f)),  // Y_POSITIVE
      (glm::vec3(0.0f, -1.0f, 0.0f)), // Y_NEGATIVE
      (glm::vec3(0.0f, 0.0f, 1.0f)),  // Z_POSITIVE
      (glm::vec3(0.0f, 0.0f, -1.0f))  // Z_NEGATIVE

  };

  // apply the rotation matrix to the face vectors
  for (auto &face_vector : face_vectors) {
    glm::vec4 transformed_vector = rotation * glm::vec4(face_vector, 0.0f);
    face_vector = glm::vec3(transformed_vector.x, transformed_vector.y,
                            transformed_vector.z);
  }

  // store the result of the dot product for each direction
  std::array<bool, 6> facing_z_negative;
  ;

  for (size_t i = 0; i < face_vectors.size(); ++i) {
    glm::vec3 face_vector = face_vectors[i];
    // calculate the dot product with the  negative Z-axis
    float dot_product = glm::dot(face_vector, glm::vec3(0.0f, 0.0f, -1.0f));

    // [DEBUG] Print the dot product for each face vector
    std::cout << "[DEBUG] Dot product for face vector " << i << ": "
              << dot_product << std::endl;

    facing_z_negative[i] = (dot_product > 0.0f);
    std::cout << "[DEBUG] Facing Z negative for face vector " << i << ": "
              << std::boolalpha << facing_z_negative[i] << std::endl;
  }
  for (auto it = triangles.begin(); it != triangles.end();) {

    switch (it->direction) {
    case (Direction::X_POSITIVE):
      if (!facing_z_negative[0]) {

        it = triangles.erase(it);
        ++culled_count;
      } else {
        ++it; // Only increment if not erasing!
      }
      break;
    case (Direction::X_NEGATIVE):
      if (!facing_z_negative[1]) {
        it = triangles.erase(it);
        ++culled_count;
      } else {
        ++it; // Only increment if not erasing!
      }
      break;
    case (Direction::Y_POSITIVE):
      if (!facing_z_negative[2]) {
        it = triangles.erase(it);
        ++culled_count;
      } else {
        ++it; // Only increment if not erasing!
      }
      break;
    case (Direction::Y_NEGATIVE):
      if (!facing_z_negative[3]) {
        it = triangles.erase(it);
        ++culled_count;
      } else {
        ++it; // Only increment if not erasing!
      }
      break;
    case (Direction::Z_POSITIVE):
      if (!facing_z_negative[4]) {
        it = triangles.erase(it);
        ++culled_count;
      } else {
        ++it; // Only increment if not erasing!
      }
      break;
    case (Direction::Z_NEGATIVE):
      if (!facing_z_negative[5]) {
        it = triangles.erase(it);
        ++culled_count;
      } else {
        ++it; // Only increment if not erasing!
      }
      break;
    default:
      break;
    }
  }

  std::cout << "[DEBUG] Culled total " << culled_count << " triangles"
            << std::endl;
}

/////////////////////////////////////////////////
sf::VertexArray Projector::ProjectOntoVertexArray(
    const std::vector<Triangle> &triangles) const {

  sf::VertexArray result(sf::PrimitiveType::Triangles);

  for (const auto &triangle : triangles) {
    // print out triangle vertices and color for debugging
    for (const auto &vertex : triangle.vertices) {
      result.append(
          sf::Vertex(sf::Vector2f(vertex.x, vertex.y), triangle.color));
    }
  }
  return result;
}

} // namespace hollow_lantern
