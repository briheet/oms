#include "./include/CLI11.hpp"
#include "include/p.h"
#include <cstdlib>

void load_env(const std::string &file) {
  std::ifstream envFile(file);
  if (!envFile.is_open()) {
    std::cerr << "Error: Unable to open .env file!" << std::endl;
    return;
  }

  std::string line;
  while (std::getline(envFile, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }

    size_t pos = line.find('=');
    if (pos == std::string::npos) {
      continue;
    }

    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);

    key.erase(key.find_last_not_of(" \t\r\n") + 1);
    value.erase(value.find_last_not_of(" \t\r\n") + 1);

    setenv(key.c_str(), value.c_str(), 1); // Use _putenv on Windows
  }
}

int main(int argc, char **argv) {
  load_env(".env");

  const char *client_id = std::getenv("CLIENT_ID");
  const char *client_secret = getenv("CLIENT_SECRET");

  std::cout << client_id << " " << client_secret << std::endl;

  CLI::App app("Welcome to order system and execution manager");

  // Test
  int p = 0;
  app.add_option("-p", p, "Outputs the value of p");
  app.callback([&]() { p_value(p); });

  // app.add_option("-place", )

  CLI11_PARSE(app, argc, argv);
}
