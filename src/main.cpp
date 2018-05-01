#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <complex>

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <streambuf>

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
  glfwSwapInterval(1);
  
  GLenum err = glewInit();
  if (err != GLEW_OK)
  {
    std::cerr << "Awww..." << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  printGLVersion();


  std::ifstream t("./res/shader.vs");
  std::string vertexShaderSource((std::istreambuf_iterator<char>(t)),
    std::istreambuf_iterator<char>());

  std::ifstream t1("./res/shader.fs");
  std::string fragmentShaderSource((std::istreambuf_iterator<char>(t1)),
    std::istreambuf_iterator<char>());

  std::shared_ptr<Shader> shader = std::make_shared<Shader>(vertexShaderSource,
    fragmentShaderSource);

  GLuint vaoID;
  glGenVertexArrays(1, &vaoID);
  glBindVertexArray(vaoID);

  static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    3.0f, -1.0f, 0.0f,
    -1.0f,  3.0f, 0.0f,
  };

  GLuint vboID;
  glGenBuffers(1, &vboID);
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(g_vertex_buffer_data),
    g_vertex_buffer_data,
    GL_STATIC_DRAW);

  while (!glfwWindowShouldClose(window))
  {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->bind();
    shader->setUniform("screenSize", glm::vec2(width, height));
    shader->setUniform("scale", 1.f);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
      0,
      3,
      GL_FLOAT,
      GL_FALSE,
      0,
      (void *) 0
    );

    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
    
    shader->unbind();

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
