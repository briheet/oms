#include "../include/ordermanager.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <stdexcept>

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
