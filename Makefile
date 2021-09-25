
CC = sw5cc
HCFLAGS = -std=gnu99 -O3 -msimd -host  -OPT:IEEE_arith=1 -ftz
#SCFLAGS = -std=gnu99 -O3 -msimd -slave -OPT:IEEE_arith=1 -ftz 
LDFLAGS = -lm

HEADER_FILES = function.h args.h util.h

LIBS  = -L/home/export/online3/swmore/opensource/swlu/lib/ -lswlu

BUILD = ./obj

main : $(BUILD)/main.o $(BUILD)/function.o $(BUILD)/master.o 
	$(CC) $(HCFLAGS) -hybrid   -o $@ $^ $(LIBS)  $(LDFLAGS) -lm_slave

$(BUILD)/%.o : %.c $(HEADER_FILES)
	$(CC) $(HCFLAGS) -o $@ -c $< 

$(BUILD)/slave.o : slave.c args.h
	$(CC) $(SCFLAGS) -o $@ -c $<

run : 
	bsub -I -m 1  -b -q q_sw_expr -share_size 6144 -host_stack 3072 -n 1 -cgsp 64  ./main

clean :
	rm -f $(BUILD)/*.o ./main
