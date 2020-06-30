aux_source_directory(ctporder CTPORDER)
SET(CMAKE_C_COMPILER "/usr/bin/gcc")
SET(CMAKE_CXX_COMPILER "/usr/bin/g++")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11 -Wall -ggdb -fPIC -pg")

include_directories(../external/common/include)
include_directories(../external/ctp/include)
include_directories(../external/zeromq/include)
include_directories(.)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/common/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/ctp/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/zeromq/lib)

macro(compile arg1)
  aux_source_directory(${arg1} ${arg1}_src)
  add_executable(${arg1} ${${arg1}_src})
  target_link_libraries(${arg1} pthread nick zmq thosttraderapi thostmduserapi config++ z)
endmacro(compile)

compile(ctpdata)
compile(ctporder)
compile(data_proxy)
compile(order_proxy)
compile(simplearb)
compile(simplearb2)
compile(backtest)
