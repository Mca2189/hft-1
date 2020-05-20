#!/bin/sh
export LD_LIBRARY_PATH=/usr/local/lib

pkill -u root ctpdata
pkill -u root ctporder
pkill -u root easy_strat
pkill -u root strat
pkill -u root data_proxy
pkill -u root order_proxy
pkill -u root mid_data
pkill -u root simplearb
pkill -u root functional_test
pkill -u root getins
pkill -u root functional_test
