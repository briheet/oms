#include "./include/CLI11.hpp"
#include "./include/ordermanager.h"
#include "./include/token.h"
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <stdexcept>
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
  auto start = std::chrono::high_resolution_clock::now();
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

  // Modify or Edit Order
  double modify_amount = 0;
  std::string modify_orderId = "";
  auto modify_cmd = app.add_subcommand("modify", "Modifies an existing order");
  modify_cmd
      ->add_option("--amount", modify_amount,
                   "The amount that the user wants to work on")
      ->required();
  modify_cmd
      ->add_option("--order_id", modify_orderId,
                   "Order ID that you want to change")
      ->required();

  // Get the order book
  std::string orderBook_instrument_name = "";
  int orderBook_depth = 0;
  auto orderBook_cmd =
      app.add_subcommand("orderBook", "Gets the present orderbook");

  orderBook_cmd
      ->add_option("--instrument_name", orderBook_instrument_name,
                   "Takes in the instrument name for the order book")
      ->required();
  orderBook_cmd
      ->add_option("--depth", orderBook_depth,
                   "Takes in the depth for the number of entries to "
                   "return for bids and asks.")
      ->required();

  // View current positions
  std::string view_current_pos_currency = "";
  std::string view_current_pos_kind = "";
  auto view_current_pos_cmd = app.add_subcommand(
      "current_pos", "Gets the current positions by currency and kind");
  view_current_pos_cmd
      ->add_option("--curr", view_current_pos_currency, "Needs currency name")
      ->required();
  view_current_pos_cmd
      ->add_option("--kind", view_current_pos_kind, "Needs kind")
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
  } else if (modify_cmd->parsed()) {

    if (modify_amount <= 0.0 || modify_orderId.empty()) {
      std::cerr << "Error: Not a valid amount or orderId" << std::endl;
      throw std::runtime_error("Please enter valid credentials");
    }

    TokenManager tokenManager;
    Response response = tokenManager.getToken();

    ModifyRequest modifyRequest;
    modifyRequest.Bearer = response.result.access_token;
    modifyRequest.Amount = modify_amount;
    modifyRequest.Order_Id = modify_orderId;

    Order orderManager;
    int ok = orderManager.modify_order(modifyRequest);

    if (ok != 0) {
      std::cerr << "Error: Failed to modify order!" << std::endl;
      return 1;
    } else {
      std::cout << "Order modify successfully!" << std::endl;
    }
  } else if (orderBook_cmd->parsed()) {

    if (orderBook_depth <= 0) {
      std::cerr << "The depth of the order book is less than or equal to 0"
                << std::endl;
      throw std::runtime_error("Check the depth value");
      return 1;
    } else if (orderBook_instrument_name.empty()) {
      std::cerr << "The instrument_name is empty" << std::endl;

      throw std::runtime_error("Check the instrument_name");
      return 1;
    }

    GetOrderBook request;
    request.Depth = orderBook_depth;
    request.InstrumentName = orderBook_instrument_name;

    Order orderManager;
    int ok = orderManager.getOrderBook(request);

    if (ok != 0) {
      std::cerr << "Failed to get order book" << std::endl;
      return 1;
    } else {
      std::cout << "Got the order book successfully" << std::endl;
    }
  } else if (view_current_pos_cmd->parsed()) {

    if (view_current_pos_currency.empty()) {
      std::cerr << "Currency is empty" << std::endl;
      throw std::runtime_error("Please entry currency");
      return 1;
    }

    if (view_current_pos_kind.empty()) {
      std::cerr << "Kind is empty" << std::endl;
      throw std::runtime_error("Please enter a valid kind");
      return 1;
    }

    TokenManager tokenManager;
    Response response = tokenManager.getToken();

    ViewCurrentPositions request;
    request.Bearer = response.result.access_token;
    request.Kind = view_current_pos_kind;
    request.Currency = view_current_pos_currency;

    Order orderManager;
    int ok = orderManager.viewCurrentOrderPositions(request);

    if (ok != 0) {
      std::cerr << "Failed to view current positions" << std::endl;
      return 1;
    } else {
      std::cout << "Got the positions successfully" << std::endl;
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;

  return 0;
}
