#include "util/dater.h"
#include <iostream>
#include <string>
using namespace std;

std::string TransCoin(std::string ticker) {
  std::string date;
  if (ticker.find("this_week") != ticker.npos) {
    date = Dater::get_weekday_string(5);
    date.erase(std::remove(date.begin(), date.end(), '-'), date.end());
    cout << date << endl;
    date = date.substr(2, 6);
    cout << date << endl;
    return ticker.replace(ticker.find("this_week"), sizeof("this_week"), date);
  } else if (ticker.find("next_week") != ticker.npos) {
    date = Dater::get_next_weekday_string(5);
    date.erase(std::remove(date.begin(), date.end(), '-'), date.end());
    date = date.substr(date.find("20"));
    return ticker.replace(ticker.find("next_week"), sizeof("next_week"), date);
  } else if (ticker.find("this_quarter") != ticker.npos) {
    date = Dater::get_quarter_lastday_string(5);
    date.erase(std::remove(date.begin(), date.end(), '-'), date.end());
    date = date.substr(date.find("20"));
    return ticker.replace(ticker.find("this_quarter"), sizeof("this_quarter"), date);
  } else if (ticker.find("next_quarter") != ticker.npos) {
    date = Dater::get_next_quarter_lastday_string(5);
    date.erase(std::remove(date.begin(), date.end(), '-'), date.end());
    date = date.substr(date.find("20"));
    return ticker.replace(ticker.find("next_quarter"), sizeof("next_quarter"), date);
  }
  return ticker;
}

int main() {
  cout << TransCoin("'OKEX'/ETH-USDT-this_week");
}
