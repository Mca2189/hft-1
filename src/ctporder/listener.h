#ifndef SRC_CTPORDER_LISTENER_H_
#define SRC_CTPORDER_LISTENER_H_

#include <ThostFtdcTraderApi.h>
#include <stdint.h>
#include <unordered_map>

#include <map>
#include <string>

#include "struct/exchange_info.h"
#include "struct/info_type.h"
// #include "util/zmq_sender.hpp"
// #include "util/contract_worker.h"
#include "./token_manager.h"

class MessageSender;

class Listener : public CThostFtdcTraderSpi {
 public:
  Listener(const std::string & order_update_path,
           MessageSender* message_sender,
           const std::string & error_list,
           std::unordered_map<int, int>* id_map,
           TokenManager* tm,
           // ContractWorker* cw,
           bool enable_stdout = true,
           bool enable_file = true);
  ~Listener();

  virtual void OnRspError(CThostFtdcRspInfoField* info, int request_id, bool is_last);

  virtual void OnFrontConnected();
  virtual void OnFrontDisconnected(int reason);
  virtual void OnHeartBeatWarning(int time_lapse);
  virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  virtual void OnRspUserLogin(CThostFtdcRspUserLoginField* user_login,
                              CThostFtdcRspInfoField* info,
                              int request_id,
                              bool is_last);

  virtual void OnRspOrderInsert(CThostFtdcInputOrderField* order,
                                CThostFtdcRspInfoField* info,
                                int request_id,
                                bool is_last);
  virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField* order,
                                   CThostFtdcRspInfoField *info);

  virtual void OnRspOrderAction(CThostFtdcInputOrderActionField* order_action,
                                CThostFtdcRspInfoField* info,
                                int request_id,
                                bool is_last);
  virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField* order,
                                   CThostFtdcRspInfoField *info);

  virtual void OnRtnOrder(CThostFtdcOrderField* order);
  virtual void OnRtnTrade(CThostFtdcTradeField* trade);

  virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField* trading_account,
                                      CThostFtdcRspInfoField* info,
                                      int request_id,
                                      bool is_last);

  virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField* investor_position,
                                        CThostFtdcRspInfoField* info,
                                        int request_id,
                                        bool is_last);

  virtual void OnRspSettlementInfoConfirm(
    CThostFtdcSettlementInfoConfirmField* settlement_info,
    CThostFtdcRspInfoField* info,
    int request_id,
    bool is_last);

 private:
  /*
  void HandleOrder(OrderUpdateReason::Enum reason,
                   OrderStatus::Enum status,
                   const char* order_ref,
                   const char direction,
                   int total_size_filled,
                   const OrderUniqueId* oui = NULL);
  void HandleFailedCancel();
  */

  void SendExchangeInfo(const ExchangeInfo& info);
  bool CheckError(const std::string & location,
                  CThostFtdcRspInfoField* info,
                  const std::string & extra_info = "");

  // only DCE will auto combine, CZCE won't. we didn't trade combo instrument, so just call it if got combo instrument position
  // bool HandleComboPosition(const CThostFtdcInvestorPositionField* investor_position);

  // ZmqSender<ExchangeInfo>* sender;
  MessageSender* message_sender_;

  TThostFtdcFrontIDType front_id_;
  TThostFtdcSessionIDType session_id_;
  int order_ref_;

  bool initialized_;

  std::map<std::string, int> startup_positions_;

  std::map<int, std::string> error_list_;
  std::unordered_map<int, int>* order_id_map;
  TokenManager* t_m;
  // ContractWorker* cw;
  FILE* exchange_file;
  FILE* position_file;
  bool e_s;
  bool e_f;
};

#endif  // SRC_CTPORDER_LISTENER_H_
