#include "data_parser.h"
#include "open_mat_file.h"
// #include <stdio.h>
#include <stdlib.h>

// Function to open, read, and close the .mat file, returning the extracted data

int main() {
  // Specify the path to the .mat file
  const char *file_path =
      "data/sim_values_row_PRBS10_start_up_values_with_voltage.mat";

  // Specify the dataset name
  const char *dataset_name = "/values";

  // Read the .mat file and get the extracted data
  MatFileData matData = readMatFile(file_path, dataset_name);

  // Parse the data to extract the different steps
  System_Data parsed_data = data_parser(matData);

  // Free allocated memory

  free(matData.data);
  free(parsed_data.clean_current);
  free(parsed_data.noisy_current);
  free(parsed_data.clean_voltage);
  free(parsed_data.noisy_voltage);
  free(parsed_data.prbs);
  return 0;
}