#include <iostream>
#include <algorithm>

#include "generators.hpp"
#include "outputs.hpp"
#include "inputs.hpp"
#include "tris.hpp"
#include "world.hpp"

// ------------------------------------------------

int main() {
  if (!Outputs::initOpenGL()) return -1;

  Outputs::prepareBuffers();

  World::loadMap();
  Generators::generateVertices();

  Outputs::prepareTextures();
  Outputs::prepareShaders();

  // -------------------------------------------------------------------------------

  float deltaTime = 0;  
  float lastTime = static_cast<float>(glfwGetTime());

  while (!glfwWindowShouldClose(Outputs::window)) {
    float currentTime = static_cast<float>(glfwGetTime());
    auto deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    Inputs::processInput(Outputs::window, deltaTime);

    Inputs::evaluateCursor();

    Outputs::renderWorld();

    Outputs::secondCounter += deltaTime;
    Outputs::frames++;

    if (Outputs::secondCounter >= 1.0f) {
      Outputs::secondCounter -= 1.0f;
      Outputs::fps = Outputs::frames;
      Outputs::frames = 0;
      std::cout << Outputs::fps << " FPS, " << Outputs::totalTris << " Tris."
                << std::endl;
    }

    glfwSwapBuffers(Outputs::window);
    glfwPollEvents();
  }

  World::saveMap();

  delete Outputs::mainShader;
  glfwTerminate();
  return 0;
}

// -------------------------------------------------------------------------------
