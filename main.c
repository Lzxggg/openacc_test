/*************************************************************************
	> File Name: main.c
	> Author:
	> Mail:
	> Created Time:
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <athread.h>
#include <time.h>
#include <sys/time.h>

#include "args.h"
#include "function.h"
#include "util.h"
#include "/home/export/online3/swmore/opensource/swlu/include/swlu.h"
#define REPEAT_N 5
const char test_data[3][16] = {"./data/t_0", "./data/t_1", "./data/t_2"};

void multihead_attention();

void run_and_check()
{

    for(int i = 0;i < TEST_DATA_NUM; i ++)
    {
        Args_t arg = create_empty_args();
        float* ori_y;
        read_data(arg, test_data[i], (void**)&ori_y);
        TIME_T start, end;
        MARK_TIME(start);
        // run your program
        for(int j = 0; j < REPEAT_N; j ++)
            multihead_attention(arg);
        MARK_TIME(end);
        LOG("average time : %.3f ms", DIFF_TIME(start, end)/REPEAT_N);
        // compare your result with original one
        if(validate_results(arg->y, ori_y, arg->B*arg->S*arg->D) == VALIDATE_PASSED)
            LOG("Passed");

        aligned_free(ori_y);
        destroy_args(arg);
    }
}

int main(int argc, char* argv[])
{
   // swlu_prof_init();
    //athread_init(); // initialize resource
   // swlu_prof_init();
    run_and_check();
    //printf("TTTTTTTTTTTTT");
    athread_halt();
    return 0;
}


