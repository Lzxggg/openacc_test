#include <slave.h>
#include <math.h>
#include <simd.h>
#include <assert.h>
#include <string.h>
#include </home/export/online3/swmore/opensource/dma_macro/dma_macros.h>
#include "args.h"
#include "./inc/temp.h"

typedef struct  data{
          int params[6];
          float *arr[3];
 }data;

typedef struct  param{
          int len;
          double norm;
          float *arr;
 }param;


struct data loop2_s;
struct data loop3_s;
void slave_sw_rcr(struct data *arg){

    struct data loop1_s;
    int local_M,local_N,local_K;
    int LDA,LDB,LDC;
    float *A,*B,*C;
//    float *A_s,*B_s,*C_s,*tmp;
//    float temp[10000];
    int volatile  get_reply;
    int volatile  put_reply;
    const int myid = athread_get_id(-1);

    dma_init();
    get_reply = 0;
    pe_get(arg,&loop1_s,sizeof(loop1_s));
    dma_syn();
    get_reply = 0;

    LDA = loop1_s.params[0];
    LDB = loop1_s.params[1];
    LDC = loop1_s.params[2];
    local_M = loop1_s.params[3];
    local_N = loop1_s.params[4];
    local_K = loop1_s.params[5];

    A = loop1_s.arr[0];
    B = loop1_s.arr[1];
    C = loop1_s.arr[2];

//    A_s = temp;
//    B_s = A_s + local_K+10;
//    C_s = B_s + local_K+10;
//    tmp = C_s + local_N+10;
   float A_s[local_K+4],
	 B_s[local_K+4],
	 C_s[local_N+4],
	 tmp[4];

    floatv4 vc,vb,va;
    int i,j,k;
    for( i = 0;i < local_M; i ++){
        if(i % 64 == myid){
            get_reply = 0;
            pe_get(A+i*LDA , A_s , local_K*4);
            dma_syn();
            pe_get(C+i*LDC , C_s , local_N*4);
            dma_syn();
            for( j = 0; j < local_N; j ++)
            {
                    get_reply = 0;
                    pe_get(B+j*LDB , B_s , local_K*4);
                    dma_syn();
		    vc = 0;
		   if(local_K %4 != 0){
			A_s[local_K]=0;
			A_s[local_K+1]=0;
			A_s[local_K+2]=0;
			A_s[local_K+3]=0;
			B_s[local_K]=0;
			B_s[local_K+1]=0;
			B_s[local_K+2]=0;
			B_s[local_K+3]=0;
		   }

                   for(k = 0; k < local_K; k +=4)
                    { 
//		        *(C_s+j) += *(A_s+k) * *(B_s+k);
			simd_load(va,A_s+k);
			simd_load(vb,B_s+k);	
			vc += va*vb;
                    }
		    simd_store(vc,tmp);
                  //  for(i=0;i<4;i++){
			*(C_s+j) = *(tmp)+ *(tmp+1)+ *(tmp+2)+ *(tmp+3);
		   // }
                   
            }
            put_reply=0;
            pe_put(C+i*LDC , C_s ,(local_N)*4 );
            dma_syn();
        }
    }
}
/*
void slave_swnorm(struct data *arg){

    struct data loop_norm_s;
    int len;
    float *buf;
    float *buf_s;
    float temp[10000];
    int volatile  get_reply;
    int volatile  put_reply;
    const int myid = athread_get_id(-1);



    get_reply = 0;

    athread_get(PE_MODE,arg,&loop_norm_s,sizeof(loop_norm_s),(void *)&get_reply,0,0,0);

    while( get_reply != 1);

    get_reply = 0;

    len = loop_norm_s.params[0];

    buf = loop_norm_s.arr[0];


    buf_s = temp;

    double sum = 0.0f;

    get_reply = 0;

    athread_get(PE_MODE,arg,&loop_norm_s,sizeof(loop_norm_s),(void *)&get_reply,0,0,0);

    while( get_reply != 1);

    get_reply = 0;

    //printf("Enter slave successful");

    get_reply = 0;

    athread_get(PE_MODE,buf,buf_s,len*4,(void *)&get_reply,0,0,0);

    while( get_reply != 1);

    get_reply = 0;

    int i;
	for( i = 0;i < len; i ++){
            sum += *(buf+i);
	}

	for( i = 0;i < len;i ++){
            *(buf+i) /= sum;
	}

    put_reply=0;
    athread_put(0 ,buf_s ,buf ,(len)*4 ,(void *)&put_reply,0,0);
    while(put_reply != 1);
    put_reply=0;
}
*/
void slave_sw_trans_head(struct data *arg){
    int B,S,D,N,pD;
    float *src,*dst;
    float *src_s,*dst_s;
    float temp[10000];
    int volatile  get_reply;
    int volatile  put_reply;
    const int myid = athread_get_id(-1);
    dma_init();
    get_reply = 0;
    pe_get(arg,&loop2_s,sizeof(loop2_s));
    dma_syn(); 

    B = loop2_s.params[0];
    S = loop2_s.params[1];
    D = loop2_s.params[2];
    N = loop2_s.params[3];

    src = loop2_s.arr[0];
    dst = loop2_s.arr[1];
    pD = D/N;
    src_s = temp;
    dst_s = src_s + S;

   floatv4 v_src,v_dst;
    int b,n,s,pd;
    for( b = 0; b < B; b ++)
        for( n = 0; n < N; n ++)
          for( pd = 0; pd < pD; pd ++){
                if (pd % 64 == myid){
                   get_reply = 0;
                   athread_get(PE_MODE,src + b*S*D + n*pD + pd,src_s , S*4 , (void*)&get_reply,0,(D-1)*4,4);
                   while(get_reply != 2);
                   for( s = 0; s < S; s +=4){
                           simd_load(v_src,src_s);
                           v_dst = v_src;
                           simd_store(v_dst,dst_s);
                    }
                    put_reply=0;
                    athread_put(PE_MODE,dst_s,dst + b*N*S*pD + n*S*pD + pd,S*4,(void*)&put_reply,(pD-1)*4,4);
                    while(put_reply != 1);
                }
            }
}

