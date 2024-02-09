#include "data_parser.h"
#include "kiss_fft.h"
#include "kiss_fftr.h"
#include "open_mat_file.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Function to open, read, and close the .mat file, returning the extracted data

typedef struct {
  kiss_fft_cpx *cc; // Clean current FFT
  kiss_fft_cpx *cv; // Clean voltage FFT
  kiss_fft_cpx *nc; // Noisy current FFT
  kiss_fft_cpx *nv; // Noisy voltage FFT
  float_t *freqs_list;
  uint32_t data_len;

} fft_data;

typedef struct {
  kiss_fft_cpx *data; // Clean current FFT
  float_t *freqs_list;
  uint32_t data_len;
} Impedance;

// Function that compute the FFT of the current and voltage (noisy and clean)
fft_data compute_fft(const System_Data in, const float sampling_period) {
  int n = in.data_len - in.data_len % 2; // ensure that n is even
  kiss_fftr_cfg cfg = kiss_fftr_alloc(n, 0, NULL, NULL);
  fft_data out;
  out.data_len = (in.data_len - in.data_len % 2) / +1;

  // allocate memory of the FFT signals
  out.cc = (kiss_fft_cpx *)malloc(out.data_len * sizeof(kiss_fft_cpx));
  out.cv = (kiss_fft_cpx *)malloc(out.data_len * sizeof(kiss_fft_cpx));
  out.nc = (kiss_fft_cpx *)malloc(out.data_len * sizeof(kiss_fft_cpx));
  out.nv = (kiss_fft_cpx *)malloc(out.data_len * sizeof(kiss_fft_cpx));

  // Compute the FTTs
  kiss_fftr(cfg, in.clean_current, out.cc);
  kiss_fftr(cfg, in.clean_voltage, out.cv);
  kiss_fftr(cfg, in.noisy_current, out.nc);
  kiss_fftr(cfg, in.noisy_voltage, out.nv);

  // Define the frequency list
  out.freqs_list = (float_t *)malloc(out.data_len * sizeof(float_t));

  for (int i = 0; i < out.data_len; i++) {
    out.freqs_list[i] = i / (sampling_period * out.data_len);
  }

  // Free unused memory
  free(cfg);
  free(in.clean_current);
  free(in.clean_voltage);
  free(in.noisy_current);
  free(in.noisy_voltage);
  free(in.prbs);

  kiss_fft_cleanup();

  return out;
}

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

  // Compute FFT of parsed signals

  float_t sampling_period = pow(10, -6);

  fft_data fft_signals = compute_fft(parsed_data, sampling_period);

  // Free allocated memory

  free(fft_signals.cc);
  free(fft_signals.cv);
  free(fft_signals.nc);
  free(fft_signals.nv);
  free(fft_signals.freqs_list);

  return 0;
}