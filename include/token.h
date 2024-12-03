#ifndef GETTOKEN
#define GETTOKEN
#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
#include <vector>

struct Result {
  std::string access_token;
  std::vector<std::string> enabled_features;
  int expires_in;
  std::string refresh_token;
  std::string scope;
  std::string sid;
  std::string state;
  std::string token_type;
};

struct Response {
  int id;
  std::string jsonrpc;
  Result result;
  long long usIn;
  long long usOut;
  long long usDiff;
  bool testnet;
};

class TokenManager {
public:
  Response getToken();
};

#endif // !GETTOKEN
