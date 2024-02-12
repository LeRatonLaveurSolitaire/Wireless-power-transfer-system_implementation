#ifndef DATA_PARSER
#define DATA_PARSER

#include "kiss_fft.h"
#include "open_mat_file.h"
#include <math.h>
#include <stdint.h>

// Strucutre of the data before analysis

typedef struct {
  kiss_fft_scalar *prbs;
  kiss_fft_scalar *clean_current;
  kiss_fft_scalar *clean_voltage;
  kiss_fft_scalar *noisy_current;
  kiss_fft_scalar *noisy_voltage;
  uint32_t data_len;
} System_Data;

typedef struct {
  kiss_fft_cpx *cc; // Clean current FFT
  kiss_fft_cpx *cv; // Clean voltage FFT
  kiss_fft_cpx *nc; // Noisy current FFT
  kiss_fft_cpx *nv; // Noisy voltage FFT
  float_t *freqs_list;
  uint32_t data_len;

} Fft_Data;

typedef struct {
  float_t amplitude;
  float_t angle;
} Impedance_data;

typedef struct {
  Impedance_data *data; // Clean current FFT
  float_t *freqs_list;
  uint32_t data_len;
} Impedance;

// define basic complex arithmetics

kiss_fft_cpx add_cpx(kiss_fft_cpx c1, kiss_fft_cpx c2);
kiss_fft_cpx sub_cpx(kiss_fft_cpx c1, kiss_fft_cpx c2);
kiss_fft_cpx mul_cpx(kiss_fft_cpx c1, kiss_fft_cpx c2);
kiss_fft_cpx div_cpx(kiss_fft_cpx c1, kiss_fft_cpx c2);
float_t mag_cpx(kiss_fft_cpx c);
float_t ang_cpx(kiss_fft_cpx c);

// Data parser function

System_Data data_parser(const MatFileData row_data);

Fft_Data compute_fft(const System_Data in, const float sampling_period);

Impedance compute_impedance(const Fft_Data sys_fft);

Impedance smoothing_filter(const Impedance sys_impedance,
                           const float_t smoothing_factor);

#endif