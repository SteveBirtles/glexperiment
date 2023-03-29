#include <iostream>
#include <cmath>
#include <bitset>

#include "world.hpp"
#include "tris.hpp"
#include "inputs.hpp"
#include "outputs.hpp"
#include "generators.hpp"

#define GLFW_INCLUDE_NONE
#include "../libraries/glfw/glfw3.h"

int Inputs::cursorX;
int Inputs::cursorY;
int Inputs::cursorZ;
int Inputs::cursorSide;
int Inputs::cursorEdge;
int Inputs::cursorCorner;
bool Inputs::cursorPassThrough;

int Inputs::lastCursorX = -1;
int Inputs::lastCursorY = -1;
int Inputs::lastCursorZ = -1;
int Inputs::lastCursorSide = -1;

float Inputs::cursorFracX;
float Inputs::cursorFracY;
float Inputs::cursorFracZ;
float Inputs::cursorFracU;
float Inputs::cursorFracV;

float Inputs::yaw = -90.0;
float Inputs::pitch = 0.0;
float Inputs::lastX = 0;
float Inputs::lastY = 0;
float Inputs::fov = 45.0;

float Inputs::cameraSpeed = 0;

bool Inputs::lastLeftMouseButton = false;
bool Inputs::lastRightMouseButton = false;
bool Inputs::firstMouse = true;

std::string Inputs::cursorSideName[6]{"North", "South", "Bottom",
                              "Top",   "East",  "West"};

void Inputs::evaluateCursor() {
  const double infinity = 1 >> 16;

  lastCursorX = cursorX;
  lastCursorY = cursorY;
  lastCursorZ = cursorZ;
  lastCursorSide = cursorSide;

  cursorX = -1;
  cursorY = -1;
  cursorZ = -1;
  cursorSide = -1;
  cursorEdge = -1;
  cursorCorner = -1;

  bool passedThroughNeeded = cursorPassThrough;

  double dx = Outputs::cameraFront.x;
  double dy = Outputs::cameraFront.y;
  double dz = Outputs::cameraFront.z;

  double stepX = Outputs::cameraPos.x - dx;
  double stepY = Outputs::cameraPos.y - dy;
  double stepZ = Outputs::cameraPos.z - dz;

  for (int steps = 0; steps < 3 * World::MAP_SIZE; steps++) {
    double toXedge = infinity;
    double toYedge = infinity;
    double toZedge = infinity;

    double fracX, fracY, fracZ, intpart;

    fracX = modf(stepX, &intpart);
    fracY = modf(stepY, &intpart);
    fracZ = modf(stepZ, &intpart);

    if (fracX == 0 && dx != 0) {
      toXedge = abs(1 / dx);
    } else {
      if (dx > 0) {
        toXedge = (1 - fracX) / dx;
      } else if (dx < 0) {
        toXedge = -fracX / dx;
      }
    }

    if (fracY == 0 && dy != 0) {
      toYedge = abs(1 / dy);
    } else {
      if (dy > 0) {
        toYedge = (1 - fracY) / dy;
      } else if (dy < 0) {
        toYedge = -fracY / dy;
      }
    }

    if (fracZ == 0 && dz != 0) {
      toZedge = abs(1 / dz);
    } else {
      if (dz > 0) {
        toZedge = (1 - fracZ) / dz;
      } else if (dz < 0) {
        toZedge = -fracZ / dz;
      }
    }

    if (toXedge == infinity && toYedge == infinity && toZedge == infinity) {
      break;
    }

    if (toXedge <= toYedge && toXedge <= toZedge) {
      stepX += dx * toXedge;
      stepY += dy * toXedge;
      stepZ += dz * toXedge;
    } else if (toYedge <= toXedge && toYedge <= toZedge) {
      stepX += dx * toYedge;
      stepY += dy * toYedge;
      stepZ += dz * toYedge;
    } else if (toZedge <= toXedge && toZedge <= toYedge) {
      stepX += dx * toZedge;
      stepY += dy * toZedge;
      stepZ += dz * toZedge;
    }

    int gridX = floor(stepX + dx * 0.001);
    int gridY = floor(stepY + dy * 0.001);
    int gridZ = floor(stepZ + dz * 0.001);

    if (gridX >= 0 && gridY >= 0 && gridZ >= 0 && gridX < World::MAP_SIZE &&
        gridY < World::MAP_SIZE && gridZ < World::MAP_SIZE) {
      if (World::voxels[gridX][gridY][gridZ] > 0) {
        if (passedThroughNeeded) {
          passedThroughNeeded = false;
        } else {
          cursorX = gridX;
          cursorY = gridY;
          cursorZ = gridZ;

          cursorFracX = modf(stepX, &intpart);
          cursorFracY = modf(stepY, &intpart);
          cursorFracZ = modf(stepZ, &intpart);

          if (cursorFracX < cursorFracY && cursorFracX < cursorFracZ) {
            if (cursorX == floor(stepX))
              cursorSide = 0;  // "North";
            else
              cursorSide = 1;  // "South";
          } else if (cursorFracY < cursorFracX && cursorFracY < cursorFracZ) {
            if (cursorY == floor(stepY))
              cursorSide = 2;  // "Bottom";
            else
              cursorSide = 3;  // "Top";
          } else if (cursorFracZ < cursorFracX && cursorFracZ < cursorFracY) {
            if (cursorZ == floor(stepZ))
              cursorSide = 4;  // "East";
            else
              cursorSide = 5;  // "West";
          } else {
            cursorX = -1;
            return;
          }

          switch (cursorSide) {
            case 0:
              cursorFracU = 1 - cursorFracZ;
              cursorFracV = cursorFracY;
              break;
            case 1:
              cursorFracU = cursorFracZ;
              cursorFracV = cursorFracY;
              break;
            case 2:
              cursorFracU = 1 - cursorFracX;
              cursorFracV = cursorFracZ;
              break;
            case 3:
              cursorFracU = cursorFracX;
              cursorFracV = cursorFracZ;
              break;
            case 4:
              cursorFracU = cursorFracX;
              cursorFracV = cursorFracY;
              break;
            case 5:
              cursorFracU = 1 - cursorFracX;
              cursorFracV = cursorFracY;
              break;
          }

          if (cursorFracU <= 0.33333 && cursorFracV <= 0.33333)
            cursorCorner = 0;
          else if (cursorFracU > 0.66666 && cursorFracV <= 0.33333)
            cursorCorner = 1;
          else if (cursorFracU <= 0.33333 && cursorFracV > 0.66666)
            cursorCorner = 2;
          else if (cursorFracU > 0.66666 && cursorFracV > 0.66666)
            cursorCorner = 3;
          else if (cursorFracU <= 0.33333)
            cursorEdge = 0;
          else if (cursorFracU > 0.66666)
            cursorEdge = 1;
          else if (cursorFracV <= 0.33333)
            cursorEdge = 2;
          else if (cursorFracV > 0.66666)
            cursorEdge = 3;

          return;
        }
      }
    }
  }
}

