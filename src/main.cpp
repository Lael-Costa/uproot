#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>

int main(int argc, char** argv)
{
  if (!glfwInit())
  {
    std::cerr << "GLFW initialization failed. Uh-oh!" << std::endl;
    exit(EXIT_FAILURE);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  GLFWwindow* window = glfwCreateWindow(640, 480, "Uproot", NULL, NULL);
  if (!window)
  {
    std::cerr << "Window creation failed. Uh-oh!" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  while (!glfwWindowShouldClose(window))
  {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
