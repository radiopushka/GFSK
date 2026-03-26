# Gaussian Frequency Shift Keying Modulation Library and test suite

## <12 kbps at a bandwidth of 16 khz with a 48 khz sample rate (75% spectral efficiency)

## FM modulated (DSB) on an FM transmitter: <8kbps 32khz total spectral usage, dual side bands, so good noise immunity
## if HD radio is taking up a channel, it can take up the other side of the channel.
- Sample rate must be 6x the bit rate for successful and robust decoding, aka you would need a 1.2 MHz sample rate to demodulate 200kbps, 6 MHz sample rate to demodulate 1Mbps
- Use an SSB modulator and demodulator for maximum total spectral efficiency.

![5055764089442864081](https://github.com/user-attachments/assets/934f6e60-24f1-4ee7-b8d0-397f4e75bf07)

