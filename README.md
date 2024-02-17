C implementation of the signal processing part of the project for future porting on a microcontroller

The current code do :
* Load a mat file
* Slice the signals
* Compute the FFT of the signals
* Compute the impedance from the FFT
* Smooth the impedance
* Slice the impedance to recover the input tensor of the neural network