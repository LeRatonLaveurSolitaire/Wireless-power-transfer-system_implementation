#include "data_parser.h"
#include <stdint.h>
#include <stdlib.h>

System_Data data_parser(const MatFileData raw_data) {
  System_Data output_data;

  // Define row data format
  // uint8_t time_index = 0;
  uint8_t prbs_index = 1;
  uint8_t current_index = 2;
  uint8_t voltage_index = 3;

  uint64_t start_index = 0;
  uint64_t stop_index = raw_data.num_rows - 1;

  // Compute size of data_len

  while (raw_data.data[(start_index * raw_data.num_columns + prbs_index)] ==
         0) {
    start_index++;
  }
  while (raw_data.data[(stop_index * raw_data.num_columns + prbs_index)] == 0) {
    stop_index--;
  }

  output_data.data_len = stop_index - start_index + 1;

  // Alocate memory to each data

  output_data.prbs = (double *)malloc(output_data.data_len * sizeof(double));
  output_data.clean_current =
      (double *)malloc(output_data.data_len * sizeof(double));
  output_data.clean_voltage =
      (double *)malloc(output_data.data_len * sizeof(double));
  output_data.noisy_current =
      (double *)malloc(output_data.data_len * sizeof(double));
  output_data.noisy_voltage =
      (double *)malloc(output_data.data_len * sizeof(double));

  // Define data value

  for (int i = 0; i < output_data.data_len; i++) {
    output_data.clean_current[i] =
        raw_data.data[(start_index - output_data.data_len + i) *
                          raw_data.num_columns +
                      current_index];
    output_data.clean_voltage[i] =
        raw_data.data[(start_index - output_data.data_len + i) *
                          raw_data.num_columns +
                      voltage_index];
    output_data.noisy_current[i] =
        raw_data.data[(start_index + i) * raw_data.num_columns + current_index];
    output_data.noisy_voltage[i] =
        raw_data.data[(start_index + i) * raw_data.num_columns + voltage_index];
    output_data.prbs[i] =
        raw_data.data[(start_index + i) * raw_data.num_columns + prbs_index];
  }
  return output_data;
};
