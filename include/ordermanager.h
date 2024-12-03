#ifndef ORDERMANAGER
#define ORDERMANAGER
#include <iostream>

struct OrderRequest {
  std::string Instrument;
  double Amount;
  std::string Order_type;
  std::string Bearer;
};

class Order {
public:
  int place_order(const OrderRequest &request);
};

#endif // !ORDERMANAGER
