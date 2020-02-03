.PHONY: runcs runtest clean runsc

compsc: System_Call
	@ gcc -c System_Call.c

runsc: compsc
	@ ./System_Call


compcs: context_switch
	@ gcc -c context_switch.c

runcs: compcs
	@ ./context_switch


clean:
	@ -rm context_switch system_call *.txt *.o *.out