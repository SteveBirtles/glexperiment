#include <iostream>
#include <vector>

#include "generators.hpp"
#include "outputs.hpp"
#include "tris.hpp"
#include "world.hpp"

Generators::kruskalCell Generators::kruskalMaze[MAZE_SIZE][MAZE_SIZE];

bool Generators::map[World::MAP_SIZE + 2][World::MAP_SIZE + 2]
                    [World::MAP_SIZE + 2];

void Generators::processMap() {
  for (int u = 0; u < World::MAP_SIZE; u++) {
    for (int v = 0; v < World::MAP_SIZE; v++) {
      for (int w = 0; w < World::MAP_SIZE; w++) {
        World::voxels[u][v][w] = 0;
        World::voxelColours[u][v][w][0] = 255;
        World::voxelColours[u][v][w][1] = 255;
        World::voxelColours[u][v][w][2] = 255;
        World::voxelTextures[u][v][w][0] = 7;
        World::voxelTextures[u][v][w][1] = 8;
        World::voxelTextures[u][v][w][2] = 8;
        World::voxelTextures[u][v][w][3] = 8;
        World::voxelTextures[u][v][w][4] = 8;
        World::voxelTextures[u][v][w][5] = 7;
        World::voxelTextures[u][v][w][6] = 6;
        World::voxelTextures[u][v][w][7] = 9;

        int du, dv, dw, pattern = 0;
        for (int c = 0; c < 8; c++) {
          switch (c) {
            case 0:
              du = 1;
              dv = 1;
              dw = 1;
              break;
            case 1:
              du = 2;
              dv = 1;
              dw = 1;
              break;
            case 2:
              du = 2;
              dv = 1;
              dw = 2;
              break;
            case 3:
              du = 1;
              dv = 1;
              dw = 2;
              break;
            case 4:
              du = 1;
              dv = 2;
              dw = 1;
              break;
            case 5:
              du = 2;
              dv = 2;
              dw = 1;
              break;
            case 6:
              du = 2;
              dv = 2;
              dw = 2;
              break;
            case 7:
              du = 1;
              dv = 2;
              dw = 2;
              break;
          }
          if (map[u + du][v + dv][w + dw]) {
            pattern = pattern | (1 << c);
          }
        }

        if (Tris::shapes[pattern].size() > 0) {
          World::voxels[u][v][w] = pattern;
        }
      }
    }
  }
}

bool Generators::kruskalStep() {
  bool oneSet = true;
  for (int i = 0; i < MAZE_SIZE; i++) {
    for (int j = 0; j < MAZE_SIZE; j++) {
      if (kruskalMaze[i][j].set != kruskalMaze[0][0].set) oneSet = false;
    }
  }

  if (oneSet) return true;

  int x = 0;
  int y = 0;
  int a = 0;
  int b = 0;
  int horizontal = 0;
  int vertical = 0;

  while (true) {
    x = rand() % MAZE_SIZE;
    y = rand() % MAZE_SIZE;

    if (rand() % 2 == 0) {
      horizontal = 1;
      vertical = 0;
    } else {
      horizontal = 0;
      vertical = 1;
    }

    if (horizontal > 0 && (kruskalMaze[x][y].right || x == MAZE_SIZE - 1))
      continue;

    if (vertical > 0 && (kruskalMaze[x][y].down || y == MAZE_SIZE - 1))
      continue;

    a = kruskalMaze[x][y].set;
    b = kruskalMaze[x + horizontal][y + vertical].set;

    if (a == b) continue;

    if (vertical > 0)
      kruskalMaze[x][y].down = true;
    else
      kruskalMaze[x][y].right = true;

    for (int i = 0; i < MAZE_SIZE; i++) {
      for (int j = 0; j < MAZE_SIZE; j++) {
        if (kruskalMaze[i][j].set == b) kruskalMaze[i][j].set = a;
      }
    }

    return false;
  }
}

