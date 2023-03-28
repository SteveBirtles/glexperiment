#ifndef _TRIS_HPP
#define _TRIS_HPP

#include <string>
#include <vector>

#include "../libraries/glm/glm.hpp"

class Tris {
 public:
  static std::string prototypes[88];
  static glm::ivec4 adjacents[24];
  static int textures[88];
  static std::vector<uint8_t> shapes[256];

 private:
  glm::vec3 shapeVertices[8]{
      {0, 0, 0}, {1, 0, 0}, {1, 0, 1}, {0, 0, 1},
      {0, 1, 0}, {1, 1, 0}, {1, 1, 1}, {0, 1, 1},
  };

  glm::vec2 textureCoords[12][3]{

      {{0, 0}, {1, 0}, {1, 1}},     {{1, 0}, {1, 1}, {0, 1}},
      {{1, 1}, {0, 1}, {0, 0}},     {{0, 1}, {0, 0}, {1, 0}},

      {{0, 0}, {1, 1}, {1, 0}},     {{1, 0}, {0, 1}, {1, 1}},
      {{1, 1}, {0, 0}, {0, 1}},     {{0, 1}, {1, 0}, {0, 0}},

      {{0, 0}, {1, 0}, {0.5, 1}},   {{0, 0}, {0.5, 1}, {1, 0}},
      {{1, 0}, {0.5, 1}, {1.5, 1}}, {{1, 0}, {1.5, 1}, {0.5, 1}},

  };

 public:
  glm::vec3 xyz[3];
  glm::vec2 uv[3];
  glm::vec3 centre;
  glm::vec3 normal;
  int texture;
  Tris(std::string prototype, int textureNo) {
    int firstIndex = prototype[0] - '0';
    int secondIndex = prototype[1] - '0';
    int thirdIndex = prototype[2] - '0';
    xyz[0] = shapeVertices[firstIndex];
    xyz[1] = shapeVertices[secondIndex];
    xyz[2] = shapeVertices[thirdIndex];
    int textureCoordIndex = prototype[3] - 'a';
    uv[0] = textureCoords[textureCoordIndex][0];
    uv[1] = textureCoords[textureCoordIndex][1];
    uv[2] = textureCoords[textureCoordIndex][2];
    texture = textureNo;
    centre = (xyz[0] + xyz[1] + xyz[2]) / 3.0f;
    normal = glm::normalize(glm::cross(xyz[1] - xyz[0], xyz[2] - xyz[0]));
  }
};

#endif