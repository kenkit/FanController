#include "Arduino.h"
#include "FanController.h"

#if defined(ARDUINO_ARCH_ESP32)
// #include <analogWrite.h>
#endif

#if defined(ESP8266)
#define ISR_PREFIX ICACHE_RAM_ATTR
#else
#define ISR_PREFIX
#endif

FanController::FanController(byte sensorPin, byte mofsetPin, unsigned int sensorThreshold, unsigned int minimum_pwm_to_off, byte pwmPin) : _minimum_pwm_to_shutdown(minimum_pwm_to_off), lastpwmDutyCycle(0)
{
	_sensorPin = sensorPin;
	_sensorInterruptPin = digitalPinToInterrupt(sensorPin);
	_sensorThreshold = sensorThreshold;
	_pwmPin = pwmPin;
	_mofsetpin = mofsetPin;
	pinMode(pwmPin, OUTPUT);
	pinMode(_mofsetpin, OUTPUT);
	_pwmDutyCycle = 100;
}

void FanController::begin()
{
	static byte instance;
	_instance = instance;
	_instances[instance] = this;
#if defined(ARDUINO_ARCH_ESP32)
	analogWriteResolution(10);
	analogWriteFrequency(25000);
#endif
	digitalWrite(_sensorPin, HIGH);
	setDutyCycle(_pwmDutyCycle);
	digitalWrite(_mofsetpin, HIGH); // turn the mofset off.
	_attachInterrupt();
	instance++;
}

unsigned int FanController::getSpeed()
{
	unsigned int elapsed = millis() - _lastMillis;
	if (elapsed > _sensorThreshold)
	{
		detachInterrupt(_sensorInterruptPin);
		double correctionFactor = 1000.0 / elapsed;
		_lastReading = correctionFactor * _halfRevs / 2 * 60;
		_halfRevs = 0;
		_lastMillis = millis();
		_attachInterrupt();
	}
	return _lastReading;
}

void FanController::setDutyCycle(byte dutyCycle)
{
	_pwmDutyCycle = min((int)dutyCycle, 100);
	analogWrite(_pwmPin, 2.55 * _pwmDutyCycle);
	if (lastpwmDutyCycle != _pwmDutyCycle)
	{
		if (_pwmDutyCycle <= _minimum_pwm_to_shutdown)
		{
			digitalWrite(_mofsetpin, HIGH); // turn the mofset off.
		}
		else
		{
			digitalWrite(_mofsetpin, LOW); // turn the mofset on.
		}
		lastpwmDutyCycle = _pwmDutyCycle;
	}
}

byte FanController::getDutyCycle()
{
	return _pwmDutyCycle;
}

void FanController::_attachInterrupt()
{
	switch (_instance)
	{
	case 0:
		attachInterrupt(_sensorInterruptPin, _triggerExt0, FALLING);
		break;
	case 1:
		attachInterrupt(_sensorInterruptPin, _triggerExt1, FALLING);
		break;
	case 2:
		attachInterrupt(_sensorInterruptPin, _triggerExt2, FALLING);
		break;
	case 3:
		attachInterrupt(_sensorInterruptPin, _triggerExt3, FALLING);
		break;
	case 4:
		attachInterrupt(_sensorInterruptPin, _triggerExt4, FALLING);
		break;
	case 5:
		attachInterrupt(_sensorInterruptPin, _triggerExt5, FALLING);
		break;
	}
}

FanController *FanController::_instances[6];

void FanController::_trigger()
{
	_halfRevs++;
}

void FanController::_triggerCaller(byte instance)
{
	if (FanController::_instances[instance] != nullptr)
	{
		FanController::_instances[instance]->_trigger();
	}
}

ISR_PREFIX void FanController::_triggerExt0() { FanController::_triggerCaller(0); }
ISR_PREFIX void FanController::_triggerExt1() { FanController::_triggerCaller(1); }
ISR_PREFIX void FanController::_triggerExt2() { FanController::_triggerCaller(2); }
ISR_PREFIX void FanController::_triggerExt3() { FanController::_triggerCaller(3); }
ISR_PREFIX void FanController::_triggerExt4() { FanController::_triggerCaller(4); }
ISR_PREFIX void FanController::_triggerExt5() { FanController::_triggerCaller(5); }