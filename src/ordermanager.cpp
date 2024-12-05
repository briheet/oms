#include "../include/ordermanager.h"
#include "../include/json.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/header.h>
#include <stdexcept>
using json = nlohmann::json;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            std::string *userp) {

  size_t totalSize = size * nmemb;
  userp->append((char *)contents, totalSize);

  return totalSize;
}

int Order::place_order(const OrderRequest &request) {
  std::string url = "https://test.deribit.com/api/v2/private/buy?";
  url += "amount=" + std::to_string(request.Amount);
  url += "&instrument_name=" + std::string(request.Instrument);
  url += "&label=market01";
  url += "&type=" + std::string(request.Order_type);
  url += "&price=0";

  std::string bearer = request.Bearer;

  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

    struct curl_slist *headers = NULL;
    headers =
        curl_slist_append(headers, ("Authorization: Bearer " + bearer).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << std::endl;
      throw std::runtime_error("CURL request failed");
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  } else {
    throw std::runtime_error("Failed to initialize CURL");
  }

  return 0;
}

int Order::cancel_order(const CancelRequest &request) {
  std::string url = "https://test.deribit.com/api/v2/private/cancel?";
  url += "order_id=" + std::string(request.Order_Id);

  // Check if it is cancellable or not
  int validCheck = isCancelable(request);
  if (validCheck != 0) {
    throw std::runtime_error("The order is not cancellable");
    return 1;
  }

  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + request.Bearer).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << std::endl;
      throw std::runtime_error("CURL request failed");
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  } else {
    throw std::runtime_error("Failed to initialize CURL");
  }

  return 0;
}

int Order::isCancelable(const CancelRequest &request) {

  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();

  std::string url = "https://test.deribit.com/api/v2/private/get_order_state?";
  url += "order_id=" + std::string(request.Order_Id);

  std::string responseData = "";

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + request.Bearer).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << std::endl;
      throw std::runtime_error("CURL request failed");
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
  } else {

    throw std::runtime_error("Failed to initialize CURL");
  }

  std::string time_in_force = "";

  try {

    auto jsonResponse = json::parse(responseData);

    if (jsonResponse.contains("result") && !jsonResponse["result"].is_null()) {

      auto result = jsonResponse["result"];
      time_in_force =
          result.contains("time_in_force") && !result["time_in_force"].is_null()
              ? result["time_in_force"].get<std::string>()
              : "";
    } else {
      throw std::runtime_error(
          "jsonResponse does not contain time_in_force parameter");
      return 1;
    }

  } catch (const json::exception &e) {

    std::cerr << "JSON parsing error: " << e.what() << std::endl;
    throw std::runtime_error("Failed to parse JSON response");
  }

  if (time_in_force != "good_til_cancelled") {
    throw std::runtime_error(
        "The order is not of the type good_till_cancelled");
    return 1;
  }

  return 0;
}

int Order::modify_order(const ModifyRequest &request) {

  // Check if it can be modified or not by checking the order_state in
  // /private/get_order_state. If can be modified, please modify it then

  int validCheck = isModifyAble(request);
  if (validCheck != 0) {
    throw std::runtime_error("The order is not modifyable");
    return 1;
  }

  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();

  std::string url = "https://test.deribit.com/api/v2/private/edit?";
  url += "amount=" + std::to_string(request.Amount);
  url += "&order_id=" + std::string(request.Order_Id);

  if (curl) {

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1l);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + request.Bearer).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << std::endl;
      throw std::runtime_error("CURL request failed");
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

  } else {
    throw std::runtime_error("Failed to initialize CURL");
  }

  return 0;
}

int Order::isModifyAble(const ModifyRequest &request) {

  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();

  std::string url = "https://test.deribit.com/api/v2/private/get_order_state?";
  url += "order_id=" + std::string(request.Order_Id);

  std::string responseData = "";

  if (curl) {

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1l);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, ("Authorization: Bearer " + request.Bearer).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << std::endl;
      throw std::runtime_error("CURL request failed");
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

  } else {
    throw std::runtime_error("Failed to initialize CURL");
  }

  std::string order_state = "";

  try {

    auto jsonResponse = json::parse(responseData);

    if (jsonResponse.contains("result") && !jsonResponse["result"].is_null()) {

      auto result = jsonResponse["result"];
      order_state =
          result.contains("order_state") && !result["order_state"].is_null()
              ? result["order_state"].get<std::string>()
              : "";
    } else {

      throw std::runtime_error(

          "jsonResponse does not contain order_state parameter");
      return 1;
    }

  } catch (json::exception &e) {

    std::cerr << "JSON parsing error: " << e.what() << std::endl;
    throw std::runtime_error("Failed to parse JSON response");
  }

  if (order_state != "open") {

    throw std::runtime_error("The order is not open for modifying");
    return 1;
  }

  return 0;
}

int Order::getOrderBook(const GetOrderBook &request) {

  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();

  std::string url = "https://test.deribit.com/api/v2/public/get_order_book";
  url += "?depth=" + std::to_string(request.Depth);
  url += "&instrument_name=" + std::string(request.InstrumentName);

  if (curl) {

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << std::endl;

      throw std::runtime_error("CURL request failed");
    }

  } else {
    throw std::runtime_error("Failed to initialize curl");
  }

  return 0;
}
