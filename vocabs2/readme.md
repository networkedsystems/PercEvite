## Build

> make

## Run

./build/vocabs2.exe curve sigma loss speed

curve = Distance dependent loss curve
A = no loss
E = extrapolated from ESAT measurements
C = extrapolated from ADS-B measurements (NASA paper from Maria Consiglio)

sigma = (floating point) variance of the positional error distribution

loss = (floating point) fixed loss probability
Between 0 (all messages are lost) and 1000.0 (all messagesare received)

speed = (floating point) drone speed in m/s
Drones speed module remains constant along the simulation