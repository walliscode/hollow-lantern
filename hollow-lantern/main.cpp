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
  auto model_data_result = vox_reader.ProvideVoxData("cube_one");
  if (!model_data_result) {
    std::cerr << "Failed to load model data." << std::endl;
    return 1;
  }
  // cast to ModelData
  hollow_lantern::ModelData model_data = model_data_result.value();

  // create VoxManipulator instance and hollow out model data
  hollow_lantern::VoxManipulator vox_manipulator;
  vox_manipulator.HollowOut(model_data);

  // create a Projector instance to project the 3D model data onto 2D shapes
  hollow_lantern::Projector projector;
  projector.ProjectOntoPlanes(model_data, 18,
                              hollow_lantern::RotationAxis::Y_AXIS, {0, 0, 0});

  // convenience reference to the projected shapes
  std::vector<sf::VertexArray> &projected_shapes = model_data.projected_data;

  std::cout << "[DEBUG] Projected shapes count: "
            << model_data.projected_data.size() << std::endl;

  // transform the projected shapes to be centered around the middle of the
  // window
  for (auto &shape : projected_shapes) {

    // calculate the bounding box of the shape
    sf::FloatRect bounds = shape.getBounds();
    // calculate the center of the bounding box
    sf::Vector2f center(bounds.size.x / 2, bounds.size.y / 2);
    // translate the shape to center it in the window
    for (size_t i = 0; i < shape.getVertexCount(); ++i) {
      shape[i].position -= center;
      shape[i].position += sf::Vector2f(400, 300); // Center in a 800x600 window
    }
  }
  // create the window
  sf::RenderWindow window(sf::VideoMode({800, 600}), "My window");

  // zoom in on the center of the window
  sf::View view = window.getView();
  view.zoom(1.0f); // Zoom in
  window.setView(view);
  // set the frame rate limit
  window.setFramerateLimit(12);
  // run the program as long as the window is open
  while (window.isOpen()) {
    // check all the window's events that were triggered since the last
    // iteration of the loop
    while (const std::optional event = window.pollEvent()) {
      // "close requested" event: we close the window
      if (event->is<sf::Event::Closed>())
        window.close();
    }

    // clear the window with black color
    window.clear(sf::Color::Black);

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

    // end the current frame
    window.display();
  }
}
