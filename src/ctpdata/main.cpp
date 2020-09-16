#include <ThostFtdcMdApi.h>
#include <stdlib.h>
#include <util/zmq_sender.hpp>
#include <util/shm_worker.hpp>
#include <sys/time.h>
#include <unordered_map>
#include <struct/market_snapshot.h>
#include <util/common_tools.h>

#include <string>
#include <ctime>
#include <vector>
#include <fstream>

class Listener : public CThostFtdcMdSpi {
 public:
  Listener(CThostFtdcMdApi* user_api,
           const std::string & broker_id,
           const std::string & user_id,
           const std::string & password,
           bool binary_record = true,
           bool show_stdout = true,
           bool file_record = false)
    : user_api_(user_api),
      broker_id_(broker_id),
      user_id_(user_id),
      password_(password),
      request_id_(0),
      is_publishing_(false),
      record_binary(binary_record),
      record_stdout(show_stdout),
      record_file(file_record) {
    sender = new ZmqSender<MarketSnapshot> ("data_sender", "connect");
    time_t time_seconds = time(0);
    struct tm now_time;
    localtime_r(&time_seconds, &now_time);
    char date[32];
    strftime(date, sizeof(date), "%Y-%m-%d", &now_time);
    std::string file_name = "future";
    file_name += date;
    file_name += ".dat";
    printf("data file is %s\n", file_name.c_str());
    binary_file.open(file_name.c_str(), ios::app | ios::out | ios::binary);
  }
  ~Listener() {
    if (record_file) {
      fclose(data_file);
    }
    if (record_binary) {
        binary_file.close();
    }
    delete sender;
  }

  virtual void OnRspError(CThostFtdcRspInfoField* info, int request_id, bool is_last) {
    printf("here reached 31\n");
  }

  virtual void OnFrontConnected() {
    printf("on front connected\n");
    SendLogin();
  }

  virtual void OnFrontDisconnected(int reason) {
    printf("front disconnected!\n");
  }
  virtual void OnHeartBeatWarning(int time_lapse) {
  }

  virtual void OnRspUserLogin(CThostFtdcRspUserLoginField* user_login,
                              CThostFtdcRspInfoField* info,
                              int request_id,
                              bool is_last) {
    printf("rsp login called!\n");
    Subscribe();
  }

  virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField* specific_instrument,
                                  CThostFtdcRspInfoField* info,
                                  int request_id,
                                  bool is_last) {
    if (!CheckError("OnRspSubMarketData", info)) {
      // printf("Subscribed to %s", specific_instrument->InstrumentID);
    }
  }

  virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField* specific_instrument,
                                    CThostFtdcRspInfoField* info,
                                    int request_id,
                                    bool is_last) {
  }

  virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *market_data) {
    if (market_data == 0) {
      printf("Received null market_data");
      return;
    }

    MarketSnapshot snapshot;
    snapshot.is_initialized = true;
    gettimeofday(&snapshot.time, NULL);

    std::string our_id = market_data->InstrumentID;
    if (our_id.size() == 0) {
      printf("Unknown exchange instrument! %s", market_data->InstrumentID);
      return;
    }

    strncpy(snapshot.ticker,
            our_id.c_str(),
            sizeof(snapshot.ticker));

    snapshot.is_trade_update = false;
    snapshot.last_trade = market_data->LastPrice;
    snapshot.volume = market_data->Volume;
    snapshot.turnover = market_data->Turnover;
    snapshot.open_interest = market_data->OpenInterest;

    snapshot.bid_sizes[0] = market_data->BidVolume1;
    snapshot.bid_sizes[1] = market_data->BidVolume2;
    snapshot.bid_sizes[2] = market_data->BidVolume3;
    snapshot.bid_sizes[3] = market_data->BidVolume4;
    snapshot.bid_sizes[4] = market_data->BidVolume5;

    snapshot.ask_sizes[0] = market_data->AskVolume1;
    snapshot.ask_sizes[1] = market_data->AskVolume2;
    snapshot.ask_sizes[2] = market_data->AskVolume3;
    snapshot.ask_sizes[3] = market_data->AskVolume4;
    snapshot.ask_sizes[4] = market_data->AskVolume5;

    snapshot.bids[0] = snapshot.bid_sizes[0] != 0 ? market_data->BidPrice1 : 0;
    snapshot.bids[1] = snapshot.bid_sizes[1] != 0 ? market_data->BidPrice2 : 0;
    snapshot.bids[2] = snapshot.bid_sizes[2] != 0 ? market_data->BidPrice3 : 0;
    snapshot.bids[3] = snapshot.bid_sizes[3] != 0 ? market_data->BidPrice4 : 0;
    snapshot.bids[4] = snapshot.bid_sizes[4] != 0 ? market_data->BidPrice5 : 0;

    snapshot.asks[0] = snapshot.ask_sizes[0] != 0 ? market_data->AskPrice1 : 0;
    snapshot.asks[1] = snapshot.ask_sizes[1] != 0 ? market_data->AskPrice2 : 0;
    snapshot.asks[2] = snapshot.ask_sizes[2] != 0 ? market_data->AskPrice3 : 0;
    snapshot.asks[3] = snapshot.ask_sizes[3] != 0 ? market_data->AskPrice4 : 0;
    snapshot.asks[4] = snapshot.ask_sizes[4] != 0 ? market_data->AskPrice5 : 0;

    for (int i = 5; i < MARKET_DATA_DEPTH; i++) {
      snapshot.bid_sizes[i] = 0;
      snapshot.ask_sizes[i] = 0;
      snapshot.bids[i] = 0;
      snapshot.asks[i] = 0;
    }
    // snapshot.Show(stdout, 5);
    sender->Send(snapshot);
    if (record_file) {
      snapshot.Show(data_file, 5);
    }
    if (record_binary) {
      SaveBin(binary_file, snapshot);
    }
  }

 private:
  void SendLogin() {
    CThostFtdcReqUserLoginField request;
    memset(&request, 0, sizeof(request));

    strncpy(request.BrokerID, broker_id_.c_str(), sizeof(request.BrokerID));
    strncpy(request.UserID, user_id_.c_str(), sizeof(request.UserID));
    strncpy(request.Password, password_.c_str(), sizeof(request.Password));

    int result = user_api_->ReqUserLogin(&request, ++request_id_);
    printf("Logging in as %s\n", user_id_.c_str());

    if (result != 0) {
      printf("SendLogin failed! (%d)", result);
      exit(1);
    }
  }
  void Subscribe() {
    // char *g_pInstrumentID[2] = {"ni1901", "ni1807"};
    fstream file;
    file.open("instruments.conf", ios::in);
    if (!file) {
      char a[6][32];
      snprintf(a[0], sizeof(a[0]), "%s", "ni1901");
      snprintf(a[1], sizeof(a[1]), "%s", "ni1905");
      snprintf(a[2], sizeof(a[2]), "%s", "hc1810");
      snprintf(a[3], sizeof(a[3]), "%s", "hc1809");
      snprintf(a[4], sizeof(a[4]), "%s", "zn1901");
      snprintf(a[5], sizeof(a[5]), "%s", "zn1903");
      char * g_pInstrumentID[6];
      g_pInstrumentID[0] = a[0];
      g_pInstrumentID[1] = a[1];
      g_pInstrumentID[2] = a[2];
      g_pInstrumentID[3] = a[3];
      g_pInstrumentID[4] = a[4];
      g_pInstrumentID[5] = a[5];
      int instrumentNum = 6;
      int result = user_api_->SubscribeMarketData(g_pInstrumentID, instrumentNum);
      if (result != 0) {
        printf("sub failed!");
        exit(1);
      }
      sleep(1);
    } else {
      char a[65536][64];
      char * g_pInstrumentID[65536];
      int count = 0;
      while (!file.eof()) {
        file.getline(a[count], 64);
        g_pInstrumentID[count] = a[count];
        count++;
      }
      printf("sending %s\n", g_pInstrumentID[0]);
      int result = user_api_->SubscribeMarketData(g_pInstrumentID, count);
      if (result != 0) {
        printf("sub failed!");
        exit(1);
      }
      sleep(1);
    }
  }
  bool CheckError(const std::string & location, CThostFtdcRspInfoField* info) {
    bool is_error = info && info->ErrorID != 0;
    if (is_error) {
      printf("Error %d at %s, Msg: %s", info->ErrorID, location.c_str(), info->ErrorMsg);
    }
    return is_error;
  }

  CThostFtdcMdApi* user_api_;

  std::string broker_id_;
  std::string user_id_;
  std::string password_;

  int request_id_;

  bool is_publishing_;

  FILE* data_file;
  bool record_binary;
  bool record_stdout;
  bool record_file;
  std::ofstream binary_file;
  // ZmqSender* sender;
  ZmqSender<MarketSnapshot> * sender;
};

int main() {
  CThostFtdcMdApi* user_api = CThostFtdcMdApi::CreateFtdcMdApi();

  Listener listener(
    user_api,
    "8060",
    "",
    "");

  user_api->RegisterSpi(&listener);

  // std::string front = "tcp://180.168.146.187:10110";  // fangzheng
  std::string front = "tcp://180.166.65.122:41213";  // wcs
  user_api->RegisterFront(const_cast<char*>(front.c_str()));

  user_api->Init();

  user_api->Join();
  user_api->Release();
  return 0;
}
