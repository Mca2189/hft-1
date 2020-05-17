WAF = backend/tools/waf

all:
	$(WAF) configure all $(PARAMS)

easy_strat:
	$(WAF) configure easy_strat $(PARAMS)

pricer:
	$(WAF) configure pricer $(PARAMS)

proxy:
	$(WAF) configure proxy $(PARAMS)

mid_data:
	$(WAF) configure mid_data $(PARAMS)

ctpdata:
	$(WAF) configure ctpdata $(PARAMS)

ctporder:
	$(WAF) configure ctporder $(PARAMS)

manual_ctp:
	$(WAF) configure manual_ctp $(PARAMS)

getins:
	$(WAF) configure getins $(PARAMS)

simplemaker:
	$(WAF) configure simplemaker $(PARAMS)

simplearb:
	$(WAF) configure simplearb $(PARAMS)

pairtrading:
	$(WAF) configure pairtrading $(PARAMS)

demostrat:
	$(WAF) configure demostrat $(PARAMS)

simdata:
	$(WAF) configure simdata $(PARAMS)

backtest:
	$(WAF) configure backtest $(PARAMS)

backtestpr:
	$(WAF) configure backtestpr $(PARAMS)

order_matcher:
	$(WAF) configure order_matcher $(PARAMS)

teststrat:
	$(WAF) configure teststrat $(PARAMS)

clean:
	rm -rf build
