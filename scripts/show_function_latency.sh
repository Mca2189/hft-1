#!/bin/bash
gprof  ./backtest gmon.out |gprof2dot -s --total=TOTALMETHOD|dot -Tpng -o example.png
