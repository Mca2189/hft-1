#ifndef DATA_HANDLER_HPP_
#define DATA_HANDLER_HPP_
#include <iostream>
#include <unordered_map>
#include <string>
#include <zlib.h>
#include "struct/market_snapshot.h"
#include "util/common_tools.h"
#include "util/time_controller.h"
#include "define.h"

template <typename T>
class DataHandler {
 public:
  DataHandler(bool get_next = true)
    : m_getnext(get_next) {
  }
  ~DataHandler() {

  }
  void LoadData(const std::string& file_path) {
    std::string file_mode = Split(file_path, ".").back();
    if (file_mode == "gz") {
      gzFile gzfp = gzopen(file_path.c_str(), "rb");
      if (!gzfp) {
        printf("gzfile open failed!%s\n", file_path.c_str());
        return;
      }
      printf("handling %s\n", file_path.c_str());
      unsigned char buf[SIZE_OF_SNAPSHOT];
      T* shot;
      if (m_getnext) {
        tc.StartTimer();
        while (gzread(gzfp, buf, sizeof(*shot)) > 0) {
          shot = reinterpret_cast<T*>(buf);
          if (all.find(shot->ticker) == all.end()) {
            std::vector<T> temp_v;
            all[shot->ticker] = temp_v;
          }
          all[shot->ticker].push_back(*shot);
        }
        tc.EndTimer("LoadAllShot");
        for (auto i : all) {
          index_count[i.first] = 0;
        }
        gzclose(gzfp);
      }
      gzfp = gzopen(file_path.c_str(), "rb");
      while (gzread(gzfp, buf, sizeof(*shot)) > 0) {
        shot = reinterpret_cast<T*>(buf);
        T* next_shot = GetNext(shot);
        HandleShot(shot, next_shot);
      }
      gzclose(gzfp);
    } else if (file_mode == "dat") {
      std::ifstream raw_file;
      raw_file.open(file_path.c_str(), ios::in|ios::binary);
      if (!raw_file) {
        printf("%s is not existed!\n", file_path.c_str());
        return;
      }
      printf("handling %s\n", file_path.c_str());
      T shot;
      if (m_getnext) {
        tc.StartTimer();
        while (raw_file.read(reinterpret_cast<char *>(&shot), sizeof(shot))) {
          if (all.find(shot.ticker) == all.end()) {
            std::vector<T> temp_v;
            all[shot.ticker] = temp_v;
          }
          all[shot.ticker].push_back(shot);
        }
        tc.EndTimer("LoadAllShot");
        for (auto i : all) {
          index_count[i.first] = 0;
        }
        raw_file.close();
      }
      raw_file.open(file_path.c_str(), ios::in|ios::binary);
      while (raw_file.read(reinterpret_cast<char *>(&shot), sizeof(shot))) {
        if (shot.time.tv_usec != all[shot.ticker][index_count[shot.ticker]].time.tv_usec) {
          printf("not correct!\n");
          exit(1);
        }
        T* next_shot = GetNext(&shot);
        HandleShot(&shot, next_shot);
      }
      raw_file.close();
    } else {
      printf("unknown mode %s\n", file_path.c_str());
    }
  }

  inline T* GetNext(T* shot) {
    T* next_shot = nullptr;
    if (m_getnext) {
      if (index_count[shot->ticker] == all[shot->ticker].size() -1) {
        next_shot = &all[shot->ticker][index_count[shot->ticker]];
      } else {
        next_shot = &all[shot->ticker][++index_count[shot->ticker]];
      }
    } else {
      next_shot = &temp_shot;
    }
    return next_shot;
  }

  virtual void HandleShot(T* this_shot, T* next_shot) = 0;
 private:
  bool m_getnext;
  T temp_shot;
  std::unordered_map<std::string, T> last_map;
  std::unordered_map<std::string, std::vector<T> > all;
  std::unordered_map<std::string, size_t> index_count;
  TimeController tc;
};

#endif // DATA_HANDLER_HPP_
