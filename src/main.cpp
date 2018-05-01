#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <complex>

#include <iostream>
#include <memory>
#include <string>

#include "polynomial.h"
#include "shader.h"

static void onKeyPress(GLFWwindow *window,
  int key,
  int scancode,
  int action,
  int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void printGLVersion()
{
  GLint maj = 0, min = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &maj);
  glGetIntegerv(GL_MINOR_VERSION, &min);
  std::cout << "OpenGL Version: " << maj << "." << min << std::endl;
}


int main(int argc, char** argv)
{
  if (!glfwInit())
  {
    std::cerr << "GLFW initialization failed!" << std::endl;
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(640, 480, "Uproot", NULL, NULL);
  if (!window)
  {
    std::cerr << "Window creation failed!" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, onKeyPress);

  glfwMakeContextCurrent(window);

  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    std::cerr << "Awww..." << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  printGLVersion();

  std::string vertexShaderSource;
  std::string fragmentShaderSource;

  // std::shared_ptr<Shader> shader = std::make_shared<Shader>(
  //   "/Users/laelcosta/Dropbox/Spring2017/Math 154/uproot/res/shader.vs",
  //   "/Users/laelcosta/Dropbox/Spring2017/Math 154/uproot/res/shader.fs");
  // shader->bind();
  glfwSwapInterval(1);

  while (!glfwWindowShouldClose(window))
  {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);



    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);

    // polynomial testing, integers
    //polynomial<std::complex<double>> p1({1,0,0,0,1});
    //polynomial<std::complex<double>> p2({1, {-0.62349,-0.781831}});

    //polynomial<std::complex<double>> p3({4, 3, 2, 1, 0.5,}); 
    //std::cout << p1; 
    //std::cout << p2; 
    
    //std::cout << p1 / p2;
    //std::cout << p1 % p2;

    //std::cout << p1.derivative();
    //std::cout << p3.derivative();
    //std::cout << p1.eval(0.5) << "\n";
}
