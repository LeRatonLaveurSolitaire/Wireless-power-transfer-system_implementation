#include <hdf5.h>

#ifndef OPEN_MAT_FILE_H
#define OPEN_MAT_FILE_H


typedef struct {
  double *data;
  size_t num_rows;
  size_t num_columns;
} MatFileData;


MatFileData readMatFile(const char *file_path, const char *dataset_name);

#endif