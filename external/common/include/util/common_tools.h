#ifndef COMMON_TOOLS_H_
#define COMMON_TOOLS_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>

#include <unordered_map>
#include <chrono>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cctype>
#include <queue>
#include <stack>
#include <chrono>
#include <thread>

#include <zmq.hpp>

#include "struct/market_snapshot.h"
#include "util/base_recver.hpp"
#include "util/base_sender.hpp"
#include "util/zmq_recver.hpp"
#include "util/zmq_sender.hpp"
#include "util/shm_recver.hpp"
#include "util/shm_sender.hpp"

using namespace std;
using namespace std::chrono;

// std::vector<std::string> Split(std::string raw_string, char split_char=' ');
bool CheckAddressLegal(std::string address);
bool CheckTimeStringLegal(std::string time);
void Register(std::unordered_map<std::string, int>* m, std::string contract, int no);
int Translate(std::string time);
std::string ExtractString(std::string s, char rm_char=' ');

template <class T1, class T2>
void PrintMap(std::unordered_map<T1, T2>map) {
  cout << "Start print map:";
  // for (class std::unordered_map<T1, T2>::iterator it = map.begin(); it != map.end(); it++) {
  for (auto it : map) {
    cout << "map[" << it.first << "]=" << it.second << " ";
  }
  cout << "End print map" << endl;
}

template <class T1, class T2>
void PrintMap(std::map<T1, T2>map) {
  cout << "Start print map:";
  // for (class std::unordered_map<T1, T2>::iterator it = map.begin(); it != map.end(); it++) {
  for (auto it : map) {
    cout << "map[" << it.first << "]=" << it.second << " ";
  }
  cout << "End print map" << endl;
}

template <class T>
void PrintVector(std::vector<T>v) {
  cout << "Start print vector:\n";
  // for (unsigned int i = 0; i < v.size(); i++) {
  for (auto i : v) {
    cout << i << "\n";
  }
  cout << "End print vector" << endl;
}
template <class T1, class T2>
std::string MapString(std::unordered_map<T1, T2>map, std::string id) {
  std::string head = "****************************";
  head +=  id;
  head += "head****************************\n";
  stringstream ss;
  for (class std::unordered_map<T1, T2>::iterator it = map.begin(); it != map.end(); it++) {
    ss << "map[" << it->first << "]=" << it->second << ";";
  }
  std::string content;
  ss >> content;
  std::string tail = "\n****************************";
  tail += id;
  tail += "tail****************************\n\n";
  return head + content + tail;
}

template <class T>
std::string VectorString(std::vector<T>v, std::string id) {
  std::string head = "****************************";
  head += id;
  head += " head****************************\n";
  stringstream ss;
  for (unsigned int i = 0; i < v.size(); i++) {
    ss << v[i] << ";";
  }
  std::string content;
  ss >> content;
  std::string tail = "\n****************************";
  tail += id;
  tail += " tail****************************\n\n";
  return head + content + tail;
}


template <class T>
bool CheckVSize(std::vector<T>v, int size) {
  int real_size = v.size();
  if (real_size == size) {
    return true;
  } else {
    // printf("check vector size failed, size if %d, require_size is %d\n", real_size, size);
    // PrintVector(v);
    return false;
  }
}

double PriceCorrector(double price, double min_price, bool is_upper = false);
bool DoubleEqual(double a, double b, double min_vaule = 0.0000001);
bool DoubleGreater(double a, double b, double min_vaule = 0.0000001);
bool DoubleLess(double a, double b, double min_vaule = 0.0000001);
void SimpleHandle(int line);

std::string GetCon(std::string ticker);
std::string GetUsername();
std::string GetDefaultPath();

MarketSnapshot HandleSnapshot(std::string raw_shot);
double TransTime(timeval t);

vector<std::string> GetFiles(std::string path, std::string pattern = "data_binary.dat");
std::string GetDir(std::string path);


template <class T>
static inline std::vector<std::string> Split(const std::string &str, const T &delim, const bool trim_empty = false){
  size_t pos, last_pos = 0, len;
  std::vector<std::string> tokens;

  std::string delim_s = "";
  delim_s += delim;

  while(true) {
    pos = str.find(delim_s, last_pos);
    if (pos == std::string::npos) {
      pos = str.size();
    }

    len = pos-last_pos;
    if ( !trim_empty || len != 0) {
      tokens.push_back(str.substr(last_pos, len));
    }

    if (pos == str.size()) {
      break; 
    } else {
      last_pos = pos + delim_s.size();
    }
  }

  return tokens;
}

template<typename T>
void SaveBin(std::ofstream &stream, const T& t) {
  stream.write((char*)&t, sizeof(T));
  stream.flush();
}

void EnsureDir(const std::string & dir);
void busy_sleep(std::chrono::nanoseconds t);

template <typename T>
std::tuple<double, double> CalMeanStd(const T & v, int head, int num) {
  T cal_v(v.begin() + head, v.begin() + head + num);
  double mean = 0.0;
  double std = 0.0;
  for (auto i : cal_v) {
    mean += i;
  }
  mean /= num;
  for (auto i : cal_v) {
    std += (i-mean) * (i-mean);
  }
  std /= num;
  std = sqrt(std);
  return std::tie(mean, std);
}

double RoundPrice(double price, double min_price, int size = 1);

static void RunDataProxy();
static void RunOrderProxy();
static void RunExchangeinfoProxy();
std::vector<std::thread*> run_proxy();

template <typename T1, template<typename> class T2>
BaseSender<T1>* CreateSender(const std::string& mode) {
  if (mode == "order") {
    return new T2<T1>("strategy_order", "connect", "ipc", "order.dat");
  } else if (mode == "data") {
    return new T2<T1>("external_data", "connect", "ipc");
  } else if (mode == "exchangeinfo") {
    return new T2<T1>("external_exchangeinfo", "connect", "ipc");
  } else {
    printf("sender unknown mode!%s\n", mode.c_str());
    return nullptr;
  }
}

template <typename T1, template<typename> class T2>
BaseSender<T1>* CreateRecver(const std::string& mode) {
  if (mode == "order") {
    return new T2<T1>("external_order");
  } else if (mode == "data") {
    return new T2<T1>("strategy_data");
  } else if (mode == "exchangeinfo") {
    return new T2<T1>("strategy_exchangeinfo");
  } else {
    printf("recver unknown mode!%s\n", mode.c_str());
    return nullptr;
  }
}

#endif // COMMON_TOOLS_H_
