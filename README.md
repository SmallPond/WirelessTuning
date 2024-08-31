# WirelessTuning

WirelessTuning supports wireless parameter tuning for SimpleFOC and can act as glue code between the Commander and Monitor. This requires no modifications to any code in SimpleFOC.

WirelessTuning encapsulates and implements a stream class that uses TCP for data transmission and reception.


```cpp


BLDCMotor motor = BLDCMotor(7);
BLDCDriver3PWM driver = BLDCDriver3PWM(MO0_1, MO0_2, MO0_3);

WirelessTuning wireless = WirelessTuning(4242);
Commander commander = Commander(wireless, '\n', false);


void setup()
{
    motor.useMonitoring(wireless);
    wireless.begin(ssid, passwd);
}


void loop()
{
    commander.run();
    motor.monitor();
}


```