///////////////////////////////////////////////////
/// @file
/// @brief Implementation of the Projector class (with debug statements)
/////////////////////////////////////////////////

#include "Projector.h"
#include "ModelData.h"
#include "glm/ext/matrix_transform.hpp"
#include <SFML/Graphics/PrimitiveType.hpp>
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
    ImplementBackFaceCulling(triangles);
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

  // treat the rotation as angles for each axis
  // create a one time rotation matrix and apply it to all triangles
  glm::mat4 rotation_matrix =
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x),
                  glm::vec3(1.0f, 0.0f, 0.0f)) *
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y),
                  glm::vec3(0.0f, 1.0f, 0.0f)) *
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z),
                  glm::vec3(0.0f, 0.0f, 1.0f));

  std::cout << "[DEBUG] FixedAngleProjection with rotation: (" << rotation.x
            << ", " << rotation.y << ", " << rotation.z << ")" << std::endl;
  std::vector<Triangle> triangles = model_data.triangles;
  for (size_t tri_idx = 0; tri_idx < triangles.size(); ++tri_idx) {
    for (size_t vert_idx = 0; vert_idx < triangles[tri_idx].vertices.size();
         ++vert_idx) {
      glm::vec4 transformed_vertex =
          rotation_matrix *
          glm::vec4(triangles[tri_idx].vertices[vert_idx], 1.0f);
      triangles[tri_idx].vertices[vert_idx] = glm::vec3(
          transformed_vertex.x, transformed_vertex.y, transformed_vertex.z);
    }
  }
  std::cout << "[DEBUG] Before back face culling: " << triangles.size()
            << " triangles" << std::endl;
  ImplementBackFaceCulling(triangles);
  // ImplementCullingWithDirections(triangles,
  //                                glm::vec3(rotation.x, rotation.y,
  //                                rotation.z));
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

    if (normal.z < 0.0f) {
      it = triangles.erase(it);
      ++culled_count;
      std::cout << "[DEBUG] Culled a triangle (normal.z < 0)" << std::endl;
    } else {
      ++it; // Only increment if not erasing!
    }
  }
  std::cout << "[DEBUG] Culled total " << culled_count << " triangles"
            << std::endl;
}

/////////////////////////////////////////////////
void Projector::ImplementCullingWithDirections(
    std::vector<Triangle> &triangles,
    const glm::vec3 &rotation_direction) const {
  size_t culled_count = 0;
  for (auto it = triangles.begin(); it != triangles.end();) {

    switch (it->direction) {
    case (Direction::X_POSITIVE):
      if (rotation_direction.x < 0.0f) {
        it = triangles.erase(it);
        ++culled_count;
        std::cout << "[DEBUG] Culled a triangle (X_POSITIVE direction)"
                  << std::endl;
      } else {
        ++it; // Only increment if not erasing!
      }
      break;
    case (Direction::X_NEGATIVE):
      if (rotation_direction.x > 0.0f) {
        it = triangles.erase(it);
        ++culled_count;
        std::cout << "[DEBUG] Culled a triangle (X_NEGATIVE direction)"
                  << std::endl;
      } else {
        ++it; // Only increment if not erasing!
      }
      break;
    case (Direction::Y_POSITIVE):
      if (rotation_direction.y < 0.0f) {
        it = triangles.erase(it);
        ++culled_count;
        std::cout << "[DEBUG] Culled a triangle (Y_POSITIVE direction)"
                  << std::endl;
      } else {
        ++it; // Only increment if not erasing!
      }
      break;
    case (Direction::Y_NEGATIVE):
      if (rotation_direction.y > 0.0f) {
        it = triangles.erase(it);
        ++culled_count;
        std::cout << "[DEBUG] Culled a triangle (Y_NEGATIVE direction)"
                  << std::endl;
      } else {
        ++it; // Only increment if not erasing!
      }
      break;
    case (Direction::Z_POSITIVE):
      if (rotation_direction.z < 0.0f) {
        it = triangles.erase(it);
        ++culled_count;
        std::cout << "[DEBUG] Culled a triangle (Z_POSITIVE direction)"
                  << std::endl;
      } else {
        ++it; // Only increment if not erasing!
      }
      break;
    case (Direction::Z_NEGATIVE):
      if (rotation_direction.z > 0.0f) {
        it = triangles.erase(it);
        ++culled_count;
        std::cout << "[DEBUG] Culled a triangle (Z_NEGATIVE direction)"
                  << std::endl;
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

  for (size_t tri_idx = 0; tri_idx < triangles.size(); ++tri_idx) {
    const auto &triangle = triangles[tri_idx];
    for (size_t vert_idx = 0; vert_idx < triangle.vertices.size(); ++vert_idx) {
      result.append(sf::Vertex(sf::Vector2f(triangle.vertices[vert_idx].x,
                                            triangle.vertices[vert_idx].y),
                               triangle.color));
    }
  }
  return result;
}

} // namespace hollow_lantern
