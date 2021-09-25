/*************************************************************************
	> File Name: convolution_forward.c
	> Author:
	> Mail:
	> Created Time:
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "function.h"

#include "args.h"
#include "util.h"

//extern void SLAVE_FUN(sw_rcr)();
//extern void SLAVE_FUN(sw_rrr)();
//extern void SLAVE_FUN(sw_trans_head)();
//extern void SLAVE_FUN(sw_trans_head_back)();
//extern void SLAVE_FUN(swnorm)();
//extern void SLAVE_FUN(sw_slave)();
//extern void SLAVE_FUN(par_multihead_attn)(); //declare slave parallel method

typedef struct  data{
          int params[6];
          float *arr[3];
 }data;

typedef struct  param{
          int len;
	  double norm;
          float *arr;
 }param;

struct data loop1;
struct data loop2;
struct data loop4;
struct data loop3;
struct param loop_slave;
struct data loop_norm;

void _local_gemm_rcr(const float* A, const int LDA, const float* B, const int LDB, float* C, const int LDC, int M, int N, int K)
{
    int i,j,k;
//    printf("TTTTTTTTTTT");
#pragma acc parallel loop private(i,j,k,M,N,K,LDA,LDB,LDC) copyin(A,B) copy(C) 
    for( i = 0;i < M; i ++)
        for( j = 0; j < N; j ++)
            for( k = 0; k < K; k ++){
                C[i*LDC+j] += A[i*LDA+k]*B[k+j*LDB];
            }
}
static void _local_gemm_rrr(const float* A, const int LDA, const float* B, const int LDB, float* C, const int LDC, int M, int N, int K)
{
	int i,j,k;
#pragma acc parallel loop private(i,j,k,M,N,K,LDA,LDB,LDC) copyin(A,B)   copy(C) 
    for( i = 0;i < M; i ++)
        for( j = 0; j < N; j ++)
            for( k = 0; k < K; k ++)
                C[i*LDC+j] += A[i*LDA+k]*B[k*LDB+j];
}



static void _local_trans_head(float* src, float* dst, int B, int S, int D, int N)
{
    int pD = D/N;
#define SRC(b, s, d) src[b*S*D+s*D+d]
#define DST(b, n, s, pd) dst[b*N*S*pD + n*S*pD + s*pD + pd]
    for(int b = 0; b < B; b ++)
        for(int n = 0; n < N; n ++)
            for(int s = 0; s < S; s ++)
                for(int pd = 0; pd < pD; pd ++)
                    DST(b,n,s,pd) = SRC(b,s,n*pD+pd);
}

static void _local_trans_head_back(float* src, float* dst, int B, int S, int D, int N)
{
    int pD = D/N;
#define D3(b, s, d) dst[b*S*D+s*D+d]
#define D4(b, n, s, pd) src[b*N*S*pD + n*S*pD + s*pD + pd]
    for(int b = 0; b < B; b ++)
        for(int n = 0; n < N; n ++)
            for(int s = 0; s < S; s ++)
                for(int pd = 0; pd < pD; pd ++)
					D3(b,s,n*pD+pd) = D4(b,n,s,pd);
}


static void _local_norm(float* buf, int len)
{
	double sum = 0.0f;
	int i;
	for( i = 0;i < len; i ++)
		sum += buf[i];

	for( i = 0;i < len;i ++)
		buf[i] /= sum;
}

static void _print_buf(float* buf, int len, const char* name)
{
	printf("====%s\n", name);
	for(int i = 0; i < 10 && i < len; i ++)
		printf("%f ", buf[i]);
	printf("\n");
}



/*
void Norm( float* buf, int len) {

       loop_norm.params[0] = len;


       loop_norm.arr[0] = buf;

       //printf("sizeofA=====%d\n",sizeof(*A));
       //printf("sizeofint=====%d\n",sizeof(int));
       athread_spawn(swnorm,&loop_norm);
       // printf("test");
       athread_join();

}*/


