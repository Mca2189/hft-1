#ifndef SHM_SENDER_HPP_
#define SHM_SENDER_HPP_

#include <mutex>
#include "shm_worker.hpp"
#include "base_sender.hpp"

template <typename T>
class ShmSender: public ShmWorker, public BaseSender<T> {
 public:
  ShmSender(const std::string& key, int size = 100000, const std::string& file_name = "")
    : f(file_name.empty() ? nullptr : new std::ofstream(file_name.c_str(), ios::out | ios::binary)) {
    init <T> (key, size);
    sleep(1);
    mutex = reinterpret_cast<pthread_mutex_t*>(m_data + 3*sizeof(atomic_int));
  }

  ~ShmSender() {

  }

  void Send(const T& shot) override {
    // pthread_mutex_lock(mutex);
    auto tail = (atomic_int*)(m_data + 2*sizeof(atomic_int));
    memcpy(m_data+header_size+(tail->load()%m_size)*sizeof(T), &shot, sizeof(T));
    tail->fetch_add(1);
    // pthread_mutex_unlock(mutex);
    /*if (f.get()) {
      std::lock_guard<std::mutex> lck(mtx);  // for mutli-thread backtest file writting
      f.get()->write((char*)&shot, sizeof(T));
      f.get()->flush();
    }*/
  }
 private:
  pthread_mutex_t* mutex;
  std::mutex mtx;
  unique_ptr<std::ofstream> f;
};

#endif  // SHM_SENDER_HPP_
