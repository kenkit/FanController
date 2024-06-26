/*
  FanController.h - Library to control up to six fans.
  Created by Giorgio Aresu, November 13, 2016.
  Released into the public domain.
*/
#ifndef FanController_h
#define FanController_h

#include "Arduino.h"

class FanController
{
public:
	FanController(byte sensorPin, byte mofsetPin, unsigned int sensorThreshold, unsigned int minimum_pwm_to_off = 30, byte pwmPin = 0);
	void begin();
	unsigned int getSpeed();
	void setDutyCycle(byte dutyCycle);
	byte getDutyCycle();
	void update_minimum_pwm(unsigned int switchofpwm)
	{
		_minimum_pwm_to_shutdown = switchofpwm;
	}

private:
	static FanController *_instances[6];
	byte _sensorPin, _mofsetpin;
	byte _sensorInterruptPin;
	unsigned int _sensorThreshold;
	byte _pwmPin;
	byte _pwmDutyCycle, lastpwmDutyCycle;
	byte _instance;
	unsigned int _lastReading, _minimum_pwm_to_shutdown;
	volatile unsigned int _halfRevs;
	unsigned long _lastMillis;
	void _trigger();
	void _attachInterrupt();
	static void _triggerCaller(byte instance);
	static void _triggerExt0();
	static void _triggerExt1();
	static void _triggerExt2();
	static void _triggerExt3();
	static void _triggerExt4();
	static void _triggerExt5();
};

#endif