int multihead_attention(Args_t arg)
{
 //   printf("TTTTTTTTTTTTTT");
    const int B = arg->B;
    const int S = arg->S;
    const int D = arg->D;
    const int N = arg->N;
    const float* x = arg->x;
    const float* w = arg->w;
    float* Q = arg->Q;
    float* K = arg->K;
    float* V = arg->V;
    float* QK = arg->QK;
    float* y = arg->y;
    const int PD = D/N;
   
    memset(Q, 0, sizeof(float)*B*S*D);
    memset(K, 0, sizeof(float)*B*S*D);
    memset(V, 0, sizeof(float)*B*S*D);
    memset(QK, 0, sizeof(float)*B*N*S*S);
    memset(y, 0, sizeof(float)*B*S*D);
    float* QN = (float*)aligned_malloc(sizeof(float)*B*N*S*PD, 128);
    float* KN = (float*)aligned_malloc(sizeof(float)*B*N*S*PD, 128);
    float* VN = (float*)aligned_malloc(sizeof(float)*B*N*S*PD, 128);
    //calculate Q, K, V
    for(int b = 0; b < B; b ++)
    {
          //rcr(x+b*S*D, D, w, D, Q+b*S*D, D, S, D, D);
          //rcr(x+b*S*D, D, w+D*D, D, K+b*S*D, D, S, D, D);
          //rcr(x+b*S*D, D, w+2*D*D, D, V+b*S*D, D, S, D, D);
          //printf("test");
        _local_gemm_rcr(x+b*S*D, D, w, D, Q+b*S*D, D, S, D, D);
        _local_gemm_rcr(x+b*S*D, D, w+D*D, D, K+b*S*D, D, S, D, D);
        _local_gemm_rcr(x+b*S*D, D, w+2*D*D, D, V+b*S*D, D, S, D, D);
    }
    //trans_head(Q, QN, B, S, D, N);
    //trans_head(K, KN, B, S, D, N);
    //trans_head(V, VN, B, S, D, N);
    _local_trans_head(Q, QN, B, S, D, N);
    _local_trans_head(K, KN, B, S, D, N);
    _local_trans_head(V, VN, B, S, D, N);
#define NI(b,n,s,pd) ((((b)*N+n)*S+s)*PD+pd)
#define QKI(b,n,sh,sl) ((((b)*N+n)*S+sh)*S+sl)
	// QK = Q*KT
	for(int b = 0; b < B; b ++)
		for(int n = 0; n < N; n ++){
            //rcr(QN+NI(b,n,0,0), PD, KN+NI(b,n,0,0), PD, QK+QKI(b,n,0,0), S, S, S, PD);
            _local_gemm_rcr(QN+NI(b,n,0,0), PD, KN+NI(b,n,0,0), PD, QK+QKI(b,n,0,0), S, S, S, PD);
        }
			

	double norm = sqrt(PD*1.0);
//	for(int i = 0; i < B*N*S*S; i ++)
		//QK[i] /= norm;  modify renjian
/*	loop_slave.len = B*N*S*S;
	loop_slave.norm = norm;
	loop_slave.arr = QK;
	printf("loop_slave = %p \n",&loop_slave);
	athread_spawn(sw_slave,&loop_slave);
	athread_join();
*/
	for(int b = 0; b < B; b ++)
		for(int n = 0; n < N; n ++)
			for(int s = 0; s < S; s ++){
                //Norm(QK+QKI(b,n,s,0), S);
               			 _local_norm(QK+QKI(b,n,s,0), S);
			}


	// reuse Q
	memset(QN, 0, sizeof(float)*B*S*D);
	for(int b = 0; b < B; b ++)
		for(int n = 0; n < N; n ++){
            //rrr(QK+QKI(b,n,0,0), S, VN+NI(b,n,0,0), PD, QN+NI(b,n,0,0), PD, S, PD, S);
            _local_gemm_rrr(QK+QKI(b,n,0,0), S, VN+NI(b,n,0,0), PD, QN+NI(b,n,0,0), PD, S, PD, S);
        }
			
    //trans back
	_local_trans_head_back(QN, y, B, S, D, N);
	//trans_head_back(QN, y, B, S, D, N);
	
	aligned_free(QN);
	aligned_free(KN);
	aligned_free(VN);
    return 0;
}

