/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the DataExporter class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DataExporter.h"
#include "directory_paths.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace hollow_lantern {

using json = nlohmann::json;
/////////////////////////////////////////////////
void DataExporter::ExportToJSON(const ModelData &model_data) {

  // construct the export path
  const auto export_path =
      config::getExportFolder() / (model_data.name + ".json");
  // create the export folder if it does not exist
  std::filesystem::create_directories(export_path.parent_path());
  // open the file for writing
  std::ofstream file(export_path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file for writing: " +
                             export_path.string());
  }
  // create the json data from the model data
  json json_data;
  json_data["name"] = model_data.name;
  json_data["projections"] = json::array();

  for (const auto &projection : model_data.projected_data) {
    json projection_json;
    projection_json["triangles"] = json::array();

    for (size_t i = 0; i < projection.getVertexCount(); i++) {

      json vertices = json::array();
      // vertex one of the triangle
      json vertex_one;
      vertex_one["x"] = projection[i].position.x;
      vertex_one["y"] = projection[i].position.y;
      vertex_one["r"] = projection[i].color.r;
      vertex_one["g"] = projection[i].color.g;
      vertex_one["b"] = projection[i].color.b;
      vertex_one["a"] = projection[i].color.a;
      vertices.push_back(vertex_one);

      if (i + 1 >= projection.getVertexCount()) {
        break; // avoid out of bounds access
      }
      i++;
      // vertex two of the triangle
      json vertex_two;
      vertex_two["x"] = projection[i].position.x;
      vertex_two["y"] = projection[i].position.y;
      vertex_two["r"] = projection[i].color.r;
      vertex_two["g"] = projection[i].color.g;
      vertex_two["b"] = projection[i].color.b;
      vertex_two["a"] = projection[i].color.a;
      vertices.push_back(vertex_two);

      if (i + 1 >= projection.getVertexCount()) {
        break; // avoid out of bounds access
      }
      i++;
      // vertex three of the triangle
      json vertex_three;
      vertex_three["x"] = projection[i].position.x;
      vertex_three["y"] = projection[i].position.y;
      vertex_three["r"] = projection[i].color.r;
      vertex_three["g"] = projection[i].color.g;
      vertex_three["b"] = projection[i].color.b;
      vertex_three["a"] = projection[i].color.a;
      vertices.push_back(vertex_three);
      // add the vertices to the projection json
      projection_json["triangles"].push_back(vertices);
    }

    json_data["projections"].push_back(projection_json);
  }

  file << json_data.dump(4); // write the json data to the file
  // check if the file was written successfully
  if (!file) {
    throw std::runtime_error("Failed to write to file: " +
                             export_path.string());
  }
  // close the file
  file.close();
}
} // namespace hollow_lantern
