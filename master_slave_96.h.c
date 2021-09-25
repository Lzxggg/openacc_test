
struct _swacc_pointer 
{
  float **_extern_buf;
  double *_extern_sum;
}
;
typedef struct _swacc_private {
int acc_p_len;
int acc_p_i;}*__z;
typedef struct _swacc_ldm {
struct _swacc_pointer _pointer;
int acc_index;
int acc_lower;
int acc_upper;
int acc_myid;
int acc_corenum;
int acc_blockindex_i0_0;
int acc_data_ret;
double _ldm_sum;}__t;
extern __thread_local_fix void *accr_priv_ptr;
extern __thread_local_fix void *accr_ldm_ptr;
#define buf	(*((_swacc_ldm_data0x1b94c00)._pointer._extern_buf))
#define sum	(*((_swacc_ldm_data0x1b94c00)._pointer._extern_sum))
#define acc_p_i	(((__z)accr_priv_ptr)->acc_p_i)
#define acc_p_len	(((__z)accr_priv_ptr)->acc_p_len)
#define _ldm_sum	((_swacc_ldm_data0x1b94c00)._ldm_sum)
#define acc_data_ret	((_swacc_ldm_data0x1b94c00).acc_data_ret)
#define acc_blockindex_i0_0	((_swacc_ldm_data0x1b94c00).acc_blockindex_i0_0)
#define acc_corenum	((_swacc_ldm_data0x1b94c00).acc_corenum)
#define acc_myid	((_swacc_ldm_data0x1b94c00).acc_myid)
#define acc_upper	((_swacc_ldm_data0x1b94c00).acc_upper)
#define acc_lower	((_swacc_ldm_data0x1b94c00).acc_lower)
#define acc_index	((_swacc_ldm_data0x1b94c00).acc_index)
__thread_kernel("master_slave_96.h")__t _swacc_ldm_data0x1b94c00;
