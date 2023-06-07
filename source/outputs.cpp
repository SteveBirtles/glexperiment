#include <vector>

#include "outputs.hpp"
#include "generators.hpp"
#include "inputs.hpp"
#include "tris.hpp"
#include "world.hpp"

#include "../libraries/glad/glad.h"
#include "../libraries/glm/glm.hpp"
#include "../libraries/glm/gtc/matrix_transform.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "../libraries/imageloader.h"
#endif

GLFWwindow *Outputs::window;
Shader *Outputs::mainShader;
Shader *Outputs::hudShader;
Shader *Outputs::wireframeShader;

unsigned int Outputs::VBO[World::CHUNKS][World::CHUNKS][12],
    Outputs::VAO[World::CHUNKS][World::CHUNKS][12];
std::vector<float> Outputs::cursorVertices;
std::vector<float> Outputs::wireframeVertices;
unsigned int Outputs::cursorVAO, Outputs::cursorVBO;
unsigned int Outputs::hudVAO, Outputs::hudVBO;
unsigned int Outputs::wireframeVAO, Outputs::wireframeVBO;

void Outputs::bindVertexBuffer(int chunkX, int chunkZ)
{
  for (int i = 0; i < 12; i++)
  {
    glBindBuffer(GL_ARRAY_BUFFER, VBO[chunkX][chunkZ][i]);
    glBufferData(GL_ARRAY_BUFFER,

                 World::vertices[chunkX][chunkZ][i].size() * sizeof(float),
                 &World::vertices[chunkX][chunkZ][i].front(), GL_STATIC_DRAW);
  }
}

void Outputs::prepareTextures()
{
  std::string textureFile[16]{
      "cursor.png",
      "cursorCentre.png",
      "cursorCorner.png",
      "cursorEdge.png",
      "1.png",
      "2.png",
      "3.png",
      "4.png",
      "5.png",
      "6.png",
      "7.png",
      "8.png",
      "9.png",
      "10.png",
      "11.png",
      "12.png",
  };

  unsigned int textures[16];

  for (int i = 0; i < 16; i++)
  {
    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data =
        stbi_load(std::string("assets/" + textureFile[i]).c_str(), &width,
                  &height, &nrChannels, 0);

    std::cout << textureFile[i] << ":" << width << " x " << height << " with "
              << nrChannels << " channels." << std::endl;

    if (data)
    {
      glGenTextures(1, &textures[i]);

      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, textures[i]);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                   GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
      std::cout << "Failed to load texture!" << std::endl;
    }
    stbi_image_free(data);
  }
}

void Outputs::prepareShaders()
{

  #ifdef ES_MODE
  std::string glslExtension{".es.glsl"};
  #else
  std::string glslExtension{".glsl"};
  #endif

  auto mainShaderV = "shaders/textured_vertex" + glslExtension;
  auto mainShaderF = "shaders/textured_fragment" + glslExtension;
  mainShader = new Shader(mainShaderV.c_str(), mainShaderF.c_str());

  mainShader->use();

  auto hudShaderV = "shaders/hud_vertex" + glslExtension;
  auto hudShaderF = "shaders/white_fragment" + glslExtension;

  hudShader = new Shader(hudShaderV.c_str(), hudShaderF.c_str());

  auto wireframeShaderV = "shaders/wireframe_vertex" + glslExtension;
  auto wireframeShaderF = "shaders/white_fragment" + glslExtension;
  wireframeShader = new Shader(wireframeShaderV.c_str(), wireframeShaderF.c_str());
}

bool Outputs::initOpenGL()
{
  if (!glfwInit())
  {
    std::cout << "Failed to initiate glfw." << std::endl;
    return false;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

  #ifdef ES_MODE
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  #else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  #endif

  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GLFW Fun", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return false;
  }
  return true;
}

void Outputs::createVertexBuffer(unsigned int &vao, unsigned int &vbo,
                                 bool extended)
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        (extended ? Outputs::vertexStride : 3) * sizeof(float),
                        (void *)0);

  if (extended)
  {
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          Outputs::vertexStride * sizeof(float),
                          (void *)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                          Outputs::vertexStride * sizeof(float),
                          (void *)(5 * sizeof(float)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                          Outputs::vertexStride * sizeof(float),
                          (void *)(8 * sizeof(float)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
                          Outputs::vertexStride * sizeof(float),
                          (void *)(11 * sizeof(float)));
  }
}

