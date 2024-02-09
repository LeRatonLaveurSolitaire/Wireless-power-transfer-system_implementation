#ifndef DATA_PARSER
#define DATA_PARSER

#include "kiss_fft.h"
#include "open_mat_file.h"
#include <stdint.h>

// Strucutre of the data before analysis

typedef struct {
  kiss_fft_scalar *prbs;
  kiss_fft_scalar *clean_current;
  kiss_fft_scalar *clean_voltage;
  kiss_fft_scalar *noisy_current;
  kiss_fft_scalar *noisy_voltage;
  uint64_t data_len;
} System_Data;

// Data parser function

System_Data data_parser(const MatFileData row_data);

#endif