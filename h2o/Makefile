mymcp23017:h2o.o mymcp23017.o
	gcc -Wall -o h2o h2o.o mymcp23017.o -lwiringPi -std=gnu99
h2o.o: h2o.c mymcp23017.h
	gcc -Wall -c h2o.c -lwiringPi -std=gnu99
clean:
	rm h2o.o h2o
