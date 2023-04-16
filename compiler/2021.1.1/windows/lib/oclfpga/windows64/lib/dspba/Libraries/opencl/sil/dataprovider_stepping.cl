
#include "dspba/config.h"

channel input_type dspb_in  __attribute__((depth(8)));

channel output_type dspb_out __attribute__((depth(256)));

channel csr_type dspb_csr __attribute__((depth(8)));


kernel void dataprovider_sender (global input_type  * test_vector_in)
{
  write_channel_altera(dspb_in,test_vector_in[0]);
}

kernel void dataprovider_receiver ( 
    global output_type * restrict test_response_out,
    global ulong * restrict response_count_out)
{
  bool valid = false;
  test_response_out[0] = read_channel_nb_altera(dspb_out, &valid);
  if (valid) {
    response_count_out[0] = 1;
  } else {
    response_count_out[0] = 0;
  }
}

kernel void dataprovider_csr (global csr_type * test_csr)
{
  write_channel_altera(dspb_csr,test_csr[0]);
}
