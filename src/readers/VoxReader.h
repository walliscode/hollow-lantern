/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the VoxReader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <expected>
#include <filesystem>
#include <glm/vec3.hpp>
#include <string>
#include <utility>
#include <vector>
namespace hollow_lantern {

/////////////////////////////////////////////////
/// @class VoxData
/// @brief Struct to hold data from a Vox file
///
/////////////////////////////////////////////////
struct VoxData {
  /////////////////////////////////////////////////
  /// @brief Name of the Vox model, taken from the filename
  /////////////////////////////////////////////////
  std::string name{"no_name"};

  /////////////////////////////////////////////////
  /// @brief Each pair contains a voxel position and its color
  /////////////////////////////////////////////////
  std::vector<std::pair<glm::vec3, sf::Color>> voxels;

  /////////////////////////////////////////////////
  /// @brief Hollowed out version ofthe voxels member
  /////////////////////////////////////////////////
  std::vector<std::pair<glm::vec3, sf::Color>> hollow_voxels;
};

class VoxReader {
private:
  /////////////////////////////////////////////////
  /// @brief Simple function to check if a Vox file exists
  ///
  /// @param model_name name of the file without extension
  /// @return Boolean indicating if the file exists
  /////////////////////////////////////////////////
  bool CheckVoxFileExists(const std::filesystem::path &model_path) const;

  /////////////////////////////////////////////////
  /// @brief Helper function to read a single byte from the file
  ///
  /// @param file binary file stream to read from
  /// @return An unsigned 8-bit integer read from the file
  /////////////////////////////////////////////////
  uint8_t ReadU8(std::ifstream &file) const;

  /////////////////////////////////////////////////
  /// @brief Helper function to read a 32-bit unsigned integer from the file
  ///
  /// @param file Binary file stream to read from
  /// @return An unsigned 32-bit integer read from the file
  /////////////////////////////////////////////////
  uint32_t ReadU32(std::ifstream &file) const;

  /////////////////////////////////////////////////
  /// @brief Checks header matches that given by the MagicaVoxel format
  ///
  /// @return Returns true if the header matches, false otherwise
  /////////////////////////////////////////////////
  bool CheckVoxHeader(std::ifstream &file) const;

  /////////////////////////////////////////////////
  /// @brief Checks if the file contains a PACK chunk
  ///
  /// @param file Vox stream to check
  /// @return Bolean indicating if the PACK chunk exists
  /////////////////////////////////////////////////
  bool CheckPackChunk(std::ifstream &file) const;

  /////////////////////////////////////////////////
  /// @brief Checks if the file contains a MAIN chunk
  ///
  /// @param file Vox stream to check
  /// @return Boolean indicating if the MAIN chunk exists
  /////////////////////////////////////////////////
  bool CheckMainChunk(std::ifstream &file) const;

  /////////////////////////////////////////////////
  /// @brief Takes a file stream and extracts voxel data from it
  ///
  /// @param file Vox stream to read from
  /// @param vox_data VoxData object to fill with extracted data
  /////////////////////////////////////////////////
  void ExtractVoxels(std::ifstream &file, VoxData &vox_data) const;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor of the VoxReader class
  /////////////////////////////////////////////////
  VoxReader() = default;

  /////////////////////////////////////////////////
  /// @brief Provide a VoxData object from file
  ///
  /// @return A VoxData object or bool indicating failure
  /////////////////////////////////////////////////
  std::expected<VoxData, std::string> ProvideVoxData(std::string model_name,
                                                     bool testing = false);
};

} // namespace hollow_lantern
