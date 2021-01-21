#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <deque>

#include "./strategy.h"
void PrintDeque(const std::deque<double> & d) {
  std::cout << "start print deque: ";
  for (auto i : d) {
    std::cout << i << " ";
  }
  std::cout << "end print" << std::endl;
}

Strategy::Strategy(const libconfig::Setting & param_setting, std::unordered_map<std::string, std::vector<BaseStrategy*> >*ticker_strat_map, ZmqSender<MarketSnapshot>* uisender, ZmqSender<Order>* ordersender, TimeController* tc, ContractWorker* cw, HistoryWorker* hw, const std::string & date, StrategyMode::Enum mode, std::ofstream* exchange_file)
  : date(date),
    last_valid_mid(0.0),
    max_close_try(10),
    no_close_today(false),
    max_round(10000),
    close_round(0),
    sample_head(0),
    sample_tail(0),
    exchange_file(exchange_file) {
  m_tc = tc;
  m_cw = cw;
  m_hw = hw;
  SetStrategyMode(mode, exchange_file);
  if (FillStratConfig(param_setting)) {
    RunningSetup(ticker_strat_map, uisender, ordersender);
  }
}

Strategy::~Strategy() {
}

void Strategy::RunningSetup(std::unordered_map<std::string, std::vector<BaseStrategy*> >*ticker_strat_map, ZmqSender<MarketSnapshot>* uisender, ZmqSender<Order>* ordersender) {
  m_ui_sender = uisender;
  m_order_sender = ordersender;
  (*ticker_strat_map)[main_ticker].emplace_back(this);
  (*ticker_strat_map)[hedge_ticker].emplace_back(this);
  (*ticker_strat_map)["positionend"].emplace_back(this);
  MarketSnapshot shot;
  m_shot_map[main_ticker] = shot;
  m_shot_map[hedge_ticker] = shot;
  m_avgcost_map[main_ticker] = 0.0;
  m_avgcost_map[hedge_ticker] = 0.0;
}

bool Strategy::FillStratConfig(const libconfig::Setting& param_setting) {
  try {
    std::string unique_name = param_setting["unique_name"];
    const libconfig::Setting & contract_setting = m_cw->Lookup(unique_name);
    m_strat_name = unique_name;
    auto v = m_hw->GetAllTicker(unique_name);
    if (v.size() < 2) {
      printf("no enough ticker for %s\n", unique_name.c_str());
      // PrintVector(v);
      return false;
    }
    main_ticker = v[1].first;
    hedge_ticker = v[0].first;
    max_pos = param_setting["max_position"];
    train_samples = param_setting["train_samples"];
    double m_r = param_setting["min_range"];
    double m_p = param_setting["min_profit"];
    min_price_move = contract_setting["min_price_move"];
    min_profit = m_p * min_price_move;
    min_range = m_r * min_price_move;
    double add_margin = param_setting["add_margin"];
    increment = add_margin*min_price_move;
    double spread_threshold_int = param_setting["spread_threshold"];
    spread_threshold = spread_threshold_int*min_price_move;
    m_max_holding_sec = param_setting["max_holding_sec"];
    range_width = param_setting["range_width"];
    std::string con = GetCon(main_ticker);
    cancel_limit = contract_setting["cancel_limit"];
    max_round = param_setting["max_round"];
    split_num = param_setting["split_num"];
    if (param_setting.exists("no_close_today")) {
      no_close_today = param_setting["no_close_today"];
    }
  } catch(const libconfig::SettingNotFoundException &nfex) {
    printf("Setting '%s' is missing", nfex.getPath());
    exit(1);
  } catch(const libconfig::SettingTypeException &tex) {
    printf("Setting '%s' has the wrong type", tex.getPath());
    exit(1);
  } catch (const std::exception& ex) {
    printf("EXCEPTION: %s\n", ex.what());
    exit(1);
  }
  up_diff = 0.0;
  down_diff = 0.0;
  return true;
}

void Strategy::Stop() {
  CancelAll(main_ticker);
  m_ss = StrategyStatus::Stopped;
}

inline bool Strategy::IsAlign() {
  if (m_shot_map[main_ticker].time.tv_sec == m_shot_map[hedge_ticker].time.tv_sec && abs(m_shot_map[main_ticker].time.tv_usec-m_shot_map[hedge_ticker].time.tv_usec) < 100000) {
    return true;
  }
  return false;
}

void Strategy::DoOperationAfterCancelled(Order* o) {
  printf("ticker %s cancel num %d!\n", o->ticker, m_cancel_map[o->ticker]);
  if (m_cancel_map[o->ticker] > cancel_limit) {
    printf("ticker %s hit cancel limit!\n", o->ticker);
    Stop();
  }
}

