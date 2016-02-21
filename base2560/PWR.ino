/*
  Measuring and Reporting Battery Level

  Use a 1MΩ (R1) and 470KΩ (R2) resistor in series, connected to the positive terminal on the battery and ground and then connected the tap point to the A0 input on the CPU.

  The tap point could be bypassed with a 0.1 uF capacitor to keep the noise level low, at this otherwise high impedance point.

  The ADC is set to use the internal reference value of 1.1V - so Vmax at ADCmax = 1.1*(16+4703)/4703 = 3.44V

  The battery power value must be converted to a percentage compatable with your Controller.

  See the following example on how to measure battery level on A0 and report it to the Controller.
  http://www.mysensors.org/battery/volts.png

  MAX 1.1V for ADC A0..A*


  #if defined(__AVR_ATmega2560__)
   analogReference(INTERNAL2V56);
  #else
   analogReference(INTERNAL);
  #endif

  DEFAULT: опорное напряжение по умолчанию, равное 5 В (на 5В-платах Ардуино) или 3.3 В (на 3.3В-платах Ардуино)
  INTERNAL: внутренне опорное напряжение, равное 1.1 В в микроконтроллерах ATmega168 и ATmega328, или 2.56 В в микроконтроллере ATmega8 (не доступно в Arduino Mega)
  INTERNAL1V1: внутреннее опорное напряжение 1.1 В (только для Arduino Mega)
  INTERNAL2V56: внутреннее опорное напряжение 2.56 В (только для Arduino Mega)
  EXTERNAL: в качестве опорного напряжения будет использоваться напряжение, приложенное к выводу AREF (от 0 до 5В)
*/

void PWR_updateInfo() {
  BASE_voltage_base = analogRead(BASE_voltage_base_pin) * 0.4503245;
  BASE_voltage_acdc = analogRead(BASE_voltage_acdc_pin) * 2.0297521;
  BASE_voltage_battery = analogRead(BASE_voltage_battery_pin) * 2.37066;
}


