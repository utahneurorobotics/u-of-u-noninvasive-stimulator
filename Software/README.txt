MultiChannel_StimShield_mat20231004 contains code for an Arduino Uno to control the noninvasive stimulator.
The micrcontroller uses serial commands from the USB to set the appropriate stimulation parameters. The microcontroller expects 8 chars for the amplitude and pulsewidth of each channel and one frequency for all channels to operate at. The final char can be used to toggle the stimulation oprating mode.

The stimulation commands are expected in the following order:0
amp1, pw1, amp2, pw2, amp3, pw3, freq, x 
