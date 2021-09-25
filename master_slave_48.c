#include <math.h> 
#include <sys/time.h> 
#include <swacc_slave.h> 
#include </home/export/online3/cesm02/liujw/xiaol/OpenACC_test/test/master-slave/master_slave_48.h.c> 

void slave_master_48(void *argv)
{
  acc_check_ldm(0 + sizeof(acc_data_ret));
  acc_sync_pe_m2l_nostride_copy(&_swacc_ldm_data0x1c2bc70 . _pointer,argv,24);
  acc_myid = acc_get_thread_num();
  acc_corenum = acc_get_num_threads();
{
    for (acc_blockindex_i0_1 = acc_myid * 1; acc_blockindex_i0_1 <= acc_p_M - 1; acc_blockindex_i0_1 += 1 * acc_corenum) {
      for (acc_index = 0; acc_index <= 0; acc_index += 1) {
        for (acc_p_j = 0; acc_p_j <= acc_p_N - 1; acc_p_j += 1) {
          for (acc_p_k = 0; acc_p_k <= acc_p_K - 1; acc_p_k += 1) {
            C[(acc_index + acc_blockindex_i0_1) * acc_p_LDC + acc_p_j] += A[(acc_index + acc_blockindex_i0_1) * acc_p_LDA + acc_p_k] * B[acc_p_k + acc_p_j * acc_p_LDB];
          }
        }
      }
    }
  }
}