void slave_sw_trans_head_back(struct data *arg){
    int B,S,D,N,pD;
    float *src,*dst;
    float *src_s,*dst_s;
    float temp[10000];
    int volatile  get_reply;
    int volatile  put_reply;
    struct data loop4_s;
    const int myid = athread_get_id(-1);
    dma_init();
    get_reply = 0;
    pe_get(arg,&loop4_s,sizeof(loop4_s));
    dma_syn();

    B = loop4_s.params[0];
    S = loop4_s.params[1];
    D = loop4_s.params[2];
    N = loop4_s.params[3];

    src = loop4_s.arr[0];
    dst = loop4_s.arr[1];
    pD = D/N;
    src_s = temp;
    dst_s = src_s + S;

   floatv4 v_src,v_dst;
    int b,n,s,pd;
    for( b = 0; b < B; b ++)
        for( n = 0; n < N; n ++)
          for( pd = 0; pd < pD; pd ++){
                if (pd % 64 == myid){
                   get_reply = 0;
		   athread_get(PE_MODE,src + b*N*S*pD + n*S*pD + pd,src_s , S*4 , (void*)&get_reply,0,(pD-1)*4,4);
                   while(get_reply != 2);
                   for( s = 0; s < S; s +=4){
                           simd_load(v_src,src_s);
                           v_dst = v_src;
                           simd_store(v_dst,dst_s);
                    }
                    put_reply=0;
                    athread_put(PE_MODE,dst_s,dst + b*S*D + n*pD + pd ,S*4,(void*)&put_reply,(D-1)*4,4);
                    while(put_reply != 1);
                }
            }
}


