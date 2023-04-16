
#include "dspba/config.h"

channel input_type dspb_in  __attribute__((depth(8)));

channel output_type dspb_out __attribute__((depth(256)));

channel csr_type dspb_csr __attribute__((depth(2)));

channel uint stop __attribute__((depth(1)));


kernel void dataprovider_sender ( 
    global input_type  * restrict test_vector_in,
    ulong size)
{
  for ( ulong i = 0; i < size; i++ ) {
    write_channel_altera(dspb_in,test_vector_in[i]);
  }
}

kernel void dataprovider_receiver ( 
    global output_type * restrict test_response_out,
    ulong max_size,
    global ulong * restrict received_size)
{
  bool valid_data = false;
  bool valid_stop = false;
  ulong index = 0;
  uint stop_value = 0;
  bool done = false; 
  while ((index < max_size) && !done) {
    test_response_out[index] = read_channel_nb_altera(dspb_out, &valid_data);
    if (valid_data) {
      index = index + 1;
    }
    stop_value = read_channel_nb_altera(stop, &valid_stop); 
    done = valid_stop;
  }
  received_size[0] = index;
}

kernel void dataprovider_stop() 
{
  write_channel_altera(stop, (uint)1);
}

kernel void dataprovider_csr ( 
    global csr_type * test_csr,
    ulong size)
{
  for ( ulong i = 0; i < size; i++ )
    write_channel_altera(dspb_csr,test_csr[i]);
}
