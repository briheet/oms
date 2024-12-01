#include "CLI11.hpp"
#include <iostream>

int main(int argc, char **argv) {

  std::cout << "Start of the program" << std::endl;

  CLI::App app("Welcome to order system and execution manager");

  int p = 0;

  app.add_option("-p", p, "value of p");

  CLI11_PARSE(app, argc, argv);

  std::cout << "Parameter value:" << p << std::endl;
}