void slave_sw_rrr(struct data *arg){

    struct data loop3_s;
    int local_M,local_N,local_K;
    int LDA,LDB,LDC;
    float *A,*B,*C;
//    float *A_s,*B_s,*C_s,*tmp;
//    float temp[12000];
    int volatile  get_reply;
    int volatile  put_reply;
    const int myid = athread_get_id(-1);

    dma_init();
    get_reply = 0;
    pe_get(arg,&loop3_s,sizeof(loop3_s));
    dma_syn();

    LDA = loop3_s.params[0];
    LDB = loop3_s.params[1];
    LDC = loop3_s.params[2];
    local_M = loop3_s.params[3];
    local_N = loop3_s.params[4];
    local_K = loop3_s.params[5];

    A = loop3_s.arr[0];
    B = loop3_s.arr[1];
    C = loop3_s.arr[2];

     float A_s[local_K+4],
	   C_s[local_N+4],
	   B_s[(local_K+4)*LDB+local_N+4],
	   tmp[4];

    int i,j,k;

    floatv4 va,vb,vc;
    for( i = 0;i < local_M; i ++){
        if(i % 64 == myid){
            get_reply = 0;
            pe_get(A+i*LDA,A_s , local_K*4);
            dma_syn();
            pe_get(C+i*LDC,C_s , local_N*4);
            dma_syn();

            get_reply = 0;
            pe_get(B, B_s , (local_K*LDB)*4);
            dma_syn();
            for( j = 0; j < local_N; j ++)
            {
		    vc = 0;
		    if(local_K % 4 != 0){
                         A_s[local_K]= 0;
                        A_s[local_K+1] = 0;
                        A_s[local_K+2] = 0;
                        A_s[local_K+3] = 0;
			B_s[local_K*LDB+j] = 0;
			B_s[(local_K+1)*LDB+j] = 0;
			B_s[(local_K+2)*LDB+j] = 0;
			B_s[(local_K+3)*LDB+j] = 0;
                   }

                    for( k = 0; k < local_K; k+=4)
                    {
//                        *(C_s+j) += *(A_s+k) * *(B_s+k*LDB+j);
		        simd_load(va,A_s+k);
                        vb = simd_set_floatv4(*(B_s + k*LDB+j),
					      *(B_s+(k+1)*LDB+j),
					      *(B_s+(k+2)*LDB+j),
					      *(B_s+(k+3)*LDB+j) );
			vc += va * vb; 
		    }
			simd_store(vc,tmp);
				*(C_s+j) = *(tmp) + *(tmp+1) + *(tmp+2) + *(tmp+3);
            }

            put_reply=0;
            pe_put(C+i*LDC , C_s,(local_N)*4);
            dma_syn();
        }
    }
}

void slave_sw_slave(struct param *arg)
{
	const int myid = athread_get_id(-1);
	if(myid == 0)
		printf("arg = %p \n",arg);
/*	struct param  loop_slave_s;
	int local_N,i;
	float *QK;
	double local_norm;

	int volatile get_reply,put_reply;
	const int myid = athread_get_id(-1);
	if(myid == 0){
		printf("arg = %p \n",arg);
		printf("data = %d \n",sizeof(param));
	}
	dma_init();
	pe_get(arg,&loop_slave_s,sizeof(param));
	dma_syn();
	
	local_N = loop_slave_s.len;
	local_norm = loop_slave_s.norm;
	QK = loop_slave_s.arr;
	float local_QK[5000];	
	
	int a = local_N % 64;
	int b = local_N / 64;
	if(myid == 0){
		printf("data = %d \n",sizeof(param));
		printf("local_N = %d\n",local_N);	
		printf("a = %d\n",a);
		printf("b = %d\n",b);
	}
	if(myid < a){
		get_reply = 0;
		athread_get(PE_MODE,QK+(myid*(b+1)),local_QK,(b+1)*4,(void*)&get_reply,0,0,0);
		while(get_reply!=1);

		for(i=0; i<b+1; i++){
			*(local_QK+i) /= local_norm; 
		}
		
		put_reply = 0;
		athread_put(PE_MODE,local_QK,QK+(myid*(b+1)),(b+1)*4,(void*)&put_reply,0,0);
		while(put_reply!=1);
	}else{
		get_reply = 0;
                athread_get(PE_MODE,QK+(myid*b+a),local_QK,(b)*4,(void*)&get_reply,0,0,0);
                while(get_reply!=1);

                for(i=0; i<b; i++){
                        *(local_QK+i) /= local_norm;
                }

                put_reply = 0;
                athread_put(PE_MODE,local_QK,QK+(myid*b+a),(b)*4,(void*)&put_reply,0,0);
                while(put_reply!=1);
	}*/
}
