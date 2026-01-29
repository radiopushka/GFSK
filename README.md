# Gaussian Frequency Shift Keying Modulation Library and test suite

## <12 kbps at a bandwidth of 16 khz with a 48 khz sample rate (75% spectral efficiency)

## FM modulated (DSB) on an FM transmitter: <8kbps 32khz total spectral usage, dual side bands, so good noise immunity
## if HD radio is taking up a channel, it can take up the other side of the channel.
- Sample rate must be 6x the bit rate for successful and robust decoding, aka you would need a 1.2 MHz sample rate to demodulate 200kbps, 6 MHz sample rate to demodulate 1Mbps
- Use an SSB modulator and demodulator for maximum total spectral efficiency.

<img width="1919" height="1173" alt="2026-01-28_16-57" src="https://github.com/user-attachments/assets/4c10739a-0d1f-4db7-8830-824775c09501" />
