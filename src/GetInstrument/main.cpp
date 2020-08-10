#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ThostFtdcTraderApi.h>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

std::string broker_id_ = "8060";
std::string user_id_ = "9030001896";
std::string passwd_ = "yifeng";

template<class T>
void SafeStrCopy(T dest, const char* source) {
  snprintf(dest, source, sizeof(dest));
}

std::vector<std::vector<std::string> > instruments;
bool g_is_last = false;

class Listener : public CThostFtdcTraderSpi {
 public:
  explicit Listener(CThostFtdcTraderApi* user_api)
    : user_api_(user_api),
      request_id_(1) {
  }

  void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    CThostFtdcReqUserLoginField request;
    memset(&request, 0, sizeof(request));

    snprintf(request.BrokerID, sizeof(request.BrokerID), "%s", broker_id_.c_str());
    snprintf(request.UserID, sizeof(request.UserID), "%s", user_id_.c_str());
    snprintf(request.Password, sizeof(request.Password), "%s", passwd_.c_str());

    printf("Connected\n");

    int result = user_api_->ReqUserLogin(&request, ++request_id_);
    if (result != 0) {
      fprintf(stderr, "Error logging in!");
      exit(1);
    }
  }


  virtual void OnFrontConnected() {
    Auth();
  }

  void Auth() {
    static const char *version = user_api_->GetApiVersion();
    printf("Version is %s\n", version);
    CThostFtdcReqAuthenticateField reqauth;
    memset(&reqauth, 0, sizeof(reqauth));
    snprintf(reqauth.BrokerID, sizeof(reqauth.BrokerID), "%s", broker_id_.c_str());
    snprintf(reqauth.UserID, sizeof(reqauth.UserID), "%s", user_id_.c_str());
    // snprintf(reqauth.AppID, sizeof(reqauth.AppID), "%s", "client_hft_168");
    // snprintf(reqauth.AuthCode, sizeof(reqauth.AuthCode), "%s", "9DEYFJ0E8189C29C");
    snprintf(reqauth.AppID, sizeof(reqauth.AppID), "%s", "client_9030001896_v1");
    snprintf(reqauth.AuthCode, sizeof(reqauth.AuthCode), "%s", "AQ1963JWQ6ATP9FE");

    printf("Get Auth...\n");
    int result = user_api_->ReqAuthenticate(&reqauth, ++request_id_);
    if (result != 0) {
      printf("Get Auth failed!(%d)\n", result);
      sleep(1);
      throw std::runtime_error("Auth failed");
    }
  }

    void OnRspUserLogin(CThostFtdcRspUserLoginField* user_login,
                      CThostFtdcRspInfoField* info,
                      int request_id,
                      bool is_last) {
    printf("Logged in successfully\n");

    CThostFtdcQryInstrumentField instrument_field;
    memset(&instrument_field, 0, sizeof(instrument_field));
    int result = user_api_->ReqQryInstrument(&instrument_field, ++request_id_);
    if (result != 0) {
      fprintf(stderr, "Error from login response!");
      exit(1);
    }
  }

  virtual void OnRspQryInstrument(
      CThostFtdcInstrumentField* instrument,
      CThostFtdcRspInfoField* info,
      int request_id,
      bool is_last) {
    std::vector<std::string> entry;
    entry.emplace_back(instrument->InstrumentID);
    char buf[500];
    snprintf(buf, sizeof(buf), "%s/%d/%lf",
      instrument->ExchangeID,
      instrument->VolumeMultiple,
      instrument->PriceTick);
    entry.emplace_back(buf);
    instruments.emplace_back(entry);

    printf("%c", is_last ? '\n' : '.');
    fflush(stdout);

    g_is_last = is_last;
  }

 private:
  CThostFtdcTraderApi* user_api_;
  int request_id_;
};

int main(int argc, char* argv[]) {
  CThostFtdcTraderApi* user_api = CThostFtdcTraderApi::CreateFtdcTraderApi();
  Listener listener(user_api);
  user_api->RegisterSpi(&listener);

  user_api->SubscribePrivateTopic(THOST_TERT_RESTART);
  user_api->SubscribePublicTopic(THOST_TERT_RESTART);

  std::string counterparty_host = "tcp://180.166.65.121:41205";
  user_api->RegisterFront(const_cast<char*>(counterparty_host.c_str()));
  user_api->Init();

  while (!g_is_last) {
    usleep(10000);
  }

  FILE* file = fopen("instruments.conf", "w");
  // fprintf(file, "// Generated from %s\n", argv[0]);
  // fprintf(file, "instruments = [\n");

  std::sort(instruments.begin(), instruments.end());
  for (size_t i = 0; i < instruments.size(); ++i) {
    /*
    fprintf(file, "\"%s\"%c // %s\n",
      instruments[i][0].c_str(),
      i == instruments.size() - 1 ? ' ' : ',',
      instruments[i][1].c_str());
    */
    fprintf(file, "%s\n", instruments[i][0].c_str());
  }
  // fprintf(file, "];\n");
  fclose(file);

  return 0;
}