void processMouseClick(uint8_t value, int dx, int dy, int dz) {
  if (Inputs::cursorX + dx < 0 || Inputs::cursorY + dy < 0 ||
      Inputs::cursorZ + dz < 0 || Inputs::cursorX + dx >= World::MAP_SIZE ||
      Inputs::cursorY + dy >= World::MAP_SIZE ||
      Inputs::cursorZ + dz >= World::MAP_SIZE)
    return;

  const int chunkSize = World::MAP_SIZE / World::CHUNKS;

  int chunkX = (Inputs::cursorX + dx) / chunkSize;
  int chunkZ = (Inputs::cursorZ + dz) / chunkSize;

  if (chunkX < 0 || chunkZ < 0 || chunkX >= 16 || chunkZ >= 16) return;

  World::voxels[Inputs::cursorX + dx][Inputs::cursorY + dy]
                [Inputs::cursorZ + dz] = value;

  Generators::regenerateChunk(chunkX, chunkZ);

  if (Inputs::cursorX + dx == chunkX * chunkSize && chunkX > 0)
    Generators::regenerateChunk(chunkX - 1, chunkZ);
  if (Inputs::cursorX + dx == (chunkX + 1) * chunkSize - 1 &&
      chunkX < World::CHUNKS - 1)
    Generators::regenerateChunk(chunkX + 1, chunkZ);

  if (Inputs::cursorZ + dz == chunkZ * chunkSize && chunkZ > 0)
    Generators::regenerateChunk(chunkX, chunkZ - 1);
  if (Inputs::cursorZ + dz == (chunkZ + 1) * chunkSize - 1 &&
      chunkZ < World::CHUNKS - 1)
    Generators::regenerateChunk(chunkX, chunkZ + 1);
}

