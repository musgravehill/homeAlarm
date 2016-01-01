/*
Measuring and Reporting Battery Level

Use a 1MΩ (R1) and 470KΩ (R2) resistor in series, connected to the positive terminal on the battery and ground and then connected the tap point to the A0 input on the CPU.

The tap point could be bypassed with a 0.1 uF capacitor to keep the noise level low, at this otherwise high impedance point.

The ADC is set to use the internal reference value of 1.1V - so Vmax at ADCmax = 1.1*(16+4703)/4703 = 3.44V

The battery power value must be converted to a percentage compatable with your Controller.

See the following example on how to measure battery level on A0 and report it to the Controller.
file:///G:/!HOBBY/!ARDU/PROJECTS/homeAlarm/MAN/sensorsGuide_files/volts.png


#if defined(__AVR_ATmega2560__)
   analogReference(INTERNAL1V1);
#else
   analogReference(INTERNAL);
#endif

*/
