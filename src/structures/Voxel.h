/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the Voxel struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <glm/vec3.hpp>

namespace hollow_lantern {
/////////////////////////////////////////////////
/// @class Voxel
/// @brief Struct to hold a single voxel's position and color
///
/////////////////////////////////////////////////
struct Voxel {
  /////////////////////////////////////////////////
  /// @brief Position of the voxel in 3D space
  /////////////////////////////////////////////////
  glm::vec3 position;

  /////////////////////////////////////////////////
  /// @brief RGBA color of the voxel
  /////////////////////////////////////////////////
  sf::Color color;

  /////////////////////////////////////////////////
  /// @brief Create comparison operator for Voxel for hashing
  ///
  /// @param other [TODO:parameter]
  /// @return [TODO:return]
  /////////////////////////////////////////////////
  bool operator==(const Voxel &other) const {
    return position == other.position;
  };
};
} // namespace hollow_lantern

namespace std {
template <> struct hash<glm::vec3> {
  std::size_t operator()(const glm::vec3 &v) const noexcept {
    std::size_t h1 = std::hash<float>{}(v.x);
    std::size_t h2 = std::hash<float>{}(v.y);
    std::size_t h3 = std::hash<float>{}(v.z);
    // Combine hashes
    return h1 ^ (h2 << 1) ^ (h3 << 2);
  }
};

template <> struct hash<hollow_lantern::Voxel> {
  std::size_t operator()(const hollow_lantern::Voxel &v) const noexcept {
    return std::hash<glm::vec3>{}(v.position);
  }
};
} // namespace std
