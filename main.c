#include "identification.h"
#include "open_mat_file.h"
// #include "inference.h"
#include <stdio.h>
#include <stdlib.h>

#define R1 0.7                        //  0.7 Ohm
#define L1 0.000236                   //  236 µH
#define C1 0.000000014855607243319716 //  14.8 nF

int main() {
  // Specify the path to the .mat file
  const char *file_path =
      "data/sim_values_row_PRBS10_start_up_values_with_voltage.mat";

  // Specify the dataset name
  const char *dataset_name = "/values";

  // Read the .mat file and get the extracted data
  MatFileData matData = readMatFile(file_path, dataset_name);

  // char to_print[matData.num_columns * matData.num_rows * 15 + 300];
  // int index = 0;
  // char start[500];

  // sprintf(start,
  //         "MatFileData matData;\nmatData.num_columns = %lu\nmatData.num_rows
  //         = "
  //         "%lu\nmatData.data = {",
  //         matData.num_columns, matData.num_rows);

  // while (start[index] != 0) {
  //   to_print[index] = start[index];
  //   index++;
  // }
  // for (int i = 0; i < matData.num_columns * matData.num_rows; i++) {
  //   char to_add[20];
  //   sprintf(to_add, "%f,", matData.data[i]);
  //   int index_2 = 0;
  //   while (to_add[index_2] != 0) {
  //     to_print[index] = to_add[index_2];
  //     index++;
  //     index_2++;
  //   }
  // }
  // char stop[] = "};";
  // int index_2 = 0;
  // while (stop[index_2] != 0) {
  //   to_print[index] = stop[index_2];
  //   index++;
  //   index_2++;
  // }
  // printf("%s", to_print);

  // Parse the data to extract the different steps (clean, PRBS)
  System_Data parsed_data = data_parser(matData);

  // Compute FFT of parsed signals
  float_t sampling_period = pow(10, -6); // Ts = 1e-6
  Fft_Data fft_signals = compute_fft(parsed_data, sampling_period);

  // for (int i = 0; i < fft_signals.data_len / 3; i++) {
  //   char to_print[200];
  //   sprintf(to_print, "%d\t%f\t%f\t%f\t%f\t%f\t%f\n",
  //           (int)fft_signals.freqs_list[i], fft_signals.cc[i].r,
  //           fft_signals.cc[i].i, fft_signals.nc[i].r, fft_signals.nc[i].i,
  //           fft_signals.nv[i].r, fft_signals.nv[i].i);
  //   printf("%s", to_print);
  // }

  // Compute Impedance from the FFT signals
  Impedance sys_impedance = compute_impedance(fft_signals);

  Impedance filtered_impedance = smoothing_filter(sys_impedance, 1.05);

  float *input_tensor = (float_t *)malloc(30 * sizeof(float_t));

  create_input_tensor(filtered_impedance, R1, L1, C1, input_tensor);

  for (int i = 0; i < 15; i++) {
    printf("%f\t%f\n", input_tensor[2 * i], input_tensor[2 * i + 1]);
  }
  free(input_tensor);
  // printf("Frequency,Magnitude,Angle, filterd_mag,filtered angle\n");
  // char to_print[150];
  // for (int i = 0; i < 10; i++) {
  //   sprintf(to_print, "%f,%f,%f,%f,%f\n", sys_impedance.freqs_list[i],
  //           sys_impedance.data[i].amplitude, sys_impedance.data[i].angle,
  //           filtered_impedance.data[i].amplitude,
  //           filtered_impedance.data[i].angle); // char
  //           to_print[matData.num_columns * matData.num_rows * 15 + 300];
  // int index = 0;
  // char start[500];

  // sprintf(start,
  //         "MatFileData matData;\nmatData.num_columns =
  //         %lu\nmatData.num_rows = "
  //         "%lu\nmatData.data = {",
  //         matData.num_columns, matData.num_rows);

  // while (start[index] != 0) {
  //   to_print[index] = start[index];
  //   index++;
  // }
  // for (int i = 0; i < matData.num_columns * matData.num_rows; i++) {
  //   char to_add[20];
  //   sprintf(to_add, "%f,", matData.data[i]);
  //   int index_2 = 0;
  //   while (to_add[index_2] != 0) {
  //     to_print[index] = to_add[index_2];
  //     index++;
  //     index_2++;
  //   }
  // }
  // char stop[] = "};";
  // int index_2 = 0;
  // while (stop[index_2] != 0) {
  //   to_print[index] = stop[index_2];
  //   index++;
  //   index_2++;
  // }
  // printf("%s", to_print);
  //   printf("%s", to_print);
  // }

  // Free allocated memory
  free(sys_impedance.data);
  free(sys_impedance.freqs_list);
  free(filtered_impedance.data);

  // printf("Program ran successfully ! \\`o`/ \n");
  return 0;
}