void Outputs::prepareBuffers()
{
  for (int chunkX = 0; chunkX < World::CHUNKS; chunkX++)
  {
    for (int chunkZ = 0; chunkZ < World::CHUNKS; chunkZ++)
    {
      for (int i = 0; i < 12; i++)
      {
        createVertexBuffer(VAO[chunkX][chunkZ][i], VBO[chunkX][chunkZ][i]);
      }
    }
  }

  createVertexBuffer(cursorVAO, cursorVBO);
  createVertexBuffer(wireframeVAO, wireframeVBO, false);

#if __APPLE__
  std::vector<float> hudVertices{-50.0 / (float)(SCR_WIDTH * 1.25),
                                 0,
                                 50.0 / (float)(SCR_WIDTH * 1.25),
                                 0,
                                 0,
                                 -50.0 / (float)SCR_HEIGHT,
                                 0,
                                 50.0 / (float)SCR_HEIGHT};
#else
  std::vector<float> hudVertices{
      -50.0 / (float)SCR_WIDTH, 0, 50.0 / (float)SCR_WIDTH, 0, 0,
      -50.0 / (float)SCR_HEIGHT, 0, 50.0 / (float)SCR_HEIGHT};
#endif

  glGenVertexArrays(1, &hudVAO);
  glBindVertexArray(hudVAO);

  glGenBuffers(1, &hudVBO);
  glBindBuffer(GL_ARRAY_BUFFER, hudVBO);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);

  glBufferData(GL_ARRAY_BUFFER, sizeof(hudVertices) * sizeof(float),
               hudVertices.data(), GL_STATIC_DRAW);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

glm::vec3 Outputs::cameraPos =
    glm::vec3(World::MAP_SIZE / 2, 12, World::MAP_SIZE / 2);
glm::vec3 Outputs::cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 Outputs::cameraUp = glm::vec3(0.0, 1.0, 0.0);

float Outputs::secondCounter = 0;
int Outputs::frames = 0;
int Outputs::fps = 0;
int Outputs::totalTris = 0;

