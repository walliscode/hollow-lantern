/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ModelData struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "glm/ext/vector_float3.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector3.hpp>
#include <glm/vec3.hpp>

#include <optional>
#include <string>
#include <vector>

namespace hollow_lantern {

enum class Direction {
  NONE,
  X_POSITIVE,
  X_NEGATIVE,
  Y_POSITIVE,
  Y_NEGATIVE,
  Z_POSITIVE,
  Z_NEGATIVE
};

struct Voxel {
  /////////////////////////////////////////////////
  /// @brief Color of the voxel
  /////////////////////////////////////////////////
  sf::Color color;

  /////////////////////////////////////////////////
  /// @brief For turning the voxel on or off
  /////////////////////////////////////////////////
  bool is_visible{false};
};

struct Mask {
  /////////////////////////////////////////////////
  /// @brief Stores the color data for each voxel in the mask
  /////////////////////////////////////////////////
  std::vector<std::vector<std::optional<sf::Color>>> data;

  /////////////////////////////////////////////////
  /// @brief Convenience variable to provide a direction for the mask
  /////////////////////////////////////////////////
  Direction direction{Direction::NONE};

  Mask(Direction dir) : direction(dir) {};
};

struct Triangle {
  /////////////////////////////////////////////////
  /// @brief Vertices of the triangle in 3D space
  /////////////////////////////////////////////////
  std::array<glm::vec3, 3> vertices;

  /////////////////////////////////////////////////
  /// @brief Colour of the triangle (one colour for the whole triangle)
  /////////////////////////////////////////////////
  sf::Color color;
  /////////////////////////////////////////////////
  /// @brief Convenience variable to provide a direction for the triangle for
  /// backface culling
  /////////////////////////////////////////////////
  Direction direction{Direction::NONE};

  Triangle(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3,
           const sf::Color &col, Direction dir)
      : vertices{v1, v2, v3}, color(col), direction(dir) {};
};
struct ModelData {
  /////////////////////////////////////////////////
  /// @brief Name of the Vox model, taken from the filename
  /////////////////////////////////////////////////
  std::string name{"no_name"};

  /////////////////////////////////////////////////
  /// @brief Maximum size of the model in 3D space
  /////////////////////////////////////////////////
  sf::Vector3i size{0, 0, 0};

  std::vector<std::vector<std::vector<Voxel>>> voxel_data;

  /////////////////////////////////////////////////
  /// @brief point data for the model in 2D space
  /////////////////////////////////////////////////
  std::vector<sf::VertexArray> projected_data;

  std::vector<sf::VertexArray> triangle_data;

  std::array<Mask, 6> masks{
      Mask(Direction::X_POSITIVE), Mask(Direction::X_NEGATIVE),
      Mask(Direction::Y_POSITIVE), Mask(Direction::Y_NEGATIVE),
      Mask(Direction::Z_POSITIVE), Mask(Direction::Z_NEGATIVE)};

  std::vector<Triangle> triangles;
};
} // namespace hollow_lantern
