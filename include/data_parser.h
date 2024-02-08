#ifndef DATA_PARSER
#define DATA_PARSER

#include <stdint.h>
#include "open_mat_file.h"

// Strucutre of the data before analysis

typedef struct {
  double *prbs;
  double *clean_current;
  double *clean_voltage;
  double *noisy_current;
  double *noisy_voltage;
  uint64_t data_len;
} System_Data;


// Data parser function

System_Data data_parser(const MatFileData row_data);

#endif