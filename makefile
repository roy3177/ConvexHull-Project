.PHONY: all clean part1 part2 part3 part4 part5 part6 part7 part8 part9 part10

all: part1 part2 part3 part4 part5 part6 part7 part8 part9 part10

part1:
	$(MAKE) -C part1-Area

part2:
	$(MAKE) -C part2-Profiling

part3:
	$(MAKE) -C part3-Interactive

part4:
	$(MAKE) -C part4-users

part5:
	$(MAKE) -C part5-reactor

part6:
	$(MAKE) -C part6-reactor-usage

part7:
	$(MAKE) -C part7-threads

part8:
	$(MAKE) -C part8-proactor

part9:
	$(MAKE) -C part9-proactor_server

part10:
	$(MAKE) -C part10-producer_consumer

clean:
	$(MAKE) -C part1-Area clean
	$(MAKE) -C part2-Profiling clean
	$(MAKE) -C part3-Interactive clean
	$(MAKE) -C part4-users clean
	$(MAKE) -C part5-reactor clean
	$(MAKE) -C part6-reactor-usage clean
	$(MAKE) -C part7-threads clean
	$(MAKE) -C part8-proactor clean
	$(MAKE) -C part9-proactor_server clean
	$(MAKE) -C part10-producer_consumer clean
