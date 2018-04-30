#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <complex>

#include <iostream>

#include "polynomial.hpp"

int main(int argc, char** argv)
{

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
