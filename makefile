.PHONY: runcs runtest clean runsc

compcs: System_Call
	@ gcc -c context_switch.c

compsc: System_Call
	@ gcc -c System_Call.c

runcs: compcs
	@ ./context_switch

runsc: compsc
	@ ./System_Call

clean:
	@ -rm *.o context_switch System_Call test

comptest: test
	@ gcc -c test.c

runtest: comptest
	@ ./test