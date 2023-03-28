#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "world.hpp"
#include "tris.hpp"

std::vector<float> World::vertices[CHUNKS][CHUNKS][12];

std::string World::VOX_FILE = "assets/monu1.vox";

uint8_t World::voxels[MAP_SIZE][MAP_SIZE][MAP_SIZE];
uint8_t World::voxelColours[MAP_SIZE][MAP_SIZE][MAP_SIZE][3];
uint8_t World::voxelTextures[MAP_SIZE][MAP_SIZE][MAP_SIZE][8];

void World::saveMap() {
  auto mapFile = std::ofstream("saves/map.csv");
  if (!mapFile.is_open()) return;
  for (auto z = 0; z < MAP_SIZE; z++) {
    for (auto y = 0; y < MAP_SIZE; y++) {
      for (auto x = 0; x < MAP_SIZE; x++) {
        mapFile << std::to_string(voxels[x][y][z]) << ",";
        
        unsigned long long int tValue = 0;
        for (auto t = 0; t < 8; t++) {
          unsigned long long int tBit = voxelTextures[x][y][z][t];
          tValue += tBit << (t * 8);
        }
        mapFile << std::to_string(tValue) << ",";

        unsigned long long int cValue = 0;
        for (auto c = 0; c < 3; c++) {
          unsigned long long int cBit = voxelColours[x][y][z][c];
          cValue += cBit << (c * 8);
        }
        mapFile << std::to_string(cValue);        

        if (x != MAP_SIZE - 1) {
          mapFile << ",";
        } else {
          mapFile << std::endl;
        }
      }
    }
  }
  mapFile.close();
}

void World::loadMap() {
  auto mapFile = std::ifstream("saves/map.csv");
  if (!mapFile.is_open()) return;
  std::string line;
  std::string token;
  for (auto z = 0; z < MAP_SIZE; z++) {
    for (auto y = 0; y < MAP_SIZE; y++) {
      std::getline(mapFile, line, '\n');
      auto stream = std::istringstream(line);
      for (auto x = 0; x < MAP_SIZE; x++) {
        std::getline(stream, token, ',');
        if (token == "") continue;
        voxels[x][y][z] = std::stoi(token);

        unsigned long long int base = 0b11111111;
        
        std::getline(stream, token, ',');
        if (token == "") continue;
        unsigned long long int tValue = std::stoull(token);      
        for (auto t = 0; t < 8; t++) {
          voxelTextures[x][y][z][t] = (tValue & (base << (t * 8))) >> (t * 8);
        }

        std::getline(stream, token, ',');
        if (token == "") continue;
        unsigned long long int cValue = std::stoull(token);        
        for (auto c = 0; c < 3; c++) {
          voxelColours[x][y][z][c] = (cValue & (base << (c * 8))) >> (c * 8);
        }

      }
    }
  }
  mapFile.close();
}