void Generators::generateMaze() {
  for (int u = 0; u < World::MAP_SIZE; u++) {
    for (int v = 0; v < World::MAP_SIZE; v++) {
      for (int w = 0; w < World::MAP_SIZE; w++) {
        World::voxels[u][v][w] = 0;
        World::voxelColours[u][v][w][0] = 255;
        World::voxelColours[u][v][w][1] = 255;
        World::voxelColours[u][v][w][2] = 255;
        World::voxelTextures[u][v][w][0] = 7;
        World::voxelTextures[u][v][w][1] = 8;
        World::voxelTextures[u][v][w][2] = 8;
        World::voxelTextures[u][v][w][3] = 8;
        World::voxelTextures[u][v][w][4] = 8;
        World::voxelTextures[u][v][w][5] = 7;
        World::voxelTextures[u][v][w][6] = 6;
        World::voxelTextures[u][v][w][7] = 9;
      }
    }
  }

  int n = 0;
  for (int i = 0; i < MAZE_SIZE; i++) {
    for (int j = 0; j < MAZE_SIZE; j++) {
      n++;
      kruskalMaze[i][j].set = n;
      kruskalMaze[i][j].right = false;
      kruskalMaze[i][j].down = false;
    }
  }

  bool mazeDone = false;
  while (!mazeDone) {
    mazeDone = kruskalStep();
  }

  for (int i = -MAZE_SIZE; i <= MAZE_SIZE; i++) {
    for (int j = -MAZE_SIZE; j <= MAZE_SIZE; j++) {
      World::voxels[i + MAZE_SIZE][0][j + MAZE_SIZE] = 255;
      if ((i + MAZE_SIZE) % 2 == 0 || (j + MAZE_SIZE) % 2 == 0) {
        for (int k = 1; k <= 3; k++) {
          World::voxels[i + MAZE_SIZE][k][j + MAZE_SIZE] = 255;
        }
      }
    }
  }

  for (int i = 0; i < MAZE_SIZE; i++) {
    for (int j = 0; j < MAZE_SIZE; j++) {
      if (kruskalMaze[i][j].right) {
        for (int k = 1; k <= 3; k++) {
          World::voxels[i * 2 + 2][k][j * 2 + 1] = 0;
        }
      }
      if (kruskalMaze[i][j].down) {
        for (int k = 1; k <= 3; k++) {
          World::voxels[i * 2 + 1][k][j * 2 + 2] = 0;
        }
      }
    }
  }

  std::cout << "Maze generated." << std::endl;
}

void Generators::generateFlatLand() {
  for (int u = 0; u < World::MAP_SIZE + 2; u++) {
    for (int w = 0; w < World::MAP_SIZE + 2; w++) {
      int height = 10;  // + 3 * noise2D(((float) u) / World::MAP_SIZE,
                        // ((float) w) / World::MAP_SIZE);
      for (int v = 0; v < World::MAP_SIZE + 2; v++) {
        Generators::map[u][v][w] = v < height;
      }
    }
  }

  std::cout << "Flat land generated." << std::endl;

  Generators::processMap();
}

void Generators::generateRandomSpheres() {
  const int coreCount = 50;

  std::vector<glm::ivec4> cores;
  for (int c = 0; c < coreCount; c++) {
    cores.emplace_back((rand() % (World::MAP_SIZE - 20)) + 10,
                       (rand() % (World::MAP_SIZE - 20)) + 10,
                       (rand() % (World::MAP_SIZE - 20)) + 10,
                       (rand() % 10) + 5);
  }

  std::cout << "Generating spheres..." << std::endl;

  auto sDistSquared = [](int u, int v, int w, int i, int j, int k) {
    return (u - i) * (u - i) + (v - j) * (v - j) + (w - k) * (w - k);
  };

  for (int u = 0; u < World::MAP_SIZE + 2; u++) {
    std::cout << (u + 1) << "/" << World::MAP_SIZE << std::endl;
    for (int v = 0; v < World::MAP_SIZE + 2; v++) {
      for (int w = 0; w < World::MAP_SIZE + 2; w++) {
        Generators::map[u][v][w] = false;
        for (const auto &core : cores) {
          if (sDistSquared(u + 0.5, v + 0.5, w + 0.5, core.x, core.y, core.z) <=
              core.w * core.w) {
            Generators::map[u][v][w] = true;
            break;
          }
        }
      }
    }
  }

  std::cout << "Random spheres generated." << std::endl;

  Generators::processMap();
}

