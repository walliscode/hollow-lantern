/////////////////////////////////////////////////
/// @file
/// @brief Implementation of VoxManipulator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "VoxManipulator.h"

namespace hollow_lantern {
/////////////////////////////////////////////////
void VoxManipulator::HollowOut(ModelData &vox_data) {

  // create vector of 6 possible neighbour offsets in glm::vec3
  const std::vector<glm::vec3> relative_neighbour_offsets = {
      {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};
  // add voxels from the voxels member variable to the hollow_voxels vector
  // only if the voxel does not have 6 neighbours
  for (const auto &voxel : vox_data.voxels) {
    int neighbour_count = 0;

    // create absolute neighbour offsets
    std::vector<glm::vec3> absolute_neighbour_offsets;
    for (const auto &offset : relative_neighbour_offsets) {
      absolute_neighbour_offsets.push_back(voxel.position + offset);
    };

    // cycle though the absolute neighbour offsets and see if any voxels match
    for (const auto &neighbour : absolute_neighbour_offsets) {
      // check if the neighbour exists in the voxels vector
      if (std::find_if(vox_data.voxels.begin(), vox_data.voxels.end(),
                       [&neighbour](const auto &v) {
                         return v.position == neighbour;
                       }) != vox_data.voxels.end()) {
        neighbour_count++;
      }
    }
    //
    // if the voxel has less than 6 neighbours, add it to the hollow_voxels
    // vector
    if (neighbour_count < 6) {
      vox_data.hollow_voxels.push_back(voxel);
    }
  }
}
} // namespace hollow_lantern
