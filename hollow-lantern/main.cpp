/////////////////////////////////////////////////
/// @file
/// @brief Main loop for the Hollow Lantern project.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ModelData.h"
#include "Projector.h"
#include "VoxManipulator.h"
#include "VoxReader.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <iostream>

int main() {
  // create VoxReader instance
  hollow_lantern::VoxReader vox_reader;
  auto model_data_result = vox_reader.ProvideVoxData("colour_cube");
  if (!model_data_result) {
    std::cerr << "Failed to load model data." << std::endl;
    return 1;
  }
  // cast to ModelData
  hollow_lantern::ModelData model_data = model_data_result.value();

  // create VoxManipulator instance and hollow out model data
  hollow_lantern::VoxManipulator vox_manipulator;
  vox_manipulator.HollowAndMesh(model_data);

  std::cout << "[DEBUG] Hollowed and meshed model data." << std::endl;
  // create a Projector instance to project the 3D model data onto 2D shapes
  hollow_lantern::Projector projector;
  // projector.BasicProjection(model_data, {30.f, 0.0f, 0.0f}, 8,
  //                           {0.0f, 1.0f, 0.0f});
  projector.FixedAngleProjection(model_data, {-15, 45.0f, 0.0f});

  std::cout << "[DEBUG] Projected model data." << std::endl;
  // convenience reference to the projected shapes
  std::vector<sf::VertexArray> projected_shapes;
  if (!model_data.triangle_data.empty()) {
    projected_shapes = model_data.triangle_data;
  } else if (!model_data.projected_data.empty()) {
    projected_shapes = model_data.projected_data;
  } else {
    std::cerr << "No projected shapes available." << std::endl;
    return 1;
  }

  std::cout << "[DEBUG] Projected shapes count: "
            << model_data.projected_data.size() << std::endl;

  // transform the projected shapes to be centered around the middle of the
  // window
  for (auto &shape : projected_shapes) {

    // calculate the bounding box of the shape
    sf::FloatRect bounds = shape.getBounds();
    // calculate the center of the bounding box
    sf::Vector2f center(bounds.size.x / 2, bounds.size.y / 2);
    std::cout << "[DEBUG] Number of vertices in shape: "
              << shape.getVertexCount() << std::endl;
    // translate the shape to center it in the window
    for (size_t i = 0; i < shape.getVertexCount(); ++i) {

      shape[i].position -= center;
      shape[i].position += sf::Vector2f(400, 300); // Center in a 800x600 window
      std::cout << "[DEBUG] Vertex " << i << ": " << shape[i].position.x << ", "
                << shape[i].position.y << std::endl;
    }
  }
  // create the window
  sf::RenderWindow window(sf::VideoMode({800, 600}), "My window");

  // zoom in on the center of the window
  sf::View view = window.getView();
  view.zoom(0.25f); // Zoom in
  window.setView(view);
  // set the frame rate limit
  window.setFramerateLimit(2);
  // run the program as long as the window is open
  while (window.isOpen()) {
    // check all the window's events that were triggered since the last
    // iteration of the loop
    while (const std::optional event = window.pollEvent()) {
      // "close requested" event: we close the window
      if (event->is<sf::Event::Closed>())
        window.close();
    }

    // clear the window with grey color
    window.clear(sf::Color(100, 100, 100));

    // draw once of the projected shapes and keep a counter to cycle through
    // them per frame
    static size_t current_shape_index = 0;
    if (current_shape_index < projected_shapes.size()) {
      const auto &shape = projected_shapes[current_shape_index];
      window.draw(shape);
      current_shape_index = (current_shape_index + 1) % projected_shapes.size();
    } else {
      current_shape_index = 0; // Reset if we exceed the number of shapes
      window.draw(projected_shapes[0]); // Draw the first shape
    }

    std::cout << "[DEBUG] Drawing shape index: " << current_shape_index
              << std::endl;

    // end the current frame
    window.display();
  }
}
