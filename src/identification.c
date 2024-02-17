#include "identification.h"
#include "kiss_fft.h"
#include "kiss_fftr.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// define complex arithmetics

// Complex addition
kiss_fft_cpx add_cpx(kiss_fft_cpx c1, kiss_fft_cpx c2) {
  kiss_fft_cpx c_out;
  c_out.r = c1.r + c2.r;
  c_out.i = c1.i + c2.i;
  return c_out;
}

// Complex substraction
kiss_fft_cpx sub_cpx(kiss_fft_cpx c1, kiss_fft_cpx c2) {
  kiss_fft_cpx c_out;
  c_out.r = c1.r - c2.r;
  c_out.i = c1.i - c2.i;
  return c_out;
}

// Complex multiplication
kiss_fft_cpx mul_cpx(kiss_fft_cpx c1, kiss_fft_cpx c2) {
  kiss_fft_cpx c_out;
  c_out.r = c1.r * c2.r - c1.i * c2.i;
  c_out.i = c1.i * c2.r + c1.r * c2.i;
  return c_out;
}

// Complex division
kiss_fft_cpx div_cpx(kiss_fft_cpx c1, kiss_fft_cpx c2) {
  kiss_fft_cpx c_out;
  c_out.r = (c1.r * c2.r + c1.i * c2.i) / (c2.r * c2.r + c2.i * c2.i);
  c_out.i = (c1.i * c2.r - c1.r * c2.i) / (c2.r * c2.r + c2.i * c2.i);
  return c_out;
}

// Magnitude
float_t mag_cpx(kiss_fft_cpx c) { return sqrt(pow(c.r, 2) + pow(c.i, 2)); }

// Angle
float_t ang_cpx(kiss_fft_cpx c) {
  float_t angle;
  if (c.r == 0) {
    angle = M_PI * (signbit(c.i) * -1);
  } else {
    angle = atan2(c.i, c.r);
  }
  return angle;
}

// Parse a Mat file data, slice the data into different states