void World::import() {
  std::vector<Voxel> fileVoxels;
  std::vector<Colour> pal;
  int modelCount{0};
  int currentModel{1};

  int maxX{0};
  int maxY{0};
  int maxZ{0};

  std::cout << "Opening " + VOX_FILE << " ..." << std::endl;

  auto voxFile =
      std::ifstream(VOX_FILE, std::ios::in | std::ios::binary);
  if (!voxFile.is_open()) {
    std::cout << "Failed to open input file!" << std::endl;
    return;
  }

  auto getByte = [&voxFile]() mutable {
    char byte;
    voxFile.read((char *)&byte, sizeof(byte));
    return byte;
  };

  auto getInt = [&getByte]() {
    uint64_t number = 0;
    for (auto i{0}; i < 4; i++) {
      uint8_t x = getByte();
      number += x << (i * 8);
    }
    return number;
  };

  auto getString = [&getByte](int n) {
    std::stringstream s;
    for (auto i{0}; i < 4; i++) {
      s << getByte();
    }
    return s.str();
  };

  if (getString(4) == "VOX ") {
    auto version = getInt();
    std::cout << "Voxel file, version " << version << "." << std::endl;

    if (getString(4) == "MAIN") {
      getInt();
      getInt();
      std::cout << "Voxel file main section..." << std::endl;

      int packCount = 1;
      auto block = getString(4);
      if (block == "PACK") {
        getInt();
        getInt();
        packCount = getInt();
        block = getString(4);
        std::cout << packCount << " packs..." << std::endl;
      }

      while (block == "SIZE") {
        modelCount++;
        getInt();
        getInt();
        int xSize = getInt();
        int ySize = getInt();
        int zSize = getInt();
        if (xSize - 1 > maxX) maxX = xSize - 1;
        if (ySize - 1 > maxY) maxY = ySize - 1;
        if (zSize - 1 > maxZ) maxZ = zSize - 1;
        std::cout << "SIZE section. Model size: " << xSize << ", " << ySize
                  << ", " << zSize << "." << std::endl;
        block = getString(4);

        if (block == "XYZI") {
          getInt();
          getInt();
          auto voxelCount = getInt();
          std::cout << "XYZI section. Voxel Count: " << voxelCount << "."
                    << std::endl;
          for (int i = 0; i < voxelCount; i++) {
            int x = (uint8_t)getByte();
            int y = (uint8_t)getByte();
            int z = (uint8_t)getByte();
            int c = (uint8_t)getByte();
            fileVoxels.emplace_back(modelCount - 1, x, y, z, c);
          }
        } else {
          std::cout << "XYZI section missing!" << std::endl;
          return;
        }

        block = getString(4);
      }

      while (!voxFile.eof()) {
        auto n = getInt();
        auto m = getInt();
        if (m > 0) {
          std::cout << "Non-zero children found for " << block << "."
                    << std::endl;
        }
        if (block == "RGBA") {
          std::cout << "Palette found!" << std::endl;
          pal.emplace_back(0, 0, 0, 0);
          for (int i = 0; i < n / 4; i++) {
            int r = (uint8_t)getByte();
            int g = (uint8_t)getByte();
            int b = (uint8_t)getByte();
            int a = (uint8_t)getByte();
            pal.emplace_back(r, g, b, a);
            std::cout << r << ',' << g << ',' << b << std::endl;
          }
        } else {
          if (block == "nTRN") {
            std::cout << "nTRN found!" << std::endl;
          }

          for (int i = 0; i < n + m; i++) {
            getByte();
          }
        }

        block = getString(4);
      }
    }
  }

  if (pal.size() == 0) {
    std::cout << "Using default palette..." << std::endl;
    for (int p = 0; p < 256; p++) {
      pal.emplace_back(default_palette[p] & 0x000000FF,
                       (default_palette[p] & 0x0000FF00) >> 8,
                       (default_palette[p] & 0x00FF0000) >> 16,
                       (default_palette[p] & 0xFF000000) >> 24);
    }
  }

  for (int u = 0; u < MAP_SIZE; u++) {
    for (int v = 0; v < MAP_SIZE; v++) {
      for (int w = 0; w < MAP_SIZE; w++) {
        voxels[u][v][w] = 0;
        voxelColours[u][v][w][0] = 255;
        voxelColours[u][v][w][1] = 255;
        voxelColours[u][v][w][2] = 255;
        voxelTextures[u][v][w][0] = 10;
        voxelTextures[u][v][w][1] = 10;
        voxelTextures[u][v][w][2] = 10;
        voxelTextures[u][v][w][3] = 10;
        voxelTextures[u][v][w][4] = 10;
        voxelTextures[u][v][w][5] = 10;
        voxelTextures[u][v][w][6] = 10;
        voxelTextures[u][v][w][7] = 10;
      }
    }
  }

  for (auto fv : fileVoxels) {
    if (fv.x >= 0 && fv.x < MAP_SIZE - 1 && fv.y >= 0 &&
        fv.y < MAP_SIZE - 1 && fv.z >= 0 &&
        fv.z < MAP_SIZE - 1) {
      int i = fv.y;
      int j = fv.z;
      int k = fv.x;
      voxels[i][j][k] = 0xFF;
      voxelColours[i][j][k][0] = pal[fv.colour].r;
      voxelColours[i][j][k][1] = pal[fv.colour].g;
      voxelColours[i][j][k][2] = pal[fv.colour].b;
    }
  }
}

unsigned int World::default_palette[256] = {
    0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff,
    0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff,
    0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff, 0xff6699ff, 0xff3399ff,
    0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff,
    0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff,
    0xff0033ff, 0xffff00ff, 0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff,
    0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc,
    0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
    0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc,
    0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc,
    0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc, 0xff6633cc, 0xff3333cc,
    0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc,
    0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99,
    0xff00ff99, 0xffffcc99, 0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99,
    0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999,
    0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
    0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399,
    0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099,
    0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66, 0xff66ff66, 0xff33ff66,
    0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66,
    0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966,
    0xff009966, 0xffff6666, 0xffcc6666, 0xff996666, 0xff666666, 0xff336666,
    0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366,
    0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
    0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33,
    0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33,
    0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933, 0xff669933, 0xff339933,
    0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633,
    0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333,
    0xff003333, 0xffff0033, 0xffcc0033, 0xff990033, 0xff660033, 0xff330033,
    0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00,
    0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
    0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900,
    0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600,
    0xff006600, 0xffff3300, 0xffcc3300, 0xff993300, 0xff663300, 0xff333300,
    0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000,
    0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077,
    0xff000055, 0xff000044, 0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00,
    0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400,
    0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
    0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000,
    0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777,
    0xff555555, 0xff444444, 0xff222222, 0xff111111};