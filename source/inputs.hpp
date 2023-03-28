#ifndef _INPUTS_HPP
#define _INPUTS_HPP

#define GLFW_INCLUDE_NONE
#include "../libraries/glfw/glfw3.h"

#include <string>

class Inputs {
 public:
  static const int FIRST_TEXTURE = 4;

  static int cursorX;
  static int cursorY;
  static int cursorZ;
  static int cursorSide;
  static int cursorEdge;
  static int cursorCorner;
  static bool cursorPassThrough;

  static float cameraSpeed;
  
  static int lastCursorX;
  static int lastCursorY;
  static int lastCursorZ;
  static int lastCursorSide;

  static float cursorFracX;
  static float cursorFracY;
  static float cursorFracZ;
  static float cursorFracU;
  static float cursorFracV;

  static float yaw;
  static float pitch;
  static float lastX;
  static float lastY;
  static float fov;

  static bool firstMouse;
  static bool lastLeftMouseButton;
  static bool lastRightMouseButton;

  static std::string cursorSideName[6];

  static void evaluateCursor();
  static void processInput(GLFWwindow *window, float deltaTime);
};

#endif
