![pnl](https://github.com/nickhuangxinyu/hft/blob/master/pnl.png "pnl running on china futures market")
# High Frequency Trading Solution #
* running on china futures market and cryptocurrency market

### What is this repository for? ###
* automatic algorithm trading system

### How do I get set up? ###
* System requirement:
  based on centos 6.5 or 7 (other system have not been tested)
* IDE recommendation:
  I developed it by vim (using some cool plugin), i also set the neccessary config file for QT, so this can be opened on QT directly.
* Complie Tools
  based on g++, using the complie tools waf, the binary file is included in the repository, path hft/backend/bin/waf, for waf, you can google it
* pre-installed software:
  this project used zeromq to do ipc, version 4.1.2
* complie command:
  in the path of yourhomepath/hft, run "make", you will get binary file in build/bin
  
### Have a quick view ###
* How to run it
  As a start, you can run ctpdata (after 'make', ./build/bin/ctpdata), if network is good, and time is in the trading session(9:00-11:30 13:30-3:00), you can see marketdata come out in your screen.
* ui
you can find the frontend ui: https://github.com/nickhuangxinyu/Gallery/blob/master/TradingMonitor.rar
you can open it in trading time, it's a monitor of spread.


### Exsited strategy:
1. Hedged market-making strategy
2. statistical arbitrage, pair trading
3. Moving-Average golden death crossing strategy
4. turtle strategy (on-goinig by ShengRui zhao)


### Open Source:
Currently, the repository opened the strategy code, exchange api code, backtest code, complementary tools code.

### Author:
* XinYu Huang

any questions or suggestions are welcome, please contract me with:huangxy17@fudan.edu.cn, i will list your name here to thanks for your contribution.

### trading system latency:
![pnl](https://github.com/nickhuangxinyu/hft/blob/master/example1.png "latency")

### Thanks list :
* ShengRui zhao: most active products getting tools on futures market, fill the detail config for all contracts.
* Jian Sun: professor of school of Economics, Fudan university. Fund-supporter and strategy idea consultant.
* Wei Sun: systems consultant.

### Exsiting problem:
1. ui: load history
