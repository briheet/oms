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

struct ModifyRequest {
  std::string Order_Id;
  double Amount;
  std::string Bearer;
};

struct GetOrderBook {
  std::string InstrumentName;
  int Depth;
};

struct ViewCurrentPositions {
  std::string Currency;
  std::string Kind;
  std::string Bearer;
};

class Order {
public:
  int place_order(const OrderRequest &request);
  int cancel_order(const CancelRequest &request);
  int isCancelable(const CancelRequest &request);
  int modify_order(const ModifyRequest &request);
  int isModifyAble(const ModifyRequest &request);
  int getOrderBook(const GetOrderBook &request);
  int viewCurrentOrderPositions(const ViewCurrentPositions &request);
};

#endif // !ORDERMANAGER