System_Data data_parser(const MatFileData raw_data) {
  System_Data output_data;

  // Define raw data format
  // uint8_t time_index = 0;
  uint8_t prbs_index = 1;
  uint8_t current_index = 2;
  uint8_t voltage_index = 3;

  uint32_t start_index = 0;
  uint32_t stop_index = raw_data.num_rows - 1;

  // Compute size of data_len

  while (raw_data.data[(start_index * raw_data.num_columns + prbs_index)] ==
         0) {
    start_index++;
  }
  while (raw_data.data[(stop_index * raw_data.num_columns + prbs_index)] == 0) {
    stop_index--;
  }

  if (stop_index - start_index + 1 > 0) {
    output_data.data_len = stop_index - start_index + 1;
  } else {
    fprintf(stderr, "Error parsing the .mat file data.\n");
    exit(1);
  }
  output_data.data_len = output_data.data_len / 2;
  // Alocate memory to each data

  output_data.prbs =
      (kiss_fft_scalar *)malloc(output_data.data_len * sizeof(kiss_fft_scalar));
  output_data.clean_current =
      (kiss_fft_scalar *)malloc(output_data.data_len * sizeof(kiss_fft_scalar));
  output_data.clean_voltage =
      (kiss_fft_scalar *)malloc(output_data.data_len * sizeof(kiss_fft_scalar));
  output_data.noisy_current =
      (kiss_fft_scalar *)malloc(output_data.data_len * sizeof(kiss_fft_scalar));
  output_data.noisy_voltage =
      (kiss_fft_scalar *)malloc(output_data.data_len * sizeof(kiss_fft_scalar));

  // check for memory allocation fail
  if (output_data.prbs == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  if (output_data.clean_current == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  if (output_data.clean_voltage == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  if (output_data.noisy_current == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  if (output_data.noisy_voltage == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

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
        raw_data.data[(start_index + output_data.data_len + i) *
                          raw_data.num_columns +
                      current_index];
    output_data.noisy_voltage[i] =
        raw_data.data[(start_index + output_data.data_len + i) *
                          raw_data.num_columns +
                      voltage_index];
    output_data.prbs[i] =
        raw_data.data[(start_index + output_data.data_len + i) *
                          raw_data.num_columns +
                      prbs_index];
  }

  free(raw_data.data);
  return output_data;
};

Fft_Data compute_fft(const System_Data in, const float sampling_period) {
  uint32_t n = (in.data_len) - ((in.data_len) % 4); // ensure that n is even
  uint32_t start_index =
      in.data_len - n; // take only the second PRBS injection into account
  // define FFT config
  kiss_fftr_cfg cfg = kiss_fftr_alloc(n, 0, NULL, NULL);
  Fft_Data out;
  out.data_len = n / 2 + 1;

  // allocate memory of the FFT signals
  out.cc = (kiss_fft_cpx *)malloc(out.data_len * sizeof(kiss_fft_cpx));
  out.cv = (kiss_fft_cpx *)malloc(out.data_len * sizeof(kiss_fft_cpx));
  out.nc = (kiss_fft_cpx *)malloc(out.data_len * sizeof(kiss_fft_cpx));
  out.nv = (kiss_fft_cpx *)malloc(out.data_len * sizeof(kiss_fft_cpx));

  // check for fail mem alloc
  if (out.cc == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  if (out.cv == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  if (out.nc == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }
  if (out.cc == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  // Compute the FTTs
  kiss_fftr(cfg, in.clean_current + start_index, out.cc);
  kiss_fftr(cfg, in.clean_voltage + start_index, out.cv);
  kiss_fftr(cfg, in.noisy_current + start_index, out.nc);
  kiss_fftr(cfg, in.noisy_voltage + start_index, out.nv);

  // Define the frequency list
  out.freqs_list = (float_t *)malloc(out.data_len * sizeof(float_t));
  if (out.freqs_list == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  for (int i = 0; i < out.data_len; i++) {
    out.freqs_list[i] = i / (2 * sampling_period * out.data_len);
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

Impedance compute_impedance(const Fft_Data sys_fft) {
  Impedance sys_impedance;
  sys_impedance.data_len = sys_fft.data_len;
  sys_impedance.freqs_list = sys_fft.freqs_list;
  sys_impedance.data =
      (Impedance_data *)malloc(sys_impedance.data_len * sizeof(Impedance_data));

  // check for failed memory alloc
  if (sys_impedance.data == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  const float_t static_gain = 6000;

  for (int i = 0; i < sys_impedance.data_len; i++) {

    sys_impedance.data[i].amplitude =
        static_gain / mag_cpx(sub_cpx(sys_fft.nc[i], sys_fft.cc[i]));

    sys_impedance.data[i].angle =
        ang_cpx(div_cpx(sys_fft.nv[i], sys_fft.nc[i]));
  }

  free(sys_fft.cc);
  free(sys_fft.cv);
  free(sys_fft.nc);
  free(sys_fft.nv);
  return sys_impedance;
}

Impedance smoothing_filter(const Impedance sys_impedance,
                           const float_t smoothing_factor) {
  Impedance filtered_impedance;

  filtered_impedance.data_len = sys_impedance.data_len;
  filtered_impedance.freqs_list = sys_impedance.freqs_list;
  filtered_impedance.data = (Impedance_data *)malloc(
      filtered_impedance.data_len * sizeof(Impedance_data));

  // check for failed memory alloc
  if (filtered_impedance.data == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    exit(1);
  }

  for (uint32_t i = 0; i < sys_impedance.data_len; i++) {
    uint32_t low_bound = i;
    uint32_t high_bound = i;
    Impedance_data current_impedance;
    float_t current_freq = sys_impedance.freqs_list[i];

    while ((sys_impedance.freqs_list[low_bound] >
            current_freq / smoothing_factor) &&
           (low_bound > 0)) {
      low_bound--;
    }
    low_bound++;

    while ((sys_impedance.freqs_list[high_bound] <
            current_freq * smoothing_factor) &&
           (high_bound < sys_impedance.data_len - 1)) {
      high_bound++;
    }
    high_bound--;
    for (uint32_t j = low_bound; j < high_bound + 1; j++) {
      current_impedance.amplitude += sys_impedance.data[j].amplitude;
      current_impedance.angle += sys_impedance.data[j].angle;
    }
    current_impedance.amplitude /= (high_bound - low_bound + 1);
    current_impedance.angle /= (high_bound - low_bound + 1);
    filtered_impedance.data[i] = current_impedance;
  }

  return filtered_impedance;
}

void create_input_tensor(const Impedance sys_impedance, const float R,
                         const float L, const float C, float_t *input_tensor) {

  // list of the frequencies at which the input tensor is created
  uint32_t freqs[] = {50000,  53937,  58185,  62767,  67710,
                      73042,  78794,  85000,  91693,  98914,
                      106704, 115107, 124172, 133951, 144500};
  uint32_t current_index = 0;
  for (uint8_t i = 0; i < 15;
       i++) { // use the derivative of freqs[i] -
              // sys_impedance.freqs_list[current_index] to find the closest
              // indexs to the wanted freqs
    while (freqs[i] - sys_impedance.freqs_list[current_index] > 0) {
      current_index++;
    }
    current_index =
        abs((int)(freqs[i] - sys_impedance.freqs_list[current_index - 1])) <
                abs((int)(freqs[i] - sys_impedance.freqs_list[current_index]))
            ? current_index - 1
            : current_index;
    // input_tensor[i * 2] = (float)i;
    // input_tensor[i * 2 + 1] =sys_impedance.freqs_list[current_index];

    kiss_fft_cpx z1_cpx;
    z1_cpx.r = R;
    z1_cpx.i = L * 2 * M_PI * sys_impedance.freqs_list[current_index] -
               1 / (C * 2 * M_PI * sys_impedance.freqs_list[current_index]);

    kiss_fft_cpx z2_cpx;
    z2_cpx.r = sys_impedance.data[current_index].amplitude *
                   cos(sys_impedance.data[current_index].angle) -
               z1_cpx.r;
    z2_cpx.i = sys_impedance.data[current_index].amplitude *
                   sin(sys_impedance.data[current_index].angle) -
               z1_cpx.i;

    input_tensor[i * 2] = mag_cpx(z2_cpx);
    input_tensor[i * 2 + 1] = ang_cpx(z2_cpx);
  }

  return;
}