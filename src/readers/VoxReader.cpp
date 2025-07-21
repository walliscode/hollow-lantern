////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the VoxReader class
/////////////////////////////////////////////////

#include "VoxReader.h"
#include "directory_paths.h"
#include <SFML/Graphics/Color.hpp>
#include <cstdint>
#include <cstring>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>

namespace hollow_lantern {

static const uint32_t default_palette[256] = {
    0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff,
    0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff,
    0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff, 0xff6699ff, 0xff3399ff,
    0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff,
    0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff,
    0xff0033ff, 0xffff00ff, 0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff,
    0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc,
    0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
    0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc,
    0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc,
    0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc, 0xff6633cc, 0xff3333cc,
    0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc,
    0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99,
    0xff00ff99, 0xffffcc99, 0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99,
    0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999,
    0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
    0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399,
    0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099,
    0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66, 0xff66ff66, 0xff33ff66,
    0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66,
    0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966,
    0xff009966, 0xffff6666, 0xffcc6666, 0xff996666, 0xff666666, 0xff336666,
    0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366,
    0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
    0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33,
    0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33,
    0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933, 0xff669933, 0xff339933,
    0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633,
    0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333,
    0xff003333, 0xffff0033, 0xffcc0033, 0xff990033, 0xff660033, 0xff330033,
    0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00,
    0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
    0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900,
    0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600,
    0xff006600, 0xffff3300, 0xffcc3300, 0xff993300, 0xff663300, 0xff333300,
    0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000,
    0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077,
    0xff000055, 0xff000044, 0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00,
    0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400,
    0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
    0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000,
    0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777,
    0xff555555, 0xff444444, 0xff222222, 0xff111111};

/////////////////////////////////////////////////
std::expected<ModelData, std::string>
VoxReader::ProvideVoxData(std::string model_name, bool testing) {
  // create path to the model file
  std::filesystem::path model_path;

  if (testing) {
    model_path = config::getTestDataFolder() / "vox" / (model_name + ".vox");
  } else {
    model_path = config::getDataFolder() / "vox" / (model_name + ".vox");
  }

  if (!CheckVoxFileExists(model_path)) {
    std::cerr << "[DEBUG] File not found: " << model_path << std::endl;
    return std::unexpected(
        format("Vox file '{}.vox' does not exist.", model_name));
  }

  std::cout << "[DEBUG] File size (filesystem): "
            << std::filesystem::file_size(model_path) << " bytes." << std::endl;

  // Open a fresh stream for each function to avoid bad state
  {
    std::ifstream file(model_path, std::ios::binary);
    if (!file) {
      std::cerr << "[DEBUG] Could not open file for header check." << std::endl;
      return std::unexpected(
          format("Failed to open file '{}.vox'.", model_name));
    }
    if (!CheckVoxHeader(file)) {
      std::cerr << "[DEBUG] Header check failed." << std::endl;
      return std::unexpected(
          format("Vox file '{}.vox' has an invalid header.", model_name));
    }
  }
  {
    std::ifstream file(model_path, std::ios::binary);
    if (!file) {
      std::cerr << "[DEBUG] Could not open file for MAIN chunk check."
                << std::endl;
      return std::unexpected(
          format("Failed to open file '{}.vox'.", model_name));
    }
    if (!CheckMainChunk(file)) {
      std::cerr << "[DEBUG] MAIN chunk not found." << std::endl;
      return std::unexpected(format(
          "Vox file '{}.vox' does not contain a MAIN chunk.", model_name));
    }
  }

  std::ifstream file(model_path, std::ios::binary);
  if (!file) {
    std::cerr << "[DEBUG] Could not open file for PACK chunk check."
              << std::endl;
    return std::unexpected(format("Failed to open file '{}.vox'.", model_name));
  }
  if (CheckPackChunk(file)) {
    std::cerr << "[DEBUG] Found PACK chunk (multiple models not supported)."
              << std::endl;
    return std::unexpected(
        format("Vox file '{}.vox' contains a PACK chunk, multiple models are "
               "not currently supported.",
               model_name));
  }

  ModelData model_data;
  model_data.name = model_name;
  {
    std::ifstream file(model_path, std::ios::binary);
    if (!file) {
      std::cerr << "[DEBUG] Could not open file for voxel extraction."
                << std::endl;
      return std::unexpected(
          format("Failed to open file '{}.vox'.", model_name));
    }
    ExtractVoxels(file, model_data);
  }
  std::cout << "[DEBUG] Model size: " << model_data.size.x << "x"
            << model_data.size.y << "x" << model_data.size.z << std::endl;

  return model_data;
}

/////////////////////////////////////////////////
bool VoxReader::CheckVoxFileExists(
    const std::filesystem::path &model_path) const {
  return std::filesystem::exists(model_path);
}

/////////////////////////////////////////////////
uint8_t VoxReader::ReadU8(std::ifstream &file) const {
  uint8_t value = 0;
  file.read(reinterpret_cast<char *>(&value), sizeof(value));
  if (!file)
    std::cerr << "[DEBUG] ReadU8 failed at pos " << file.tellg() << std::endl;
  return value;
}

/////////////////////////////////////////////////
uint32_t VoxReader::ReadU32(std::ifstream &file) const {
  uint32_t value = 0;
  file.read(reinterpret_cast<char *>(&value), sizeof(value));
  if (!file)
    std::cerr << "[DEBUG] ReadU32 failed at pos " << file.tellg() << std::endl;
  return value;
}

/////////////////////////////////////////////////
bool VoxReader::CheckVoxHeader(std::ifstream &file) const {
  char header[4] = {};
  file.read(header, 4);
  if (!file)
    std::cerr << "[DEBUG] Read header failed." << std::endl;
  std::cout << "[DEBUG] Header: " << header << std::endl;
  return (std::strncmp(header, "VOX ", 4) == 0);
}

/////////////////////////////////////////////////
bool VoxReader::CheckPackChunk(std::ifstream &file) const {
  file.seekg(8, std::ios::beg);
  char chunk_ID[4];
  while (file.read(chunk_ID, 4)) {
    if (!file) {
      std::cerr << "[DEBUG] PACK: Failed reading chunk_ID." << std::endl;
      break;
    }
    uint32_t chunk_size = ReadU32(file);
    uint32_t child_chunks = ReadU32(file);
    if (std::strncmp(chunk_ID, "PACK", 4) == 0) {
      return true;
    }
    file.seekg(chunk_size + child_chunks, std::ios::cur);
  }
  return false;
}

/////////////////////////////////////////////////
bool VoxReader::CheckMainChunk(std::ifstream &file) const {
  file.seekg(8, std::ios::beg);
  char chunk_ID[4];
  while (file.read(chunk_ID, 4)) {
    if (!file) {
      std::cerr << "[DEBUG] MAIN: Failed reading chunk_ID." << std::endl;
      break;
    }
    uint32_t chunk_size = ReadU32(file);
    uint32_t child_chunks = ReadU32(file);
    if (std::strncmp(chunk_ID, "MAIN", 4) == 0) {
      return true;
    }
    file.seekg(chunk_size + child_chunks, std::ios::cur);
  }
  return false;
}

/////////////////////////////////////////////////
void VoxReader::ExtractVoxels(std::ifstream &file,
                              ModelData &model_data) const {
  // Start with default palette
  uint32_t palette[256];
  std::memcpy(palette, default_palette, sizeof(palette));
  bool found_rgba = false;

  file.seekg(8, std::ios::beg);

  // Find RGBA chunk
  char chunk_ID[4];
  while (file.read(chunk_ID, 4)) {
    if (!file) {
      std::cerr << "[DEBUG] Failed reading chunk_ID for RGBA scan."
                << std::endl;
      break;
    }
    uint32_t chunk_size = ReadU32(file);
    uint32_t child_chunks = ReadU32(file);

    if (std::strncmp(chunk_ID, "RGBA", 4) == 0) {
      found_rgba = true;
      std::cout << "[DEBUG] Found RGBA chunk, reading palette colors..."
                << std::endl;
      // As per spec, color [0-254] are mapped to palette[1-255]
      for (int i = 1; i <= 255; ++i) {
        uint8_t r = ReadU8(file);
        uint8_t g = ReadU8(file);
        uint8_t b = ReadU8(file);
        uint8_t a = ReadU8(file);
        palette[i] = (a << 24) | (b << 16) | (g << 8) | r;
      }
      break; // after RGBA, go back to chunk scan for XYZI
    } else {
      file.seekg(chunk_size + child_chunks, std::ios::cur);
    }
  }

  // Now find MAIN chunk
  file.clear();
  file.seekg(8, std::ios::beg);
  while (file.read(chunk_ID, 4)) {
    if (!file) {
      std::cerr << "[DEBUG] Failed reading chunk_ID for MAIN scan."
                << std::endl;
      break;
    }
    uint32_t chunk_size = ReadU32(file);
    uint32_t child_chunks = ReadU32(file);
    if (std::strncmp(chunk_ID, "MAIN", 4) == 0) {
      std::cout << "[DEBUG] Found MAIN chunk." << std::endl;
      break; // After MAIN, go back to chunk scan for XYZI
    } else {
      file.seekg(chunk_size + child_chunks, std::ios::cur);
    }
  }

  // Now MAIN: read all children
  while (file.read(chunk_ID, 4)) {
    if (!file) {
      std::cerr << "[DEBUG] Failed reading chunk_ID for children scan."
                << std::endl;
      break;
    }
    uint32_t chunk_size = ReadU32(file);
    uint32_t child_chunks = ReadU32(file);
    std::streampos next_chunk = file.tellg();
    next_chunk += chunk_size;

    if (std::strncmp(chunk_ID, "SIZE", 4) == 0) {
      uint32_t x = ReadU32(file);
      uint32_t y = ReadU32(file);
      uint32_t z = ReadU32(file);
      model_data.size = sf::Vector3i(x, y, z);

      // Resize voxel_data to match the model size
      model_data.voxel_data.resize(x);
      for (int i = 0; i < x; ++i) {
        model_data.voxel_data[i].resize(y);
        for (int j = 0; j < y; ++j) {
          model_data.voxel_data[i][j].resize(z);
        }
      }
      std::cout << "[DEBUG] Found SIZE chunk: " << x << "x" << y << "x" << z
                << std::endl;

    } else if (std::strncmp(chunk_ID, "XYZI", 4) == 0) {
      uint32_t num_voxels = ReadU32(file);
      std::cout << "[DEBUG] Found XYZI chunk with " << num_voxels << " voxels."
                << std::endl;
      for (uint32_t i = 0; i < num_voxels; ++i) {
        uint32_t x = ReadU8(file);
        uint32_t y = ReadU8(file);
        uint32_t z = ReadU8(file);

        // [DEBUG] print voxel indices before accessing array
        std::cout << "[DEBUG] Voxel indices: (" << x << "," << y << "," << z
                  << ")" << std::endl;

        uint8_t color_index = ReadU8(file);

        if (color_index > 255) {
          std::cerr << "[DEBUG] Color index out of bounds: " << (int)color_index
                    << std::endl;
          color_index = 0;
        }

        std::uint32_t color32 = palette[color_index];
        sf::Color color(color32 & 0xFF, (color32 >> 8) & 0xFF,
                        (color32 >> 16) & 0xFF, (color32 >> 24) & 0xFF);

        // [DEBUG] check bounds before assignment
        if (x >= model_data.size.x || y >= model_data.size.y ||
            z >= model_data.size.z) {
          std::cerr << "[DEBUG] Voxel position (" << x << "," << y << "," << z
                    << ") out of bounds!" << std::endl;
          continue; // skip out-of-bounds
        }
        std::cout << "[DEBUG] Assigning color: " << std::hex << color32
                  << std::dec << " to voxel at (" << x << ", " << y << ", " << z
                  << ")" << std::endl;
        model_data.voxel_data[x][y][z].color = color;
        std::cout << "assigning visible flag to voxel at (" << x << ", " << y
                  << ", " << z << ")" << std::endl;
        model_data.voxel_data[x][y][z].is_visible = true;
        std::cout << "[DEBUG] Voxel at (" << x << ", " << y << ", " << z
                  << ") with color: " << std::hex << color32 << std::dec
                  << std::endl;
      }
      break;
    } else {
      file.seekg(next_chunk);
    }
  }
}

} // namespace hollow_lantern
