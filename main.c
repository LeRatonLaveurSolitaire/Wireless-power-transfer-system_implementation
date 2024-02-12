#include "identification.h"
#include "open_mat_file.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  // Specify the path to the .mat file
  const char *file_path =
      "data/sim_values_row_PRBS10_start_up_values_with_voltage.mat";

  // Specify the dataset name
  const char *dataset_name = "/values";

  // Read the .mat file and get the extracted data
  MatFileData matData = readMatFile(file_path, dataset_name);

  // Parse the data to extract the different steps (clean, PRBS)
  System_Data parsed_data = data_parser(matData);

  // Compute FFT of parsed signals
  float_t sampling_period = pow(10, -6); // Ts = 1e-6
  Fft_Data fft_signals = compute_fft(parsed_data, sampling_period);

  // Compute Impedance from the FFT signals
  Impedance sys_impedance = compute_impedance(fft_signals);

  Impedance filtered_impedance = smoothing_filter(sys_impedance, 1.07);

  // printf("Frequency,Magnitude,Angle, filterd_mag,filtered angle\n");
  char to_print[150];
  for (int i = 0; i < sys_impedance.data_len; i++) {
    sprintf(to_print, "%f,%f,%f,%f,%f\n", sys_impedance.freqs_list[i],
            sys_impedance.data[i].amplitude, sys_impedance.data[i].angle,
            filtered_impedance.data[i].amplitude,
            filtered_impedance.data[i].angle);
    printf("%s", to_print);
  }

  // Free allocated memory
  free(sys_impedance.data);
  free(sys_impedance.freqs_list);
  free(filtered_impedance.data);

  // printf("Program ran successfully ! \\`o`/ \n");
  return 0;
}