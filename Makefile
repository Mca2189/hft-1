WAF = backend/tools/waf-light

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

simplearb2:
	$(WAF) configure simplearb2 $(PARAMS)

coinarb:
	$(WAF) configure coinarb $(PARAMS)

pairtrading:
	$(WAF) configure pairtrading $(PARAMS)

demostrat:
	$(WAF) configure demostrat $(PARAMS)

simdata:
	$(WAF) configure simdata $(PARAMS)

backtest:
	$(WAF) configure backtest $(PARAMS)

backtest2:
	$(WAF) configure backtest2 $(PARAMS)

backtestpr:
	$(WAF) configure backtestpr $(PARAMS)

order_matcher:
	$(WAF) configure order_matcher $(PARAMS)

teststrat:
	$(WAF) configure teststrat $(PARAMS)

lib_simplemaker:
	$(WAF) configure lib_simplemaker $(PARAMS)

lib_simplearb:
	$(WAF) configure lib_simplearb $(PARAMS)

lib_simplearb2:
	$(WAF) configure lib_simplearb2 $(PARAMS)

lib_coinarb:
	$(WAF) configure lib_coinarb $(PARAMS)

lib_pairtrading:
	$(WAF) configure lib_pairtrading $(PARAMS)

lib_demostrat:
	$(WAF) configure lib_demostrat $(PARAMS)

clean:
	rm -rf build
