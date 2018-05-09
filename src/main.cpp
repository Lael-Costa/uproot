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

bool keys_held[400];

static void onKeyPress(GLFWwindow *window,
  int key,
  int scancode,
  int action,
  int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  if (key != GLFW_KEY_UNKNOWN && action == GLFW_PRESS) {
    keys_held[key] = true;
  }
  if (key != GLFW_KEY_UNKNOWN && action == GLFW_RELEASE) {
    keys_held[key] = false;
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
  srand(time(NULL));
  for (int i = 0; i < 400; i++) {
    keys_held[i] = false;
  }

  if (!glfwInit())
  {
    std::cerr << "GLFW initialization failed!" << std::endl;
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(1000, 1000, "Uproot", NULL, NULL);
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

  std::shared_ptr<Shader> shader = std::make_shared<Shader>(
    vertexShaderSource,
    fragmentShaderSource);

  GLuint vaoID;
  glGenVertexArrays(1, &vaoID);
  glBindVertexArray(vaoID);

  static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    +3.0f, -1.0f, 0.0f,
    -1.0f, +3.0f, 0.0f,
  };

  GLuint vboID;
  glGenBuffers(1, &vboID);
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(g_vertex_buffer_data),
    g_vertex_buffer_data,
    GL_STATIC_DRAW);

  float scale = 3.f;
  glm::vec2 pos = glm::vec2(0.f);

  std::vector<std::complex<double>> coeffs(6);
  coeffs[0] = {1, 0};
  coeffs[1] = {0, 0};
  coeffs[2] = {0, 0};
  coeffs[3] = {0, 0};
  coeffs[4] = {0, 0};
  coeffs[5] = {-1, 0};

  polynomial<std::complex<double>> poly(coeffs);



  polynomial<std::complex<double>> deriv = poly.derivative();
  int degree = poly.degree();
  std::vector<std::complex<double>> roots = solve(poly);

  std::cout << "polynomial " << poly << std::endl;
  for (int i = 0; i < degree; i++) {
    std::string istr = "[" + std::to_string(i) + "] ";
    std::complex<double> c = deriv[(degree - 1) - i];
    std::cout << "derivative" + istr << c << std::endl;
    c = roots[i];
    std::cout << "roots" + istr << c << std::endl;
  }

  bool drawn = false;

  glfwSetTime(0.0);
  double time = 0.0;
  double dt = 0.0;

  int coeffIndex = 5;

  while (!glfwWindowShouldClose(window))
  {

    if (!drawn) {
      // double xpos, ypos;
      // glfwGetCursorPos(window, &xpos, &ypos);


      time = glfwGetTime();
      dt = time - dt;

      float xDiff = 0.f;
      float yDiff = 0.f;
      xDiff += keys_held[GLFW_KEY_RIGHT] * scale * 0.05;
      xDiff -= keys_held[GLFW_KEY_LEFT] * scale * 0.05;
      yDiff += keys_held[GLFW_KEY_UP] * scale * 0.05;
      yDiff -= keys_held[GLFW_KEY_DOWN] * scale * 0.05;
      pos.x += xDiff;
      pos.y += yDiff;

      scale *= 1.f + keys_held[GLFW_KEY_S] * 0.05f;
      scale /= 1.f + keys_held[GLFW_KEY_W] * 0.05f;

      if (keys_held[GLFW_KEY_R]) {
        scale = 3.f;
        pos = glm::vec2(0.f);
      }

      if (keys_held[GLFW_KEY_0]) coeffIndex = 5;
      if (keys_held[GLFW_KEY_1]) coeffIndex = 4;
      if (keys_held[GLFW_KEY_2]) coeffIndex = 3;
      if (keys_held[GLFW_KEY_3]) coeffIndex = 2;
      if (keys_held[GLFW_KEY_4]) coeffIndex = 1;
      if (keys_held[GLFW_KEY_5]) coeffIndex = 0;

      std::complex<double> coeff = coeffs[coeffIndex];
      double re = coeff.real();
      double im = coeff.imag();
      re += keys_held[GLFW_KEY_L] * dt;
      re -= keys_held[GLFW_KEY_J] * dt;
      im += keys_held[GLFW_KEY_I] * dt;
      im -= keys_held[GLFW_KEY_K] * dt;
      coeffs[coeffIndex] = std::complex<double>(re, im);

     //  if (keys_held[GLFW_KEY_P]) {
     //  	std::vector<std::complex<double>> new_coeff;
     //  	std::cout << "Please enter the coefficients of your new polynomial" << std::endl;

     //  	  std::string buf;
    	//     while(getline(std::cin, buf)) {
     //    	  std::istringstream ssin(buf);
     //    	  std::complex<double> input;
     //    	  while(ssin >> input) {
     //        	new_coeff.push_back(input);
     //        }
     //      }
     //    poly = polynomial<std::complex<double>>(new_coeff);
     //    deriv = poly.derivative();
  		 //  degree = poly.degree();
  		 //  roots = solve(poly);

  		 //  std::cout << "polynomial " << poly << std::endl;
  		 //  for (int i = 0; i < degree; i++) {
    	// 	  std::string istr = "[" + std::to_string(i) + "] ";
    	// 	  std::complex<double> c = deriv[(degree - 1) - i];
    	// 	  std::cout << "derivative" + istr << c << std::endl;
    	// 	  c = roots[i];
    	// 	  std::cout << "roots" + istr << c << std::endl;
  		 //  }
  	  // }

      poly = polynomial<std::complex<double>>(coeffs);
      deriv = poly.derivative();
      degree = poly.degree();
      roots = solve(poly);

      std::cout << "polynomial " << poly << std::endl;
      for (int i = 0; i < degree; i++) {
        std::string istr = "[" + std::to_string(i) + "] ";
        std::complex<double> c = deriv[(degree - 1) - i];
        std::cout << "derivative" + istr << c << std::endl;
        c = roots[i];
        std::cout << "roots" + istr << c << std::endl;
      }

      std::ostringstream stream;
      stream << poly;
      std::string str = stream.str();
      const char* chr = str.c_str();

      glfwSetWindowTitle(window, chr);

      int width, height;
      glfwGetFramebufferSize(window, &width, &height);
      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);

      shader->bind();

      shader->setUniform("screenSize", glm::vec2(width, height));
      shader->setUniform("pos", pos);
      shader->setUniform("scale", scale);
      shader->setUniform("time", (float) time);
      shader->setUniform("degree", degree);

      for (int i = 0; i < degree + 1; i++) {
        std::complex<double> c = poly[degree - i];
        shader->setUniformArrayByIndex("polynomial", glm::vec2(c.real(), c.imag()), i);

        if (i == degree) break;

        c = deriv[(degree - 1) - i];
        shader->setUniformArrayByIndex("derivative", glm::vec2(c.real(), c.imag()), i);
        c = roots[i];
        shader->setUniformArrayByIndex("roots", glm::vec2(c.real(), c.imag()), i);
      }

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
      // drawn = true;
      dt = time;
      glfwSwapBuffers(window);
    }

    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
