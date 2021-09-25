
struct _swacc_pointer 
{
  const float **_extern_A;
  const float **_extern_B;
  float **_extern_C;
}
;
typedef struct _swacc_private {
const int acc_p_LDA;
const int acc_p_LDB;
const int acc_p_LDC;
int acc_p_M;
int acc_p_N;
int acc_p_K;
int acc_p_i;
int acc_p_j;
int acc_p_k;}*__z;
typedef struct _swacc_ldm {
struct _swacc_pointer _pointer;
int acc_index;
int acc_lower;
int acc_upper;
int acc_myid;
int acc_corenum;
int acc_blockindex_i0_0;
int acc_data_ret;}__t;
extern __thread_local_fix void *accr_priv_ptr;
extern __thread_local_fix void *accr_ldm_ptr;
#define A	(*((_swacc_ldm_data0x1c2bc00)._pointer._extern_A))
#define B	(*((_swacc_ldm_data0x1c2bc00)._pointer._extern_B))
#define C	(*((_swacc_ldm_data0x1c2bc00)._pointer._extern_C))
#define acc_p_k	(((__z)accr_priv_ptr)->acc_p_k)
#define acc_p_j	(((__z)accr_priv_ptr)->acc_p_j)
#define acc_p_i	(((__z)accr_priv_ptr)->acc_p_i)
#define acc_p_K	(((__z)accr_priv_ptr)->acc_p_K)
#define acc_p_N	(((__z)accr_priv_ptr)->acc_p_N)
#define acc_p_M	(((__z)accr_priv_ptr)->acc_p_M)
#define acc_p_LDC	(((__z)accr_priv_ptr)->acc_p_LDC)
#define acc_p_LDB	(((__z)accr_priv_ptr)->acc_p_LDB)
#define acc_p_LDA	(((__z)accr_priv_ptr)->acc_p_LDA)
#define acc_data_ret	((_swacc_ldm_data0x1c2bc00).acc_data_ret)
#define acc_blockindex_i0_0	((_swacc_ldm_data0x1c2bc00).acc_blockindex_i0_0)
#define acc_corenum	((_swacc_ldm_data0x1c2bc00).acc_corenum)
#define acc_myid	((_swacc_ldm_data0x1c2bc00).acc_myid)
#define acc_upper	((_swacc_ldm_data0x1c2bc00).acc_upper)
#define acc_lower	((_swacc_ldm_data0x1c2bc00).acc_lower)
#define acc_index	((_swacc_ldm_data0x1c2bc00).acc_index)
__thread_kernel("master_slave_58.h")__t _swacc_ldm_data0x1c2bc00;
