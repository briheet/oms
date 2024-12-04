#include "./include/CLI11.hpp"
#include "./include/ordermanager.h"
#include "./include/token.h"
#include <cstdlib>
#include <ostream>
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

  // Place Order
  std::string place_instrument_name;
  double place_amount = 0.0;
  std::string place_order_type = "market";
  auto place_cmd = app.add_subcommand("place", "Place an order");
  place_cmd
      ->add_option("--instrument", place_instrument_name,
                   "Instrument name (e.g., ETH-PERPETUAL)")
      ->required();
  place_cmd->add_option("--amount", place_amount, "Order amount")->required();
  place_cmd
      ->add_option("--type", place_order_type, "Order type (market, limit)")
      ->required();

  // Cancel Order
  std::string cancel_order_id = "";
  auto cancel_cmd = app.add_subcommand("cancel", "Cancels an order");
  cancel_cmd->add_option("--order_id", cancel_order_id, "Placed Order Id")
      ->required();

  CLI11_PARSE(app, argc, argv);

  if (place_cmd->parsed()) {
    if (place_instrument_name.empty() || place_amount <= 0.0 ||
        place_order_type.empty()) {
      std::cerr << "Error: Instrument name and amount and valid market are "
                   "required to place an order."
                << std::endl;
      return 1;
    }

    TokenManager tokenmanager;
    Response response = tokenmanager.getToken();

    OrderRequest newOrder;
    newOrder.Instrument = place_instrument_name;
    newOrder.Amount = place_amount;
    newOrder.Order_type = place_order_type;
    newOrder.Bearer = response.result.access_token;

    Order orderManager;
    int ok = orderManager.place_order(newOrder);
    if (ok != 0) {
      std::cerr << "Error: Failed to place order!" << std::endl;
      return 1;
    } else {
      std::cout << "Order placed successfully!" << std::endl;
    }

  } else if (cancel_cmd->parsed()) {

    if (cancel_order_id.empty()) {
      std::cerr << "Error: Not a valid order_id" << std::endl;
      throw std::runtime_error("Please enter a valid order_id");
    }

    TokenManager tokerManager;
    Response response = tokerManager.getToken();

    CancelRequest cancelOrder;
    cancelOrder.Bearer = response.result.access_token;
    cancelOrder.Order_Id = cancel_order_id;

    Order orderManager;
    int ok = orderManager.cancel_order(cancelOrder);
    if (ok != 0) {
      std::cerr << "Error: Failed to cancel order!" << std::endl;
      return 1;
    } else {
      std::cout << "Order cancel successfully!" << std::endl;
    }
  }

  return 0;
}
