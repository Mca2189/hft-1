aux_source_directory(ctporder CTPORDER)

include_directories(../external/common/include)
include_directories(../external/ctp/include)
include_directories(../external/zeromq/include)
include_directories(.)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/common/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/ctp/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/zeromq/lib)

link_directories(/home/nick/hft/external/common/lib)
link_directories(/home/nick/hft/external/ctp/lib)
link_directories(/home/nick/hft/external/zeromq/lib)

add_executable(ctporder ${CTPORDER})
target_link_libraries(ctporder pthread nick zmq thosttraderapi)
