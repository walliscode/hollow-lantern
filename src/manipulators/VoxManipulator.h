/////////////////////////////////////////////////
/// @file
/// @brief Delcaration of VoxManipulator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "ModelData.h"
namespace hollow_lantern {

class VoxManipulator {

private:
  /////////////////////////////////////////////////
  /// @brief Creates a hollowed-out version of the given VoxData and stores it
  /// in the same VoxData object.
  ///
  /// @param vox_data VoxData object to be manipulated.
  /////////////////////////////////////////////////
  void HollowOut(ModelData &model_data);

  /////////////////////////////////////////////////
  /// @brief Generate masks from voxel data and store them in the ModelData
  ///
  /// @param model_data  ModelData object containing voxel data and masks
  /////////////////////////////////////////////////
  void CreateMasks(ModelData &model_data);
  /////////////////////////////////////////////////
  /// @brief Manipulates the mask data to generate triangles
  ///
  /// @param model_data ModelData object containing voxel data and masks
  /////////////////////////////////////////////////
  void GreedyMeshing(ModelData &model_data);

  /////////////////////////////////////////////////
  /// @brief Create triangles from the mask data without any greey meshing
  ///
  /// @param model_data [TODO:parameter]
  /////////////////////////////////////////////////
  void CreateTrianglesFromMask(ModelData &model_data);

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor for VoxManipulator
  /////////////////////////////////////////////////
  VoxManipulator() = default;

  /////////////////////////////////////////////////
  /// @brief A workflow function that collates a set of operations
  ///
  /// @param model_data ModelData needed for the manipulations
  /////////////////////////////////////////////////
  void HollowAndMesh(ModelData &model_data);
  ;
};

} // namespace hollow_lantern
