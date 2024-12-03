#include "../include/token.h"
#include "../include/json.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
using json = nlohmann::json;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            std::string *userp) {
  size_t totalSize = size * nmemb;
  userp->append((char *)contents, totalSize);
  return totalSize;
}

Response TokenManager::getToken() {
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();

  std::string responseData = "";

  const char *client_id = std::getenv("CLIENT_ID");
  const char *client_secret = std::getenv("CLIENT_SECRET");

  if (!client_id || !client_secret) {
    throw std::runtime_error(
        "CLIENT_ID or CLIENT_SECRET not found in environment variables");
  }

  std::string location = "https://test.deribit.com/api/v2/public/auth?";
  location += "client_id=" + std::string(client_id);
  location += "&client_secret=" + std::string(client_secret);
  location += "&grant_type=client_credentials";

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, location.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << std::endl;
      throw std::runtime_error("CURL request failed");
    }
    curl_easy_cleanup(curl);
  } else {
    throw std::runtime_error("Failed to initialize CURL");
  }
  Response response;

  try {
    auto jsonResponse = json::parse(responseData);

    response.id = jsonResponse.contains("id") && !jsonResponse["id"].is_null()
                      ? jsonResponse["id"].get<int>()
                      : -1;
    response.jsonrpc =
        jsonResponse.contains("jsonrpc") && !jsonResponse["jsonrpc"].is_null()
            ? jsonResponse["jsonrpc"].get<std::string>()
            : "";
    response.usIn =
        jsonResponse.contains("usIn") && !jsonResponse["usIn"].is_null()
            ? jsonResponse["usIn"].get<long long>()
            : 0;
    response.usOut =
        jsonResponse.contains("usOut") && !jsonResponse["usOut"].is_null()
            ? jsonResponse["usOut"].get<long long>()
            : 0;
    response.usDiff =
        jsonResponse.contains("usDiff") && !jsonResponse["usDiff"].is_null()
            ? jsonResponse["usDiff"].get<long long>()
            : 0;
    response.testnet =
        jsonResponse.contains("testnet") && !jsonResponse["testnet"].is_null()
            ? jsonResponse["testnet"].get<bool>()
            : false;

    if (jsonResponse.contains("result") && !jsonResponse["result"].is_null()) {
      auto result = jsonResponse["result"];
      response.result.access_token =
          result.contains("access_token") && !result["access_token"].is_null()
              ? result["access_token"].get<std::string>()
              : "";
      response.result.enabled_features =
          result.contains("enabled_features") &&
                  !result["enabled_features"].is_null()
              ? result["enabled_features"].get<std::vector<std::string>>()
              : std::vector<std::string>();
      response.result.expires_in =
          result.contains("expires_in") && !result["expires_in"].is_null()
              ? result["expires_in"].get<int>()
              : 0;
      response.result.refresh_token =
          result.contains("refresh_token") && !result["refresh_token"].is_null()
              ? result["refresh_token"].get<std::string>()
              : "";
      response.result.scope =
          result.contains("scope") && !result["scope"].is_null()
              ? result["scope"].get<std::string>()
              : "";
      response.result.sid = result.contains("sid") && !result["sid"].is_null()
                                ? result["sid"].get<std::string>()
                                : "";
      response.result.state =
          result.contains("state") && !result["state"].is_null()
              ? result["state"].get<std::string>()
              : "";
      response.result.token_type =
          result.contains("token_type") && !result["token_type"].is_null()
              ? result["token_type"].get<std::string>()
              : "";
    }

  } catch (const json::exception &e) {
    std::cerr << "JSON parsing error: " << e.what() << std::endl;
    throw std::runtime_error("Failed to parse JSON response");
  }
  return response;
}
