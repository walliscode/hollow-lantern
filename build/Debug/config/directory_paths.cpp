#include "directory_paths.h"
#include <filesystem>

namespace config {
////////////////////////////////////////////////////////////
const std::filesystem::path getDataFolder() { return "/home/bwallis/projects/walliscode/hollow-lantern/data"; };

/////////////////////////////////////////////////
const std::filesystem::path getSchemaFolder() {
  return getDataFolder() / "schema";
}

/////////////////////////////////////////////////
const std::filesystem::path getExportFolder() {
  return getDataFolder() / "export";
}
/////////////////////////////////////////////////
const std::filesystem::path getTestDataFolder() {
  return getDataFolder() / "test_data";
}
} // namespace config
