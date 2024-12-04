#ifndef ORDERMANAGER
#define ORDERMANAGER
#include <iostream>

struct OrderRequest {
  std::string Instrument;
  double Amount;
  std::string Order_type;
  std::string Bearer;
};

struct CancelRequest {
  std::string Bearer;
  std::string Order_Id;
};

class Order {
public:
  int place_order(const OrderRequest &request);
  int cancel_order(const CancelRequest &request);
  int isCancelable(const CancelRequest &request);
};

#endif // !ORDERMANAGER
