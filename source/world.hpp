#ifndef _VOXELS_HPP
#define _VOXELS_HPP

#include <string>
#include <vector>

class World {
 public:
  static const int MAP_SIZE = 128;
  static const int CHUNKS = 16;

  static std::vector<float> vertices[CHUNKS][CHUNKS][12];

  static std::string VOX_FILE;

  static uint8_t voxels[MAP_SIZE][MAP_SIZE][MAP_SIZE];
  static uint8_t voxelColours[MAP_SIZE][MAP_SIZE][MAP_SIZE][3];
  static uint8_t voxelTextures[MAP_SIZE][MAP_SIZE][MAP_SIZE][8];

  static void saveMap();
  static void loadMap();
  static void import();
  static void process();

  struct Voxel {
    int modelId;
    int x;
    int y;
    int z;
    int colour;
    Voxel(int _id, int _x, int _y, int _z, int _c) {
      modelId = _id;
      x = _x;
      y = _y;
      z = _z;
      colour = _c;
    }
  };

  struct Colour {
    int r;
    int g;
    int b;
    int a;
    Colour(int _r, int _g, int _b, int _a) {
      r = _r;
      g = _g;
      b = _b;
      a = _a;
    }
  };

 private:
  static unsigned int default_palette[256];
};

#endif