#include "./include/CLI11.hpp"
#include "./include/ordermanager.h"
#include "./include/token.h"
#include <cstdlib>
#include <string>

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

    setenv(key.c_str(), value.c_str(), 1);
  }
}

int main(int argc, char **argv) {
  load_env(".env");

  const char *client_id = std::getenv("CLIENT_ID");
  const char *client_secret = std::getenv("CLIENT_SECRET");

  CLI::App app("Welcome to order system and execution manager");

  std::string instrument_name;
  double amount = 0.0;
  std::string order_type = "market";
  auto place_cmd = app.add_subcommand("place", "Place an order");
  place_cmd
      ->add_option("--instrument", instrument_name,
                   "Instrument name (e.g., ETH-PERPETUAL)")
      ->required();
  place_cmd->add_option("--amount", amount, "Order amount")->required();
  place_cmd->add_option("--type", order_type, "Order type (market, limit)")
      ->required();

  CLI11_PARSE(app, argc, argv);

  if (place_cmd->parsed()) {
    if (instrument_name.empty() || amount <= 0.0 || order_type.empty()) {
      std::cerr << "Error: Instrument name and amount and valid market are "
                   "required to place an order."
                << std::endl;
      return 1;
    }

    TokenManager tokenmanager;
    Response response = tokenmanager.getToken();

    OrderRequest newOrder;
    newOrder.Instrument = instrument_name;
    newOrder.Amount = amount;
    newOrder.Order_type = order_type;
    newOrder.Bearer = response.result.access_token;

    Order orderManager;
    int ok = orderManager.place_order(newOrder);
    if (ok != 0) {
      std::cerr << "Error: Failed to place order!" << std::endl;
      return 1;
    } else {
      std::cout << "Order placed successfully!" << std::endl;
    }
  }

  return 0;
}
