#include "main.h"
#include "wheel.hpp"

const double oneRot = 4 * M_PI / sqrt(2);
const uint8_t FORWARD = 0;
const uint8_t BACKWARD = 1;
const uint8_t LEFT = 2;
const uint8_t RIGHT = 3;

void move(std::array<Wheel, 4> wheels, uint8_t direction, double inches) {
	std::array<double, 4> mags = director(direction, inches / oneRot * 360);
	wheelH(wheels, mags);
	delay(inches * 100.0);
}

void wheelH(std::array<Wheel, 4> &wheels, std::array<double, 4> &magnitude) {
	for (int i = 0; i < 4; i++) {
		wheels[i].motor.move_relative(magnitude[i], 100);
	}
}

std::array<double, 4> director(uint8_t direction, double magnitude) {
	std::array<double, 4> returnVal;
	switch (direction) {
	case FORWARD:
		returnVal = {magnitude, magnitude, magnitude, magnitude};
		break;
	case BACKWARD:
		returnVal = {-magnitude, -magnitude, -magnitude, -magnitude};
		break;
	case LEFT:
		returnVal = {-magnitude, magnitude, magnitude, -magnitude};
		break;
	case RIGHT:
		returnVal = {magnitude, -magnitude, -magnitude, magnitude};
		break;
	}
	return returnVal;
}