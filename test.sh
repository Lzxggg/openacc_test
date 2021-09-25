sw5cc -std=gnu99 -O1 -msimd -host  -OPT:IEEE_arith=1 -ftz -o obj/main.o -c main.c 
sw5cc -std=gnu99 -O1 -msimd -host  -OPT:IEEE_arith=1 -ftz -o obj/function.o -c function.c 
swacc -keep -OPT:IEEE_arith=1 -ftz  -std=gnu99 -O1  -o obj/master.o -c master.c 
swacc -std=gnu99 -O1 -msimd -host  -OPT:IEEE_arith=1 -ftz -hybrid   -o main obj/main.o obj/function.o obj/master.o -L/home/export/online3/swmore/opensource/swlu/lib/ -lswlu  -lm -lm_slave
