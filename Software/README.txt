ArdStim contains Matlab code to communicate with a microcontroller running the simulator code.
ArdStim requires the mex implementation of CSerial by Jfriesen222: https://github.com/Jfriesen222/Mex_C_Serial_Interface
ArdStim operates as a class with the following methods:
- init() -> initializes communication
- stim([amp1, pw1, amp2, pw2, amp3, pw3, freq, 1]) -> sends the given stimulation parameters to the stimulator
- close() -> closes the serial communication

MultiChannel_StimShield_mat20231004 contains code for an Arduino Uno to control the noninvasive stimulator.
The micrcontroller uses serial commands from the USB to set the appropriate stimulation parameters. The microcontroller expects 8 chars for the amplitude and pulsewidth of each channel and one frequency for all channels to operate at. The final char can be used to toggle the stimulation oprating mode.

The stimulation commands are expected in the following order:0
amp1, pw1, amp2, pw2, amp3, pw3, freq, x 
