#ifndef _OUTPUTS_HPP
#define _OUTPUTS_HPP

#define GLFW_INCLUDE_NONE
#include "../libraries/glfw/glfw3.h"
#include "../libraries/glm/glm.hpp"
#include "world.hpp"

#include "../libraries/shaderloader.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

class Outputs {
 public:
  static glm::vec3 cameraPos;
  static glm::vec3 cameraFront;
  static glm::vec3 cameraUp;

  static float secondCounter;
  static int frames;
  static int fps;
  static int totalTris;

  static void renderWorld();

  static const int RENDER_DISTANCE = 16;
  static const int vertexStride = 14;

  static unsigned int VBO[World::CHUNKS][World::CHUNKS][12],
      VAO[World::CHUNKS][World::CHUNKS][12];

  static std::vector<float> cursorVertices;
  static std::vector<float> wireframeVertices;
  static unsigned int cursorVAO, cursorVBO;
  static unsigned int hudVAO, hudVBO;
  static unsigned int wireframeVAO, wireframeVBO;

  static const unsigned int SCR_WIDTH = 1280;
  static const unsigned int SCR_HEIGHT = 1024;

  static GLFWwindow *window;
  static Shader *mainShader;
  static Shader *hudShader;
  static Shader *wireframeShader;

  static void bindVertexBuffer(int chunkX, int chunkZ);
  static void prepareTextures();
  static void prepareShaders();
  static void prepareBuffers();
  static bool initOpenGL();
  static void createVertexBuffer(unsigned int &vao, unsigned int &vbo, bool extended = true);

};

#endif