void Generators::generateVertices() {
  std::cout << "Processing Vertices..." << std::endl;

  for (int chunkX = 0; chunkX < World::CHUNKS; chunkX++) {
    for (int chunkZ = 0; chunkZ < World::CHUNKS; chunkZ++) {
      Generators::regenerateChunk(chunkX, chunkZ);
    }
  }
}

void Generators::regenerateChunk(int chunkX, int chunkZ) {
  for (int i = 0; i < 12; i++) {
    World::vertices[chunkX][chunkZ][i].clear();
  }

  const int chunkSize = World::MAP_SIZE / World::CHUNKS;
  for (auto i = chunkX * chunkSize; i < (chunkX + 1) * chunkSize; i++) {
    if (i >= World::MAP_SIZE) break;
    for (auto k = chunkZ * chunkSize; k < (chunkZ + 1) * chunkSize; k++) {
      if (k >= World::MAP_SIZE) break;
      for (auto j = 0; j < World::MAP_SIZE; j++) {
        auto s = World::voxels[i][j][k];
        if (s == 0) continue;

        auto r = (float) (World::voxelColours[i][j][k][0]) / 255.0f;
        auto g = (float) (World::voxelColours[i][j][k][1]) / 255.0f;
        auto b = (float) (World::voxelColours[i][j][k][2]) / 255.0f;

        for (auto t = 0; t < Tris::shapes[s].size(); t++) {
          auto skip = false;

          if (Tris::shapes[s][t] < 24) {
            auto adjacentTriVector = Tris::adjacents[Tris::shapes[s][t]];
            auto dx = -adjacentTriVector.x;
            auto dy = -adjacentTriVector.y;
            auto dz = -adjacentTriVector.z;
            auto adjacentTri = adjacentTriVector.w;

            if (i + dx >= 0 && i + dx < World::MAP_SIZE && j + dy >= 0 &&
                j + dy < World::MAP_SIZE && k + dz >= 0 &&
                k + dz < World::MAP_SIZE) {
              auto adjacentShape = World::voxels[i + dx][j + dy][k + dz];
              for (auto t2 = 0; t2 < Tris::shapes[adjacentShape].size(); t2++) {
                if (Tris::shapes[adjacentShape][t2] == adjacentTri) {
                  skip = true;
                  break;
                }
              }
            }
          }

          if (!skip) {
            auto triId = Tris::shapes[s][t];
            auto prototype = Tris::prototypes[triId];
            auto tri = Tris(prototype, Tris::textures[triId]);

            int tex = World::voxelTextures[i][j][k][Tris::textures[triId]];

            for (auto v = 0; v < 3; v++) {
              World::vertices[chunkX][chunkZ][tex].push_back(tri.xyz[v].x + i);
              World::vertices[chunkX][chunkZ][tex].push_back(tri.xyz[v].y + j);
              World::vertices[chunkX][chunkZ][tex].push_back(tri.xyz[v].z + k);
              World::vertices[chunkX][chunkZ][tex].push_back(tri.uv[v].x);
              World::vertices[chunkX][chunkZ][tex].push_back(tri.uv[v].y);
              World::vertices[chunkX][chunkZ][tex].push_back(tri.normal.x);
              World::vertices[chunkX][chunkZ][tex].push_back(tri.normal.y);
              World::vertices[chunkX][chunkZ][tex].push_back(tri.normal.z);
              World::vertices[chunkX][chunkZ][tex].push_back(tri.centre.x + i);
              World::vertices[chunkX][chunkZ][tex].push_back(tri.centre.y + j);
              World::vertices[chunkX][chunkZ][tex].push_back(tri.centre.z + k);
              World::vertices[chunkX][chunkZ][tex].push_back(r);
              World::vertices[chunkX][chunkZ][tex].push_back(g);
              World::vertices[chunkX][chunkZ][tex].push_back(b);
            }
          }
        }
      }
    }
  }

  Outputs::bindVertexBuffer(chunkX, chunkZ);
}