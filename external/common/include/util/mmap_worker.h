#ifndef MMAP_WORKER_H_
#define MMAP_WORKER_H_

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

template <typename T>
class MmapWorker {
 public:
  MmapWorker();
  ~MmapWorker();
  

 private:
  
};

#endif  // MMAP_WORKER_H_
