#include "open_mat_file.h"
#include <hdf5.h>
// #include <stdio.h>
#include <stdlib.h>

MatFileData readMatFile(const char *file_path, const char *dataset_name) {
  MatFileData matData;

  // Open the .mat file
  hid_t file_id = H5Fopen(file_path, H5F_ACC_RDONLY, H5P_DEFAULT);
  if (file_id < 0) {
    fprintf(stderr, "Error opening the .mat file.\n");
    exit(1);
  }

  // Access the dataset
  hid_t dataset_id = H5Dopen2(file_id, dataset_name, H5P_DEFAULT);
  if (dataset_id < 0) {
    fprintf(stderr, "Error opening the dataset from the .mat file.\n");
    H5Fclose(file_id);
    exit(1);
  }

  // Get the dataspace
  hid_t dataspace_id = H5Dget_space(dataset_id);

  // Get the dimensions of the dataset
  hsize_t dims[2];
  H5Sget_simple_extent_dims(dataspace_id, dims, NULL);

  matData.num_rows = dims[0];
  matData.num_columns = dims[1];

  // Allocate memory for the data
  matData.data =
      (double *)malloc(matData.num_rows * matData.num_columns * sizeof(double));

  // Read the data from the dataset
  H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
          matData.data);

  // Close resources
  H5Dclose(dataset_id);
  H5Sclose(dataspace_id);
  H5Fclose(file_id);

  return matData;
}