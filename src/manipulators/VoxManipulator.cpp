
/// @file
/// @brief Implementation of VoxManipulator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "VoxManipulator.h"
#include "ModelData.h"
#include <iostream> // For debug output

namespace hollow_lantern {
/////////////////////////////////////////////////
void VoxManipulator::HollowAndMesh(ModelData &model_data) {
  std::cout << "[DEBUG] Starting HollowAndMesh()" << std::endl;
  // Step 1: Hollow out the voxel data
  HollowOut(model_data);
  std::cout << "[DEBUG] Finished HollowOut()" << std::endl;
  // Step 2: Create masks based on the hollowed voxel data
  CreateMasks(model_data);
  std::cout << "[DEBUG] Finished CreateMasks()" << std::endl;
  // Step 3: Generate triangles from the masks
  GreedyMeshing(model_data);
  std::cout << "[DEBUG] Finished GreedyMeshing()" << std::endl;
}
/////////////////////////////////////////////////
void VoxManipulator::HollowOut(ModelData &model_data) {
  std::cout << "[DEBUG] Starting HollowOut()" << std::endl;

  // Create a buffer to store which voxels should be hollowed
  std::vector<std::vector<std::vector<bool>>> hollowed(
      model_data.size.x,
      std::vector<std::vector<bool>>(
          model_data.size.y, std::vector<bool>(model_data.size.z, false)));

  // First pass: evaluate but do not change is_visible
  for (int x = 0; x < model_data.size.x; ++x) {
    for (int y = 0; y < model_data.size.y; ++y) {
      for (int z = 0; z < model_data.size.z; ++z) {
        const Voxel &voxel = model_data.voxel_data[x][y][z];
        if (!voxel.is_visible)
          continue;
        // Check neighbors in all 6 directions
        size_t neighbors = 0;
        if (x > 0 && model_data.voxel_data[x - 1][y][z].is_visible)
          ++neighbors; // left
        if (x < model_data.size.x - 1 &&
            model_data.voxel_data[x + 1][y][z].is_visible)
          ++neighbors; // right
        if (y > 0 && model_data.voxel_data[x][y - 1][z].is_visible)
          ++neighbors; // down
        if (y < model_data.size.y - 1 &&
            model_data.voxel_data[x][y + 1][z].is_visible)
          ++neighbors; // up
        if (z > 0 && model_data.voxel_data[x][y][z - 1].is_visible)
          ++neighbors; // back
        if (z < model_data.size.z - 1 &&
            model_data.voxel_data[x][y][z + 1].is_visible)
          ++neighbors; // front

        // If it has all 6 neighbors, mark to be hollowed
        if (neighbors == 6) {
          hollowed[x][y][z] = true;
        }
      }
    }
  }

  // Second pass: update is_visible
  for (int x = 0; x < model_data.size.x; ++x) {
    for (int y = 0; y < model_data.size.y; ++y) {
      for (int z = 0; z < model_data.size.z; ++z) {
        if (hollowed[x][y][z]) {
          model_data.voxel_data[x][y][z].is_visible = false;
          std::cout << "[DEBUG] Voxel at (" << x << "," << y << "," << z
                    << ") hollowed out." << std::endl;
        }
      }
    }
  }

  std::cout << "[DEBUG] Finished HollowOut()" << std::endl;
}

/////////////////////////////////////////////////
void VoxManipulator::CreateMasks(ModelData &model_data) {
  std::cout << "[DEBUG] Starting CreateMasks()" << std::endl;
  auto &voxel_data = model_data.voxel_data;
  size_t mask_count = 0;

  for (auto &mask : model_data.masks) {
    std::cout << "[DEBUG] Creating mask #" << mask_count << " for direction "
              << (int)mask.direction << std::endl;

    // Resize mask.data before accessing it
    switch (mask.direction) {
    case Direction::X_POSITIVE:
    case Direction::X_NEGATIVE: {
      // mask.data[y][z] : y = size.y, z = size.z
      mask.data.resize(model_data.size.y);
      for (size_t y = 0; y < model_data.size.y; ++y)
        mask.data[y].resize(model_data.size.z);
      break;
    }
    case Direction::Y_POSITIVE:
    case Direction::Y_NEGATIVE: {
      // mask.data[x][z] : x = size.x, z = size.z
      mask.data.resize(model_data.size.x);
      for (size_t x = 0; x < model_data.size.x; ++x)
        mask.data[x].resize(model_data.size.z);
      break;
    }
    case Direction::Z_POSITIVE:
    case Direction::Z_NEGATIVE: {
      // mask.data[x][y] : x = size.x, y = size.y
      mask.data.resize(model_data.size.x);
      for (size_t x = 0; x < model_data.size.x; ++x)
        mask.data[x].resize(model_data.size.y);
      break;
    }
    default:
      std::cout << "[DEBUG] Unknown mask direction: " << (int)mask.direction
                << std::endl;
      break;
    }

    switch (mask.direction) {
    case Direction::X_POSITIVE: {
      for (size_t x = 0; x < model_data.size.x; ++x) {
        for (size_t y = 0; y < model_data.size.y; ++y) {
          for (size_t z = 0; z < model_data.size.z; ++z) {
            if (voxel_data[x][y][z].is_visible) {
              if (x == model_data.size.x - 1 ||
                  !voxel_data[x + 1][y][z].is_visible ||
                  voxel_data[x][y][z].color != voxel_data[x + 1][y][z].color) {
                mask.data[y][z] = voxel_data[x][y][z].color;
                std::cout << "[DEBUG] Mask X_POSITIVE: Visible at (" << x << ","
                          << y << "," << z << ")" << std::endl;
              } else {
                mask.data[y][z] = std::nullopt;
              }
            } else {
              mask.data[y][z] = std::nullopt;
            }
          }
        }
      }
      break;
    }
    case Direction::X_NEGATIVE: {
      for (int x = (int)model_data.size.x - 1; x >= 0; --x) {
        for (size_t y = 0; y < model_data.size.y; ++y) {
          for (size_t z = 0; z < model_data.size.z; ++z) {
            if (voxel_data[x][y][z].is_visible) {
              if (x == 0 || !voxel_data[x - 1][y][z].is_visible ||
                  voxel_data[x][y][z].color != voxel_data[x - 1][y][z].color) {
                mask.data[y][z] = voxel_data[x][y][z].color;
                std::cout << "[DEBUG] Mask X_NEGATIVE: Visible at (" << x << ","
                          << y << "," << z << ")" << std::endl;
              } else {
                mask.data[y][z] = std::nullopt;
              }
            } else {
              mask.data[y][z] = std::nullopt;
            }
          }
        }
      }
      break;
    }
    case Direction::Y_POSITIVE: {
      for (size_t y = 0; y < model_data.size.y; ++y) {
        for (size_t x = 0; x < model_data.size.x; ++x) {
          for (size_t z = 0; z < model_data.size.z; ++z) {
            if (voxel_data[x][y][z].is_visible) {
              if (y == model_data.size.y - 1 ||
                  !voxel_data[x][y + 1][z].is_visible ||
                  voxel_data[x][y][z].color != voxel_data[x][y + 1][z].color) {
                mask.data[x][z] = voxel_data[x][y][z].color;
                std::cout << "[DEBUG] Mask Y_POSITIVE: Visible at (" << x << ","
                          << y << "," << z << ")" << std::endl;
              } else {
                mask.data[x][z] = std::nullopt;
              }
            } else {
              mask.data[x][z] = std::nullopt;
            }
          }
        }
      }
      break;
    }
    case Direction::Y_NEGATIVE: {
      for (int y = (int)model_data.size.y - 1; y >= 0; --y) {
        for (size_t x = 0; x < model_data.size.x; ++x) {
          for (size_t z = 0; z < model_data.size.z; ++z) {
            if (voxel_data[x][y][z].is_visible) {
              if (y == 0 || !voxel_data[x][y - 1][z].is_visible ||
                  voxel_data[x][y][z].color != voxel_data[x][y - 1][z].color) {
                mask.data[x][z] = voxel_data[x][y][z].color;
                std::cout << "[DEBUG] Mask Y_NEGATIVE: Visible at (" << x << ","
                          << y << "," << z << ")" << std::endl;
              } else {
                mask.data[x][z] = std::nullopt;
              }
            } else {
              mask.data[x][z] = std::nullopt;
            }
          }
        }
      }
      break;
    }
    case Direction::Z_POSITIVE: {
      for (size_t z = 0; z < model_data.size.z; ++z) {
        for (size_t x = 0; x < model_data.size.x; ++x) {
          for (size_t y = 0; y < model_data.size.y; ++y) {
            if (voxel_data[x][y][z].is_visible) {
              if (z == model_data.size.z - 1 ||
                  !voxel_data[x][y][z + 1].is_visible ||
                  voxel_data[x][y][z].color != voxel_data[x][y][z + 1].color) {
                mask.data[x][y] = voxel_data[x][y][z].color;
                std::cout << "[DEBUG] Mask Z_POSITIVE: Visible at (" << x << ","
                          << y << "," << z << ")" << std::endl;
              } else {
                mask.data[x][y] = std::nullopt;
              }
            } else {
              mask.data[x][y] = std::nullopt;
            }
          }
        }
      }
      break;
    }
    case Direction::Z_NEGATIVE: {
      for (int z = (int)model_data.size.z - 1; z >= 0; --z) {
        for (size_t x = 0; x < model_data.size.x; ++x) {
          for (size_t y = 0; y < model_data.size.y; ++y) {
            if (voxel_data[x][y][z].is_visible) {
              if (z == 0 || !voxel_data[x][y][z - 1].is_visible ||
                  voxel_data[x][y][z].color != voxel_data[x][y][z - 1].color) {
                mask.data[x][y] = voxel_data[x][y][z].color;
                std::cout << "[DEBUG] Mask Z_NEGATIVE: Visible at (" << x << ","
                          << y << "," << z << ")" << std::endl;
              } else {
                mask.data[x][y] = std::nullopt;
              }
            } else {
              mask.data[x][y] = std::nullopt;
            }
          }
        }
      }
      break;
    }
    default:
      std::cout << "[DEBUG] Unknown mask direction: " << (int)mask.direction
                << std::endl;
      break;
    }
    ++mask_count;
  }
  std::cout << "[DEBUG] Finished CreateMasks()" << std::endl;
}
/////////////////////////////////////////////////
void VoxManipulator::CreateTrianglesFromMask(ModelData &model_data) {
  std::cout << "[DEBUG] Starting CreateTrianglesFromMask()" << std::endl;
  model_data.triangles.clear(); // Clear previous results
  for (const auto &mask : model_data.masks) {
    for (size_t y = 0; y < mask.data.size(); ++y) {
      for (size_t x = 0; x < mask.data[y].size(); ++x) {
        if (mask.data[y][x].has_value()) {
          sf::Color color = mask.data[y][x].value();
          glm::vec3 position(x, y, 0); // Assuming Z=0 for 2D projection
          model_data.triangles.emplace_back(
              position, position + glm::vec3(1, 0, 0),
              position + glm::vec3(0, 1, 0), color, mask.direction);
          model_data.triangles.emplace_back(
              position + glm::vec3(1, 0, 0), position + glm::vec3(1, 1, 0),
              position + glm::vec3(0, 1, 0), color, mask.direction);
        }
      }
    }
  }
  std::cout << "[DEBUG] Finished CreateTrianglesFromMask()" << std::endl;
}
/////////////////////////////////////////////////
void VoxManipulator::GreedyMeshing(ModelData &model_data) {
  std::cout << "[DEBUG] Starting GreedyMeshing()" << std::endl;
  model_data.triangles.clear(); // Clear previous results

  size_t mask_index = 0;
  for (const auto &mask : model_data.masks) {
    // Determine mask size (rows, cols)
    size_t rows = mask.data.size();
    if (rows == 0) {
      std::cout << "[DEBUG] Mask #" << mask_index << " has 0 rows, skipping."
                << std::endl;
      ++mask_index;
      continue;
    }
    size_t cols = mask.data[0].size();

    // Track which cells are already meshed
    std::vector<std::vector<bool>> visited(rows,
                                           std::vector<bool>(cols, false));

    for (size_t y = 0; y < rows; ++y) {
      for (size_t x = 0; x < cols; ++x) {
        // Only process unvisited, colored cells
        if (!visited[y][x] && mask.data[y][x].has_value()) {
          sf::Color color = mask.data[y][x].value();

          // Find maximal width
          size_t width = 1;
          while (x + width < cols && !visited[y][x + width] &&
                 mask.data[y][x + width].has_value() &&
                 mask.data[y][x + width].value() == color) {
            ++width;
          }
          // Find maximal height
          size_t height = 1;
          bool can_expand = true;
          while (y + height < rows && can_expand) {
            for (size_t w = 0; w < width; ++w) {
              if (visited[y + height][x + w] ||
                  !mask.data[y + height][x + w].has_value() ||
                  mask.data[y + height][x + w].value() != color) {
                can_expand = false;
                break;
              }
            }
            if (can_expand)
              ++height;
          }

          std::cout << "[DEBUG] Creating quad at mask #" << mask_index
                    << " position (" << x << "," << y << ") with size " << width
                    << "x" << height << " and color (" << (int)color.r << ","
                    << (int)color.g << "," << (int)color.b << ","
                    << (int)color.a << ")" << std::endl;

          // Mark all cells in the quad as visited
          for (size_t dy = 0; dy < height; ++dy)
            for (size_t dx = 0; dx < width; ++dx)
              visited[y + dy][x + dx] = true;

          // Compute quad corners in 3D space
          std::array<glm::vec3, 4> quad_vertices;
          switch (mask.direction) {
          case Direction::X_POSITIVE:
          case Direction::X_NEGATIVE: {
            // x = constant, y and z vary
            int X = (mask.direction == Direction::X_POSITIVE)
                        ? model_data.size.x - 1
                        : 0;
            quad_vertices = {glm::vec3(X, y, x), glm::vec3(X, y, x + width),
                             glm::vec3(X, y + height, x),
                             glm::vec3(X, y + height, x + width)};
            break;
          }
          case Direction::Y_POSITIVE:
          case Direction::Y_NEGATIVE: {
            // y = constant, x and z vary
            int Y = (mask.direction == Direction::Y_POSITIVE)
                        ? model_data.size.y - 1
                        : 0;
            quad_vertices = {glm::vec3(x, Y, y), glm::vec3(x + width, Y, y),
                             glm::vec3(x, Y, y + height),
                             glm::vec3(x + width, Y, y + height)};
            break;
          }
          case Direction::Z_POSITIVE:
          case Direction::Z_NEGATIVE: {
            // z = constant, x and y vary
            int Z = (mask.direction == Direction::Z_POSITIVE)
                        ? model_data.size.z - 1
                        : 0;
            quad_vertices = {glm::vec3(x, y, Z), glm::vec3(x + width, y, Z),
                             glm::vec3(x, y + height, Z),
                             glm::vec3(x + width, y + height, Z)};
            break;
          }
          default:
            std::cout << "[DEBUG] Unknown mask direction in GreedyMeshing: "
                      << (int)mask.direction << std::endl;
            continue;
          }

          // Split quad into two triangles
          // Triangle 1: v0, v1, v2 | Triangle 2: v2, v1, v3
          model_data.triangles.emplace_back(quad_vertices[0], quad_vertices[1],
                                            quad_vertices[2], color,
                                            mask.direction);
          model_data.triangles.emplace_back(quad_vertices[2], quad_vertices[1],
                                            quad_vertices[3], color,
                                            mask.direction);
        }
      }
    }
    ++mask_index;
  }
  std::cout << "[DEBUG] Finished GreedyMeshing()" << std::endl;
}
} // namespace hollow_lantern
