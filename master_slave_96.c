#include <math.h> 
#include <sys/time.h> 
#include <swacc_slave.h> 
#include </home/export/online3/cesm02/liujw/xiaol/OpenACC_test/test/master-slave/master_slave_96.h.c> 

void slave_master_96(void *argv)
{
  acc_check_ldm(0 + sizeof(acc_data_ret) + sizeof(_ldm_sum));
  acc_sync_pe_m2l_nostride_copy(&_swacc_ldm_data0x1b94c00 . _pointer,argv,16);
  acc_myid = acc_get_thread_num();
  acc_corenum = acc_get_num_threads();
  _ldm_sum = sum;
{
    for (acc_blockindex_i0_0 = acc_myid * 1; acc_blockindex_i0_0 <= acc_p_len - 1; acc_blockindex_i0_0 += 1 * acc_corenum) {
      for (acc_index = 0; acc_index <= 0; acc_index += 1) {
        _ldm_sum += ((double )buf[acc_index + acc_blockindex_i0_0]);
      }
    }
  }
}
