.PHONY: runcs runtest clean runsc

compsc: system_call
	@ gcc -c system_call.c

runsc: compsc
	@ ./system_call


compcs: context_switch
	@ gcc -c context_switch.c

runcs: compcs
	@ ./context_switch


clean:
	@ -rm *.o context_switch system_call *.txt