void Inputs::processInput(GLFWwindow *window, float deltaTime) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
      Generators::generateMaze();
      Generators::generateVertices();
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
      Generators::generateRandomSpheres();
      Generators::generateVertices();
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
      Generators::generateFlatLand();
      Generators::generateVertices();
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
      World::import();
      Generators::generateVertices();
    }
  }

  Inputs::cameraSpeed = static_cast<float>(10 * deltaTime);

  Inputs::cursorPassThrough = (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    Outputs::cameraPos += Inputs::cameraSpeed * Outputs::cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    Outputs::cameraPos -= Inputs::cameraSpeed * Outputs::cameraFront;

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    Outputs::cameraPos -=
        glm::normalize(glm::cross(Outputs::cameraFront, Outputs::cameraUp)) *
        Inputs::cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    Outputs::cameraPos +=
        glm::normalize(glm::cross(Outputs::cameraFront, Outputs::cameraUp)) *
        Inputs::cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    Outputs::cameraPos += Inputs::cameraSpeed * Outputs::cameraUp;
  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    Outputs::cameraPos -= Inputs::cameraSpeed * Outputs::cameraUp;

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
    // if (lastLeftMouseButton) {
    // if (c) }

    if (!lastLeftMouseButton) {
      if (Inputs::cursorSide != -1 &&
          glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        std::cout << Inputs::cursorSideName[Inputs::cursorSide] << " : "
                  << Inputs::cursorX << ", " << Inputs::cursorY << ", "
                  << Inputs::cursorZ << " (" << Inputs::cursorFracX << ", "
                  << Inputs::cursorFracY << ", " << Inputs::cursorFracZ << ")"
                  << std::endl;

        lastLeftMouseButton = true;

        int dx = 0, dy = 0, dz = 0;
        if (Inputs::cursorSide == 0)
          dx = -1;
        else if (Inputs::cursorSide == 1)
          dx = 1;
        else if (Inputs::cursorSide == 2)
          dy = -1;
        else if (Inputs::cursorSide == 3)
          dy = 1;
        else if (Inputs::cursorSide == 4)
          dz = -1;
        else if (Inputs::cursorSide == 5)
          dz = 1;

        auto current =
            World::voxels[Inputs::cursorX][Inputs::cursorY][Inputs::cursorZ];

        bool fullSide =
            (Inputs::cursorSide == 0 && (current & 0b10011001) == 0b10011001 ||
             Inputs::cursorSide == 1 && (current & 0b01100110) == 0b01100110 ||
             Inputs::cursorSide == 2 && (current & 0b00001111) == 0b00001111 ||
             Inputs::cursorSide == 3 && (current & 0b11110000) == 0b11110000 ||
             Inputs::cursorSide == 4 && (current & 0b00110011) == 0b00110011 ||
             Inputs::cursorSide == 5 && (current & 0b11001100) == 0b11001100);

        std::bitset<8> cBits(current);
        std::cout << "Current: " << (int)current
                  << ", Side: " << Inputs::cursorSide
                  << ", Edge: " << Inputs::cursorEdge
                  << ", Corner: " << Inputs::cursorCorner << " --- " << cBits
                  << " (" << fullSide << ")" << std::endl;

        if (Inputs::cursorCorner == -1 && Inputs::cursorEdge == -1) {
          if (fullSide) {
            processMouseClick(255, dx, dy, dz);
          } else {
            processMouseClick(255, 0, 0, 0);
          }
        } else if (Inputs::cursorEdge != -1 || Inputs::cursorCorner != -1) {
          auto previous = current;
          if (fullSide) {
            current = World::voxels[Inputs::cursorX + dx][Inputs::cursorY + dy]
                                    [Inputs::cursorZ + dz];

            if (Inputs::cursorSide == 0)
              current |= 0b01100110;
            else if (Inputs::cursorSide == 1)
              current |= 0b10011001;
            else if (Inputs::cursorSide == 2)
              current |= 0b11110000;
            else if (Inputs::cursorSide == 3)
              current |= 0b00001111;
            else if (Inputs::cursorSide == 4)
              current |= 0b11001100;
            else if (Inputs::cursorSide == 5)
              current |= 0b00110011;
          }

          if (Inputs::cursorSide == 0 && Inputs::cursorEdge == 0 ||
              Inputs::cursorSide == 5 && Inputs::cursorEdge == 1) {
            current |= ~0b01110111;
          } else if (Inputs::cursorSide == 5 && Inputs::cursorEdge == 2 ||
                     Inputs::cursorSide == 2 && Inputs::cursorEdge == 3) {
            current |= ~0b11110011;
          } else if (Inputs::cursorSide == 5 && Inputs::cursorEdge == 3 ||
                     Inputs::cursorSide == 3 && Inputs::cursorEdge == 3) {
            current |= ~0b00111111;
          } else if (Inputs::cursorSide == 5 && Inputs::cursorEdge == 0 ||
                     Inputs::cursorSide == 1 && Inputs::cursorEdge == 1) {
            current |= ~0b10111011;
          } else if (Inputs::cursorSide == 4 && Inputs::cursorEdge == 3 ||
                     Inputs::cursorSide == 3 && Inputs::cursorEdge == 2) {
            current |= ~0b11001111;
          } else if (Inputs::cursorSide == 4 && Inputs::cursorEdge == 2 ||
                     Inputs::cursorSide == 2 && Inputs::cursorEdge == 2) {
            current |= ~0b11111100;
          } else if (Inputs::cursorSide == 4 && Inputs::cursorEdge == 0 ||
                     Inputs::cursorSide == 0 && Inputs::cursorEdge == 1) {
            current |= ~0b11101110;
          } else if (Inputs::cursorSide == 4 && Inputs::cursorEdge == 1 ||
                     Inputs::cursorSide == 1 && Inputs::cursorEdge == 0) {
            current |= ~0b11011101;
          } else if (Inputs::cursorSide == 1 && Inputs::cursorEdge == 2 ||
                     Inputs::cursorSide == 2 && Inputs::cursorEdge == 0) {
            current |= ~0b11111001;
          } else if (Inputs::cursorSide == 3 && Inputs::cursorEdge == 1 ||
                     Inputs::cursorSide == 1 && Inputs::cursorEdge == 3) {
            current |= ~0b10011111;
          } else if (Inputs::cursorSide == 0 && Inputs::cursorEdge == 2 ||
                     Inputs::cursorSide == 2 && Inputs::cursorEdge == 1) {
            current |= ~0b11110110;
          } else if (Inputs::cursorSide == 0 && Inputs::cursorEdge == 3 ||
                     Inputs::cursorSide == 3 && Inputs::cursorEdge == 0) {
            current |= ~0b01101111;
          } else if (Inputs::cursorSide == 5 && Inputs::cursorCorner == 3 ||
                     Inputs::cursorSide == 0 && Inputs::cursorCorner == 2 ||
                     Inputs::cursorSide == 3 && Inputs::cursorCorner == 2) {
            current |= 0b10000000;
          } else if (Inputs::cursorSide == 3 && Inputs::cursorCorner == 3 ||
                     Inputs::cursorSide == 1 && Inputs::cursorCorner == 3 ||
                     Inputs::cursorSide == 5 && Inputs::cursorCorner == 2) {
            current |= 0b01000000;
          } else if (Inputs::cursorSide == 4 && Inputs::cursorCorner == 3 ||
                     Inputs::cursorSide == 1 && Inputs::cursorCorner == 2 ||
                     Inputs::cursorSide == 3 && Inputs::cursorCorner == 1) {
            current |= 0b00100000;
          } else if (Inputs::cursorSide == 4 && Inputs::cursorCorner == 2 ||
                     Inputs::cursorSide == 3 && Inputs::cursorCorner == 0 ||
                     Inputs::cursorSide == 0 && Inputs::cursorCorner == 3) {
            current |= 0b00010000;
          } else if (Inputs::cursorSide == 5 && Inputs::cursorCorner == 1 ||
                     Inputs::cursorSide == 2 && Inputs::cursorCorner == 3 ||
                     Inputs::cursorSide == 0 && Inputs::cursorCorner == 0) {
            current |= 0b00001000;
          } else if (Inputs::cursorSide == 1 && Inputs::cursorCorner == 1 ||
                     Inputs::cursorSide == 2 && Inputs::cursorCorner == 2 ||
                     Inputs::cursorSide == 5 && Inputs::cursorCorner == 0) {
            current |= 0b00000100;
          } else if (Inputs::cursorSide == 4 && Inputs::cursorCorner == 1 ||
                     Inputs::cursorSide == 1 && Inputs::cursorCorner == 0 ||
                     Inputs::cursorSide == 2 && Inputs::cursorCorner == 0) {
            current |= 0b00000010;
          } else if (Inputs::cursorSide == 4 && Inputs::cursorCorner == 0 ||
                     Inputs::cursorSide == 2 && Inputs::cursorCorner == 1 ||
                     Inputs::cursorSide == 0 && Inputs::cursorCorner == 1) {
            current |= 0b00000001;
          }

          if (Tris::shapes[current].size() == 0) current = 0;

          if (fullSide) {
            processMouseClick(current, dx, dy, dz);
          } else {
            processMouseClick(current, 0, 0, 0);
          }
        }
      }
    }
  } else {
    if (lastLeftMouseButton) lastLeftMouseButton = false;
  }

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
    if (!lastRightMouseButton) {
      lastRightMouseButton = true;
      if (Inputs::cursorSide != -1 &&
          glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (Inputs::cursorCorner == -1 && Inputs::cursorEdge == -1) {
          processMouseClick(0, 0, 0, 0);
        } else {
          uint8_t current =
              World::voxels[Inputs::cursorX][Inputs::cursorY][Inputs::cursorZ];
          std::bitset<8> cBits(current);

          std::cout << "Current: " << (int)current
                    << ", Side: " << Inputs::cursorSide
                    << ", Edge: " << Inputs::cursorEdge
                    << ", Corner: " << Inputs::cursorCorner << " --- " << cBits
                    << std::endl;

          if (Inputs::cursorSide == 0 && Inputs::cursorEdge == 0 ||
              Inputs::cursorSide == 5 && Inputs::cursorEdge == 1) {
            current &= 0b01110111;
          } else if (Inputs::cursorSide == 5 && Inputs::cursorEdge == 2 ||
                     Inputs::cursorSide == 2 && Inputs::cursorEdge == 3) {
            current &= 0b11110011;
          } else if (Inputs::cursorSide == 5 && Inputs::cursorEdge == 3 ||
                     Inputs::cursorSide == 3 && Inputs::cursorEdge == 3) {
            current &= 0b00111111;
          } else if (Inputs::cursorSide == 5 && Inputs::cursorEdge == 0 ||
                     Inputs::cursorSide == 1 && Inputs::cursorEdge == 1) {
            current &= 0b10111011;
          } else if (Inputs::cursorSide == 4 && Inputs::cursorEdge == 3 ||
                     Inputs::cursorSide == 3 && Inputs::cursorEdge == 2) {
            current &= 0b11001111;
          } else if (Inputs::cursorSide == 4 && Inputs::cursorEdge == 2 ||
                     Inputs::cursorSide == 2 && Inputs::cursorEdge == 2) {
            current &= 0b11111100;
          } else if (Inputs::cursorSide == 4 && Inputs::cursorEdge == 0 ||
                     Inputs::cursorSide == 0 && Inputs::cursorEdge == 1) {
            current &= 0b11101110;
          } else if (Inputs::cursorSide == 4 && Inputs::cursorEdge == 1 ||
                     Inputs::cursorSide == 1 && Inputs::cursorEdge == 0) {
            current &= 0b11011101;
          } else if (Inputs::cursorSide == 1 && Inputs::cursorEdge == 2 ||
                     Inputs::cursorSide == 2 && Inputs::cursorEdge == 0) {
            current &= 0b11111001;
          } else if (Inputs::cursorSide == 3 && Inputs::cursorEdge == 1 ||
                     Inputs::cursorSide == 1 && Inputs::cursorEdge == 3) {
            current &= 0b10011111;
          } else if (Inputs::cursorSide == 0 && Inputs::cursorEdge == 2 ||
                     Inputs::cursorSide == 2 && Inputs::cursorEdge == 1) {
            current &= 0b11110110;
          } else if (Inputs::cursorSide == 0 && Inputs::cursorEdge == 3 ||
                     Inputs::cursorSide == 3 && Inputs::cursorEdge == 0) {
            current &= 0b01101111;
          } else if (Inputs::cursorSide == 5 && Inputs::cursorCorner == 3 ||
                     Inputs::cursorSide == 0 && Inputs::cursorCorner == 2 ||
                     Inputs::cursorSide == 3 && Inputs::cursorCorner == 2) {
            if (current & 0b10000000) {
              current &= ~0b10000000;
            } else {
              current &= ~0b11011000;
            }
          } else if (Inputs::cursorSide == 3 && Inputs::cursorCorner == 3 ||
                     Inputs::cursorSide == 1 && Inputs::cursorCorner == 3 ||
                     Inputs::cursorSide == 5 && Inputs::cursorCorner == 2) {
            if (current & 0b01000000) {
              current &= ~0b01000000;
            } else {
              current &= ~0b11100100;
            }
          } else if (Inputs::cursorSide == 4 && Inputs::cursorCorner == 3 ||
                     Inputs::cursorSide == 1 && Inputs::cursorCorner == 2 ||
                     Inputs::cursorSide == 3 && Inputs::cursorCorner == 1) {
            if (current & 0b00100000) {
              current &= ~0b00100000;
            } else {
              current &= ~0b01110010;
            }
          } else if (Inputs::cursorSide == 4 && Inputs::cursorCorner == 2 ||
                     Inputs::cursorSide == 3 && Inputs::cursorCorner == 0 ||
                     Inputs::cursorSide == 0 && Inputs::cursorCorner == 3) {
            if (current & 0b00010000) {
              current &= ~0b00010000;
            } else {
              current &= ~0b10110001;
            }
          } else if (Inputs::cursorSide == 5 && Inputs::cursorCorner == 1 ||
                     Inputs::cursorSide == 2 && Inputs::cursorCorner == 3 ||
                     Inputs::cursorSide == 0 && Inputs::cursorCorner == 0) {
            if (current & 0b00001000) {
              current &= ~0b00001000;
            } else {
              current &= ~0b10001101;
            }
          } else if (Inputs::cursorSide == 1 && Inputs::cursorCorner == 1 ||
                     Inputs::cursorSide == 2 && Inputs::cursorCorner == 2 ||
                     Inputs::cursorSide == 5 && Inputs::cursorCorner == 0) {
            if (current & 0b00000100) {
              current &= ~0b00000100;
            } else {
              current &= ~0b01001110;
            }
          } else if (Inputs::cursorSide == 4 && Inputs::cursorCorner == 1 ||
                     Inputs::cursorSide == 1 && Inputs::cursorCorner == 0 ||
                     Inputs::cursorSide == 2 && Inputs::cursorCorner == 0) {
            if (current & 0b00000010) {
              current &= ~0b00000010;
            } else {
              current &= ~0b00100111;
            }
          } else if (Inputs::cursorSide == 4 && Inputs::cursorCorner == 0 ||
                     Inputs::cursorSide == 2 && Inputs::cursorCorner == 1 ||
                     Inputs::cursorSide == 0 && Inputs::cursorCorner == 1) {
            if (current & 0b00000001) {
              current &= ~0b00000001;
            } else {
              current &= ~0b00011011;
            }
          }

          if (Tris::shapes[current].size() == 0) current = 0;

          processMouseClick(current, 0, 0, 0);
        }
      }
    }
  } else {
    if (lastRightMouseButton) lastRightMouseButton = false;
  }
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (Inputs::firstMouse) {
    Inputs::lastX = xpos;
    Inputs::lastY = ypos;
    Inputs::firstMouse = false;
  }

  float xoffset = xpos - Inputs::lastX;
  float yoffset = Inputs::lastY - ypos;
  Inputs::lastX = xpos;
  Inputs::lastY = ypos;

  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  Inputs::yaw += xoffset * Inputs::fov / 45;
  Inputs::pitch += yoffset * Inputs::fov / 45;

  if (Inputs::pitch > 89.0f) Inputs::pitch = 89.0f;
  if (Inputs::pitch < -89.0f) Inputs::pitch = -89.0f;

  glm::vec3 front;
  front.x = cos(glm::radians(Inputs::yaw)) * cos(glm::radians(Inputs::pitch));
  front.y = sin(glm::radians(Inputs::pitch));
  front.z = sin(glm::radians(Inputs::yaw)) * cos(glm::radians(Inputs::pitch));
  Outputs::cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
    Inputs::fov -= (float)yoffset;
    if (Inputs::fov < 1.0f) Inputs::fov = 1.0f;
    if (Inputs::fov > 45.0f) Inputs::fov = 45.0f;
  }
}
