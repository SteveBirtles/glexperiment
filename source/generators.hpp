#ifndef _GENERATORS_HPP
#define _GENERATORS_HPP

#include "world.hpp"

class Generators {
  struct kruskalCell {
    int set;
    bool right;
    bool down;
  };
  
  static bool map[World::MAP_SIZE + 2][World::MAP_SIZE + 2][World::MAP_SIZE + 2];  

  static const int MAZE_SIZE = World::MAP_SIZE / 2 - 2;
  static kruskalCell kruskalMaze[MAZE_SIZE][MAZE_SIZE];
  static void processMap();

 public:

  static bool kruskalStep();
  static void generateMaze();
  static void generateFlatLand();
  static void generateRandomSpheres();
  static void generateVertices();
  static void regenerateChunk(int chunkX, int chunkZ);
};

#endif
