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

  // Step 2: Create masks based on the hollowed voxel data
  CreateMasks(model_data);

  // CreateTrianglesFromMask(model_data);
  // Step 3: Generate triangles from the masks
  // GreedyMeshing(model_data);
}
/////////////////////////////////////////////////
void VoxManipulator::HollowOut(ModelData &model_data) {
  std::cout << "[DEBUG] Starting HollowOut()" << std::endl;

  // check all neighbors of each voxel and see if visisble or not
  for (int x = 0; x < model_data.size.x; ++x) {
    for (int y = 0; y < model_data.size.y; ++y) {
      for (int z = 0; z < model_data.size.z; ++z) {
        Voxel &voxel = model_data.voxel_data[x][y][z];
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
          voxel.is_internal_voxel = true;
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

  for (auto &mask : model_data.masks) {

    // Resize mask.data before accessing it
    switch (mask.direction) {
    case Direction::X_POSITIVE:
    case Direction::X_NEGATIVE: {
      // mask.data[x][y][z]
      mask.data.resize(model_data.size.x);
      for (size_t x = 0; x < model_data.size.x; ++x) {
        mask.data[x].resize(model_data.size.y);
        for (size_t y = 0; y < model_data.size.y; ++y)
          mask.data[x][y].resize(model_data.size.z);
      }
      break;
    }
    case Direction::Y_POSITIVE:
    case Direction::Y_NEGATIVE: {
      // mask.data[y][z][x]
      mask.data.resize(model_data.size.y);
      for (size_t y = 0; y < model_data.size.y; ++y) {
        mask.data[y].resize(model_data.size.z);
        for (size_t z = 0; z < model_data.size.z; ++z)
          mask.data[y][z].resize(model_data.size.x);
      }
      break;
    }
    case Direction::Z_POSITIVE:
    case Direction::Z_NEGATIVE: {
      // mask.data[z][x][y]
      mask.data.resize(model_data.size.z);
      for (size_t z = 0; z < model_data.size.z; ++z) {
        mask.data[z].resize(model_data.size.x);
        for (size_t x = 0; x < model_data.size.x; ++x)
          mask.data[z][x].resize(model_data.size.y);
      }
      break;
    }
    default:
      std::cout << "[DEBUG] Unknown mask direction: " << (int)mask.direction
                << std::endl;
      break;
    }

    std::cout << "[DEBUG] Mask data resized for direction "
              << static_cast<int>(mask.direction) << std::endl;
    switch (mask.direction) {
    case Direction::X_POSITIVE: {
      // X_POSITIVE: we look at each x slice and evaluate y,z
      for (size_t x = 0; x < model_data.size.x; ++x) {
        for (size_t y = 0; y < model_data.size.y; ++y) {
          for (size_t z = 0; z < model_data.size.z; ++z) {
            if (voxel_data[x][y][z].is_visible) {
              // if at end of model or next voxel is visible (then it is masked)
              if (x == model_data.size.x - 1 ||
                  !voxel_data[x + 1][y][z].is_visible) {

                mask.data[x][y][z] = voxel_data[x][y][z].color;
              } else {
                mask.data[x][y][z] = std::nullopt;
              }
            } else {
              mask.data[x][y][z] = std::nullopt;
            }
          }
        }
      }
      break;
    }
    case Direction::X_NEGATIVE: {
      // start from the other end of the model
      // X_NEGATIVE: we look at each x slice and evaluate y,z
      for (int x = model_data.size.x - 1; x >= 0; --x) {
        for (size_t y = 0; y < model_data.size.y; ++y) {
          for (size_t z = 0; z < model_data.size.z; ++z) {
            if (voxel_data[x][y][z].is_visible) {
              if (x == 0 || !voxel_data[x - 1][y][z].is_visible) {
                mask.data[x][y][z] = voxel_data[x][y][z].color;

              } else {
                mask.data[x][y][z] = std::nullopt;
              }
            } else {
              mask.data[x][y][z] = std::nullopt;
            }
          }
        }
      }
      break;
    }
    case Direction::Y_POSITIVE: {
      // Y_POSITIVE: we look at each y slice and evaluate x,z
      for (size_t y = 0; y < model_data.size.y; ++y) {
        for (size_t x = 0; x < model_data.size.x; ++x) {
          for (size_t z = 0; z < model_data.size.z; ++z) {
            if (voxel_data[x][y][z].is_visible) {
              if (y == model_data.size.y - 1 ||
                  !voxel_data[x][y + 1][z].is_visible) {
                mask.data[y][z][x] = voxel_data[x][y][z].color;
              } else {
                mask.data[y][z][x] = std::nullopt;
              }
            } else {
              mask.data[y][z][x] = std::nullopt;
            }
          }
        }
      }
      break;
    }
    case Direction::Y_NEGATIVE: {
      // Y_NEGATIVE: we look at each y slice and evaluate x,z
      for (int y = model_data.size.y - 1; y >= 0; --y) {
        for (size_t x = 0; x < model_data.size.x; ++x) {
          for (size_t z = 0; z < model_data.size.z; ++z) {
            if (voxel_data[x][y][z].is_visible) {
              if (y == 0 || !voxel_data[x][y - 1][z].is_visible) {
                mask.data[y][z][x] = voxel_data[x][y][z].color;
              } else {
                mask.data[y][z][x] = std::nullopt;
              }
            } else {
              mask.data[y][z][x] = std::nullopt;
            }
          }
        }
      }
      break;
    }
    case Direction::Z_POSITIVE: {
      // Z_POSITIVE: we look at each z slice and evaluate x,y
      for (size_t z = 0; z < model_data.size.z; ++z) {
        for (size_t x = 0; x < model_data.size.x; ++x) {
          for (size_t y = 0; y < model_data.size.y; ++y) {
            if (voxel_data[x][y][z].is_visible) {
              if (z == model_data.size.z - 1 ||
                  !voxel_data[x][y][z + 1].is_visible) {
                mask.data[z][x][y] = voxel_data[x][y][z].color;
              } else {
                mask.data[z][x][y] = std::nullopt;
              }
            } else {
              mask.data[z][x][y] = std::nullopt;
            }
          }
        }
      }
      break;
    }
    case Direction::Z_NEGATIVE: {
      // Z_NEGATIVE: we look at each z slice and evaluate x,y
      for (int z = model_data.size.z - 1; z >= 0; --z) {
        for (size_t x = 0; x < model_data.size.x; ++x) {
          for (size_t y = 0; y < model_data.size.y; ++y) {
            if (voxel_data[x][y][z].is_visible) {
              if (z == 0 || !voxel_data[x][y][z - 1].is_visible) {
                mask.data[z][x][y] = voxel_data[x][y][z].color;
              } else {
                mask.data[z][x][y] = std::nullopt;
              }
            } else {
              mask.data[z][x][y] = std::nullopt;
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
  }
  std::cout << "[DEBUG] Finished CreateMasks()" << std::endl;
}

/////////////////////////////////////////////////
void VoxManipulator::CreateTrianglesFromMask(ModelData &model_data) {

  std::cout << "[DEBUG] Starting CreateTrianglesFromMask()" << std::endl;
  // clear previous triangles (if any)
  model_data.triangles.clear();

  // the mask will have x,y and z in any order we will call them dimension_one
  // e.t.c. iterate over each mask and create triangles from voxel data
  for (const auto &mask : model_data.masks) {
    std::cout << "[DEBUG] Processing mask for direction "
              << static_cast<int>(mask.direction) << std::endl;
    // Iterate over the mask data
    for (size_t dim1 = 0; dim1 < mask.data.size(); ++dim1) {
      for (size_t dim2 = 0; dim2 < mask.data[dim1].size(); ++dim2) {
        for (size_t dim3 = 0; dim3 < mask.data[dim1][dim2].size(); ++dim3) {
          if (mask.data[dim1][dim2][dim3].has_value()) {
            std::cout << "[DEBUG] Found voxel at (" << dim1 << ", " << dim2
                      << ", " << dim3 << ") with color "
                      << mask.data[dim1][dim2][dim3]->toInteger() << std::endl;
            sf::Color color = mask.data[dim1][dim2][dim3].value();
            glm::vec3 vertex_position;

            // create two Triangle objects for each voxel
            Triangle triangle1, triangle2;

            // Determine vertex position based on direction as we are drawing
            // triangles on sides of a cube
            switch (mask.direction) {

              // positive directions are the front of the cube so will need
              // shifting the slice dimension by 1
              // negative directions are the back of the cube so will not need
              // shifting the slice dimension
            case Direction::X_POSITIVE:
              triangle1 = Triangle(glm::vec3(dim1 + 1.0f, dim2, dim3),     // v0
                                   glm::vec3(dim1 + 1.0f, dim2 + 1, dim3), // v1
                                   glm::vec3(dim1 + 1.0f, dim2, dim3 + 1), // v2
                                   color, mask.direction);

              triangle2 =
                  Triangle(glm::vec3(dim1 + 1.0f, dim2 + 1, dim3),     // v2
                           glm::vec3(dim1 + 1.0f, dim2 + 1, dim3 + 1), // v3
                           glm::vec3(dim1 + 1.0f, dim2, dim3 + 1),     // v4
                           color, mask.direction);

              break;
            case Direction::X_NEGATIVE:
              triangle1 = Triangle(glm::vec3(dim1, dim2, dim3),     // v0
                                   glm::vec3(dim1, dim2 + 1, dim3), // v1
                                   glm::vec3(dim1, dim2, dim3 + 1), // v2
                                   color, mask.direction);
              triangle2 = Triangle(glm::vec3(dim1, dim2 + 1, dim3),     // v2
                                   glm::vec3(dim1, dim2 + 1, dim3 + 1), // v3
                                   glm::vec3(dim1, dim2, dim3 + 1),     // v4
                                   color, mask.direction);
              break;
            case Direction::Y_POSITIVE:
              triangle1 = Triangle(glm::vec3(dim1, dim2 + 1.0f, dim3),     // v0
                                   glm::vec3(dim1 + 1, dim2 + 1.0f, dim3), // v1
                                   glm::vec3(dim1, dim2 + 1.0f, dim3 + 1), // v2
                                   color, mask.direction);
              triangle2 =
                  Triangle(glm::vec3(dim1 + 1, dim2 + 1.0f, dim3),     // v2
                           glm::vec3(dim1 + 1, dim2 + 1.0f, dim3 + 1), // v3
                           glm::vec3(dim1, dim2 + 1.0f, dim3 + 1),     // v4
                           color, mask.direction);

              break;
            case Direction::Y_NEGATIVE:
              triangle1 = Triangle(glm::vec3(dim1, dim2, dim3),     // v0
                                   glm::vec3(dim1 + 1, dim2, dim3), // v1
                                   glm::vec3(dim1, dim2, dim3 + 1), // v2
                                   color, mask.direction);
              triangle2 = Triangle(glm::vec3(dim1 + 1, dim2, dim3),     // v2
                                   glm::vec3(dim1 + 1, dim2, dim3 + 1), // v3
                                   glm::vec3(dim1, dim2, dim3 + 1),     // v4
                                   color, mask.direction);

              break;
            case Direction::Z_POSITIVE:
              triangle1 = Triangle(glm::vec3(dim1, dim2, dim3 + 1.0f),     // v0
                                   glm::vec3(dim1 + 1, dim2, dim3 + 1.0f), // v1
                                   glm::vec3(dim1, dim2 + 1, dim3 + 1.0f), // v2
                                   color, mask.direction);

              triangle2 =
                  Triangle(glm::vec3(dim1 + 1, dim2, dim3 + 1.0f),     // v2
                           glm::vec3(dim1 + 1, dim2 + 1, dim3 + 1.0f), // v3
                           glm::vec3(dim1, dim2 + 1, dim3 + 1.0f),     // v4
                           color, mask.direction);
              break;

            case Direction::Z_NEGATIVE:
              triangle1 = Triangle(glm::vec3(dim1, dim2, dim3),     // v0
                                   glm::vec3(dim1 + 1, dim2, dim3), // v1
                                   glm::vec3(dim1, dim2 + 1, dim3), // v2
                                   color, mask.direction);
              triangle2 = Triangle(glm::vec3(dim1 + 1, dim2, dim3),     // v2
                                   glm::vec3(dim1 + 1, dim2 + 1, dim3), // v3
                                   glm::vec3(dim1, dim2 + 1, dim3),     // v4
                                   color, mask.direction);
              break;
            default:
              std::cout << "[DEBUG] Unknown mask direction: "
                        << static_cast<int>(mask.direction) << std::endl;
              continue;
            }
            // Create a triangle from the vertex position and color
            model_data.triangles.emplace_back(triangle1);
            model_data.triangles.emplace_back(triangle2);
          }
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
    // convenience variable for number of slices in the mask
    size_t slice = mask.data.size();
    if (slice == 0) {
      std::cout << "[DEBUG] Mask #" << mask_index << " has no slices, skipping."
                << std::endl;
      ++mask_index;
      continue;
    }
    // Determine mask size (rows, cols)
    size_t rows = mask.data[0].size();
    if (rows == 0) {
      std::cout << "[DEBUG] Mask #" << mask_index << " has 0 rows, skipping."
                << std::endl;
      ++mask_index;
      continue;
    }
    size_t cols = mask.data[0][0].size();

    // Track which cells are already meshed using a corresponding 3D vector of
    // bools. The order must match mask.data[dim1][dim2][dim3].
    std::vector<std::vector<std::vector<bool>>> visited(
        slice,
        std::vector<std::vector<bool>>(rows, std::vector<bool>(cols, false)));

    // Iterate over each slice of the mask and find quads
    for (size_t dim1 = 0; dim1 < slice; ++dim1) {
      for (size_t dim2 = 0; dim2 < rows; ++dim2) {
        for (size_t dim3 = 0; dim3 < cols; ++dim3) {
          // Only process unvisited, colored cells
          if (!visited[dim1][dim2][dim3] &&
              mask.data[dim1][dim2][dim3].has_value()) {
            sf::Color color = mask.data[dim1][dim2][dim3].value();

            // Find maximal width
            size_t width = 1;
            // Expand to the right as long as the next cell is the same color
            while (dim3 + width < cols && !visited[dim1][dim2][dim3 + width] &&
                   mask.data[dim1][dim2][dim3 + width].has_value() &&
                   mask.data[dim1][dim2][dim3 + width].value() == color) {
              ++width;
            }
            // Find maximal height
            size_t height = 1;
            // Expand downwards as long as the next row is the same color
            // (uses width specified above)
            bool can_expand = true;
            while (dim2 + height < rows && can_expand) {

              // the whole row must be the the same colour for the increase in
              // height so we end up with squares
              for (size_t w = 0; w < width; ++w) {
                if (visited[dim1][dim2 + height][dim3 + w] ||
                    !mask.data[dim1][dim2 + height][dim3 + w].has_value() ||
                    mask.data[dim1][dim2 + height][dim3 + w].value() != color) {
                  can_expand = false;
                  break;
                }
              }
              if (can_expand)
                ++height;
            }

            // Mark all cells in the quad as visited
            for (size_t dy = 0; dy < height; ++dy)
              for (size_t dx = 0; dx < width; ++dx)
                visited[dim1][dim2 + dy][dim3 + dx] = true;

            // create two triangles to add to the model data
            Triangle triangle1, triangle2;
            switch (mask.direction) {
            case Direction::X_POSITIVE:
              // TODO: Fill in correct triangle creation for greedy meshing
              break;
            case Direction::X_NEGATIVE:
              // TODO: Fill in correct triangle creation for greedy meshing
              break;
            case Direction::Y_POSITIVE:
              // TODO: Fill in correct triangle creation for greedy meshing
              break;
            case Direction::Y_NEGATIVE:
              // TODO: Fill in correct triangle creation for greedy meshing
              break;
            case Direction::Z_POSITIVE:
              // TODO: Fill in correct triangle creation for greedy meshing
              break;
            case Direction::Z_NEGATIVE:
              // TODO: Fill in correct triangle creation for greedy meshing
              break;
            default:
              std::cout << "[DEBUG] Unknown mask direction in GreedyMeshing: "
                        << (int)mask.direction << std::endl;
              continue;
            }

            // Split quad into two triangles
            // Triangle 1: v0, v1, v2 | Triangle 2: v2, v1, v3
            model_data.triangles.emplace_back(triangle1);
            model_data.triangles.emplace_back(triangle2);
          }
        }
      }
    }
    ++mask_index;
  }
  std::cout << "[DEBUG] Finished GreedyMeshing()" << std::endl;
}
} // namespace hollow_lantern