double Strategy::OrderPrice(const std::string & ticker, OrderSide::Enum side, bool control_price) {
  if (m_mode == StrategyMode::NextTest) {
    if (ticker == hedge_ticker) {
      return (side == OrderSide::Buy)?m_next_shot_map[ticker].asks[0]:m_next_shot_map[ticker].bids[0];
    } else if (ticker == main_ticker) {
      return (side == OrderSide::Buy)?m_shot_map[ticker].asks[0]:m_shot_map[ticker].bids[0];
    } else {
      printf("error ticker %s\n", ticker.c_str());
      return -1.0;
    }
  } else {
    if (ticker == hedge_ticker) {
      return (side == OrderSide::Buy)?m_shot_map[hedge_ticker].asks[0]:m_shot_map[hedge_ticker].bids[0];
    } else if (ticker == main_ticker) {
      return (side == OrderSide::Buy)?m_shot_map[main_ticker].asks[0]:m_shot_map[main_ticker].bids[0];
    } else {
      printf("error ticker %s\n", ticker.c_str());
      return -1.0;
    }
  }
}

std::tuple<double, double> Strategy::CalMeanStd(const std::vector<double> & v, int head, int num) {
  std::vector<double> cal_v(v.begin() + head, v.begin() + head + num);
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

void Strategy::CalParams() {
  // int num_sample = sample_tail - sample_head;
  if (sample_tail < train_samples) {
    printf("[%s %s]no enough mid data! tail is %d\n", main_ticker.c_str(), hedge_ticker.c_str(), sample_tail);
    exit(1);
  }
  auto r = CalMeanStd(mids, sample_tail - train_samples, train_samples);
  double avg = std::get<0>(r);
  double std = std::get<1>(r);
  // FeePoint main_point = m_cw->CalFeePoint(main_ticker, GetMid(main_ticker), 1, GetMid(main_ticker), 1, no_close_today);
  // FeePoint hedge_point = m_cw->CalFeePoint(hedge_ticker, GetMid(hedge_ticker), 1, GetMid(hedge_ticker), 1, no_close_today);
  // double round_fee_cost = main_point.open_fee_point + main_point.close_fee_point + hedge_point.open_fee_point + hedge_point.close_fee_point;
  double margin = std::max(range_width * std, min_profit);  //  + round_fee_cost;
  up_diff = avg + margin;
  down_diff = avg - margin;
  mean = avg;
  sample_head = sample_tail;
  printf("[updateprams]up=%lf down=%lf mean=%lf\n", up_diff, down_diff, mean);
}

void Strategy::ForceFlat() {
}

double Strategy::GetPairMid() {
  return GetMid(main_ticker) - GetMid(hedge_ticker);
}

void Strategy::Flatting() {
}

bool Strategy::OpenLogic() {
  int pos = m_position_map[main_ticker];

  if (pos <= 0 && m_shot_map[main_ticker].bids[0] + 2 * min_price_move - m_shot_map[hedge_ticker].bids[0] + 2*min_price_move < down_diff) {  // long
    int time_sec = m_shot_map[main_ticker].time.tv_sec %(24*3600);
    int size = (time_sec >= 14*3600 + 30*60) ? 1-pos : -pos;
    //  Order* o =
    PlaceOrder(main_ticker, m_shot_map[main_ticker].bids[0] + 2 * min_price_move, size, no_close_today, "close");
    // o->Show(stdout);
    return true;
  }

  if (pos >= 0 && m_shot_map[main_ticker].asks[0] - 2 * min_price_move - m_shot_map[hedge_ticker].asks[0] - 2*min_price_move > up_diff) {  // short
    int time_sec = m_shot_map[main_ticker].time.tv_sec %(24*3600);
    int size = (time_sec >= 14*3600 + 30*60) ? -1-pos : -pos;
    //  Order* o =
    PlaceOrder(main_ticker, m_shot_map[main_ticker].asks[0] - 2 * min_price_move, size, no_close_today, "close");
    // o->Show(stdout);
    return true;
  }
  return false;
}

void Strategy::Run() {
  if (IsAlign() && close_round < max_round) {
    OpenLogic();
  } else {
  }
}

void Strategy::DoOperationAfterUpdateData(const MarketSnapshot& shot) {
  mid_map[shot.ticker] = (shot.bids[0]+shot.asks[0]) / 2;  // mid_map saved the newest mid, no matter it is aligned or not
  current_spread = m_shot_map[main_ticker].asks[0] - m_shot_map[main_ticker].bids[0] + m_shot_map[hedge_ticker].asks[0] - m_shot_map[hedge_ticker].bids[0];
  if (IsAlign()) {
    double mid = GetPairMid();
    mids.emplace_back(mid);  // map_vector saved the aligned mid, all the elements here are safe to trade
    int num_sample = ++sample_tail - sample_head;
    if (num_sample > train_samples && num_sample % (train_samples) == 1) {
      CalParams();
    }
    if (sample_tail % 100 == 99) {
      printf("%ld [%s, %s]mid_diff is %lf, head=%d, tail=%d\n", shot.time.tv_sec, main_ticker.c_str(), hedge_ticker.c_str(), mid_map[main_ticker]-mid_map[hedge_ticker], sample_head, sample_tail);
    }
  }
}

void Strategy::HandleCommand(const Command& shot) {
  printf("received command!\n");
  shot.Show(stdout);
  if (abs(shot.vdouble[0]) > MIN_DOUBLE_DIFF) {
    up_diff = shot.vdouble[0];
    return;
  }
  if (abs(shot.vdouble[1]) > MIN_DOUBLE_DIFF) {
    down_diff = shot.vdouble[1];
    return;
  }
  if (abs(shot.vdouble[2]) > MIN_DOUBLE_DIFF) {
    return;
  }
  if (abs(shot.vdouble[3]) > MIN_DOUBLE_DIFF) {
    return;
  }
}

void Strategy::Train() {
}

void Strategy::Pause() {
}

void Strategy::Resume() {
  sample_head = sample_tail;
}

bool Strategy::Ready() {
  int num_sample = sample_tail - sample_head;
  if (m_position_ready && m_shot_map[main_ticker].IsGood() && m_shot_map[hedge_ticker].IsGood() && num_sample >= train_samples) {
    if (num_sample == train_samples) {
      // first cal params
      CalParams();
    }
    return true;
  }
  if (!m_position_ready) {
    printf("waiting position query finish!\n");
  }
  return false;
}

void Strategy::ModerateOrders(const std::string & ticker) {
  // just make sure the order filled
  if (m_mode == StrategyMode::Real) {
    for (auto m : m_order_map) {
      Order* o = m.second;
      if (o->Valid()) {
        CancelOrder(o);
      }
    }
  }
}

void Strategy::Start() {
  if (!is_started) {
    is_started = true;
  }
  Run();
}

void Strategy::DoOperationAfterUpdatePos(Order* o, const ExchangeInfo& info) {
}

void Strategy::HandleTestOrder(Order* o) {
  if (m_mode == StrategyMode::Real) {
    return;
  }
  ExchangeInfo info;
  info.shot_time = o->shot_time;
  info.show_time = o->shot_time;
  info.type = InfoType::Filled;
  info.trade_size = o->size;
  info.trade_price = o->price;
  info.side = o->side;
  snprintf(info.order_ref, sizeof(info.order_ref), "%s", o->order_ref);
  snprintf(info.ticker, sizeof(info.ticker), "%s", o->ticker);
  snprintf(info.reason, sizeof(info.reason), "%s", "test");
  // m_position_map[o->ticker] += o->side == OrderSide::Buy ? o->size : -o->size;
  exchange_file->write(reinterpret_cast<char*>(&info), sizeof(info));
  exchange_file->flush();
  // info.Show(stdout);
  UpdatePos(o, info);
  // m_order_map.clear();
  PrintMap(m_position_map);
  PrintMap(m_avgcost_map);
  DoOperationAfterFilled(o, info);
}

void Strategy::DoOperationAfterFilled(Order* o, const ExchangeInfo& info) {
  if (strcmp(o->ticker, main_ticker.c_str()) == 0) {
    std::string a = o->tbd;
    if (a.find("close") != string::npos) {
      close_round++;
      CalParams();
    } else {
    }
    // OrderSide::Enum hedge_side = (o->side == OrderSide::Buy) ? OrderSide::Sell : OrderSide::Buy;
    // Order* order = NewOrder(hedge_ticker, hedge_side, info.trade_size, false, false, o->tbd, no_close_today);
    // HandleTestOrder(order);
    // order->Show(stdout);
  } else if (strcmp(o->ticker, hedge_ticker.c_str()) == 0) {
  } else {
    printf("o->ticker=%s, main:%s, hedge:%s\n", o->ticker, main_ticker.c_str(), hedge_ticker.c_str());
    SimpleHandle(322);
  }
}

bool Strategy::Spread_Good() {
  return (current_spread > spread_threshold) ? false : true;
}