void Outputs::renderWorld()
{
  glClearColor(0.4f, 0.4f, 0.8f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDepthFunc(GL_LESS);

  glDisable(GL_BLEND);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  Outputs::mainShader->use();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // #if __APPLE__
  //   glm::mat4 projection = glm::perspective(
  //       glm::radians(Inputs::fov), (float)(SCR_WIDTH * 1.25) / (float)SCR_HEIGHT,
  //       0.1f, 10000.0f);
  // #else
  glm::mat4 projection = glm::perspective(
      glm::radians(Inputs::fov),
      (float)Outputs::SCR_WIDTH / (float)Outputs::SCR_HEIGHT, 0.1f, 10000.0f);
  // #endif

  Outputs::mainShader->setMat4("projection", projection);

  glm::mat4 view =
      glm::lookAt(Outputs::cameraPos, Outputs::cameraPos + Outputs::cameraFront,
                  Outputs::cameraUp);
  Outputs::mainShader->setMat4("view", view);

  Outputs::mainShader->setVec3("sun", Outputs::cameraPos);
  Outputs::mainShader->setBool("illuminate", true);

  glm::mat4 model = glm::mat4(1.0f);
  Outputs::mainShader->setMat4("model", model);

  const float chunkSize = (float)World::MAP_SIZE / World::CHUNKS;
  int camChunkX = (int)(Outputs::cameraPos.x / chunkSize);
  int camChunkZ = (int)(Outputs::cameraPos.z / chunkSize);

  Outputs::mainShader->setVec4("fogColour", 0.4f, 0.4f, 0.8f, 1.0f);

  totalTris = 0;
  for (int i = 0; i < 12; i++)
  {
    Outputs::mainShader->setInt("textureNo", i + Inputs::FIRST_TEXTURE);
    for (int chunkX = 0; chunkX < World::CHUNKS; chunkX++)
    {
      for (int chunkZ = 0; chunkZ < World::CHUNKS; chunkZ++)
      {
        if (std::abs(chunkX - camChunkX) > Outputs::RENDER_DISTANCE ||
            std::abs(chunkZ - camChunkZ) > Outputs::RENDER_DISTANCE)
          continue;

        glBindVertexArray(Outputs::VAO[chunkX][chunkZ][i]);
        glDrawArrays(
            GL_TRIANGLES, 0,
            World::vertices[chunkX][chunkZ][i].size() / Outputs::vertexStride);
        totalTris +=
            World::vertices[chunkX][chunkZ][i].size() / Outputs::vertexStride;
      }
    }
  }

  if (Inputs::cursorSide != -1 &&
      glfwGetKey(Outputs::window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
  {
    glm::mat4 cursorModel = glm::mat4(1.0f);
    Outputs::mainShader->setMat4("model", cursorModel);
    Outputs::mainShader->setBool("illuminate", false);

    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    auto s =
        World::voxels[Inputs::cursorX][Inputs::cursorY][Inputs::cursorZ] > 0
            ? 255
            : 0;

    for (auto t = 0; t < Tris::shapes[s].size(); t++)
    {
      Outputs::cursorVertices.clear();

      auto triId = Tris::shapes[s][t];

      auto skip = false;

      if (triId < 24)
      {
        auto adjacentTriVector = Tris::adjacents[triId];
        auto dx = -adjacentTriVector.x;
        auto dy = -adjacentTriVector.y;
        auto dz = -adjacentTriVector.z;
        auto adjacentTri = adjacentTriVector.w;

        if (Inputs::cursorX + dx >= 0 &&
            Inputs::cursorX + dx < World::MAP_SIZE &&
            Inputs::cursorY + dy >= 0 &&
            Inputs::cursorY + dy < World::MAP_SIZE &&
            Inputs::cursorZ + dz >= 0 &&
            Inputs::cursorZ + dz < World::MAP_SIZE)
        {
          auto adjacentShape =
              World::voxels[Inputs::cursorX + dx][Inputs::cursorY + dy]
                           [Inputs::cursorZ + dz];
          for (auto t2 = 0; t2 < Tris::shapes[adjacentShape].size(); t2++)
          {
            if (Tris::shapes[adjacentShape][t2] == adjacentTri)
            {
              skip = true;
              break;
            }
          }
        }
      }

      if (skip)
        continue;

      auto mainSide = true;

      switch (Inputs::cursorSide)
      {
      case 0: //"North"
        if (!(triId >= 16 && triId <= 19))
          mainSide = false;
        break;
      case 1: //"South"
        if (!(triId >= 8 && triId <= 11))
          mainSide = false;
        break;
      case 2: //"Bottom"
        if (!(triId >= 0 && triId <= 3))
          mainSide = false;
        break;
      case 3: //"Top"
        if (!(triId >= 20 && triId <= 23))
          mainSide = false;
        break;
      case 4: //"East"
        if (!(triId >= 4 && triId <= 7))
          mainSide = false;
        break;
      case 5: //"West"
        if (!(triId >= 12 && triId <= 15))
          mainSide = false;
        break;
      }

      auto prototype = Tris::prototypes[triId];
      auto tri = Tris(prototype, Tris::textures[triId]);
      for (auto v = 0; v < 3; v++)
      {
        Outputs::cursorVertices.push_back(tri.xyz[v].x + Inputs::cursorX);
        Outputs::cursorVertices.push_back(tri.xyz[v].y + Inputs::cursorY);
        Outputs::cursorVertices.push_back(tri.xyz[v].z + Inputs::cursorZ);

        float p = tri.uv[v].x, q = tri.uv[v].y;
        float x, y;

        if (Inputs::cursorCorner == 0 || Inputs::cursorEdge == 2)
        {
          x = -p;
          y = -q;
        }
        else if (Inputs::cursorCorner == 1 || Inputs::cursorEdge == 1)
        {
          x = q;
          y = -p;
        }
        else if (Inputs::cursorCorner == 2 || Inputs::cursorEdge == 0)
        {
          x = -q;
          y = p;
        }
        else
        { // cursorCorner == 3 || cursorEdge == 3
          x = p;
          y = q;
        }

        Outputs::cursorVertices.push_back(x);
        Outputs::cursorVertices.push_back(y);

        Outputs::cursorVertices.push_back(tri.normal.x);
        Outputs::cursorVertices.push_back(tri.normal.y);
        Outputs::cursorVertices.push_back(tri.normal.z);
        Outputs::cursorVertices.push_back(tri.centre.x + Inputs::cursorX);
        Outputs::cursorVertices.push_back(tri.centre.y + Inputs::cursorY);
        Outputs::cursorVertices.push_back(tri.centre.z + Inputs::cursorZ);

        Outputs::cursorVertices.push_back(1.0f);
        Outputs::cursorVertices.push_back(1.0f);
        Outputs::cursorVertices.push_back(1.0f);
      }

      glBindBuffer(GL_ARRAY_BUFFER, Outputs::cursorVBO);
      glBufferData(GL_ARRAY_BUFFER,
                   Outputs::cursorVertices.size() * sizeof(float),
                   &Outputs::cursorVertices.front(), GL_STATIC_DRAW);

      if (!mainSide)
      {
        Outputs::mainShader->setInt("textureNo", 0);
      }
      else if (Inputs::cursorCorner >= 0)
      {
        Outputs::mainShader->setInt("textureNo", 2);
      }
      else if (Inputs::cursorEdge >= 0)
      {
        Outputs::mainShader->setInt("textureNo", 3);
      }
      else
      {
        Outputs::mainShader->setInt("textureNo", 1);
      }

      glBindVertexArray(Outputs::cursorVAO);
      glDrawArrays(GL_TRIANGLES, 0,
                   Outputs::cursorVertices.size() / Outputs::vertexStride);
    }
  }

  glDisable(GL_CULL_FACE);

  if (Inputs::cursorSide != -1)
  {
    Outputs::wireframeShader->use();

    Outputs::wireframeShader->setMat4("projection", projection);
    Outputs::wireframeShader->setMat4("view", view);
    Outputs::wireframeShader->setMat4("model", model);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  Outputs::hudShader->use();
  glDisable(GL_BLEND);

  glDisable(GL_DEPTH_TEST);

  glBindVertexArray(Outputs::hudVAO);
  glDrawArrays(GL_LINES, 0, 4);
}
