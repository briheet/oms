#include "./include/CLI11.hpp"
#include "include/p.h"

int main(int argc, char **argv) {
  CLI::App app("Welcome to order system and execution manager");

  // Test
  int p = 0;
  app.add_option("-p", p, "Outputs the value of p");
  app.callback([&]() { p_value(p); });
  CLI11_PARSE(app, argc, argv);
}
