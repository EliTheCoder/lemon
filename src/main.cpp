#include "main.h"
#include "wheel.hpp"
using namespace pros;

void initialize() { lcd::initialize(); }
void disabled() {
	Controller master(E_CONTROLLER_MASTER);
	master.clear();
	master.rumble("_..");
	master.print(0, 0, "lemon is angy");
	master.print(1, 0, "lemon can't move >:(");
}

const double oneRot = 8 * M_PI / sqrt(2);
const uint8_t FORWARD = 0;
const uint8_t BACKWARD = 1;
const uint8_t LEFT = 2;
const uint8_t RIGHT = 3;

void wheelH(std::array<Wheel, 4> &wheels, std::array<double, 4> &magnitude) {
	for (int i = 0; i < 4; i++) {
		wheels[i].motor.move_relative(magnitude[i], 200);
	}
}

std::array<double, 4> director(uint8_t direction, double magnitude) {
	std::array<double, 4> returnVal;
	switch (direction) {
	case RIGHT:
		returnVal = {magnitude, magnitude, magnitude, magnitude};
		break;
	case LEFT:
		returnVal = {-magnitude, -magnitude, -magnitude, -magnitude};
		break;
	case BACKWARD:
		returnVal = {-magnitude, magnitude, magnitude, -magnitude};
		break;
	case FORWARD:
		returnVal = {magnitude, -magnitude, -magnitude, magnitude};
		break;
	}
	return returnVal;
}
void move(std::array<Wheel, 4> wheels, uint8_t direction, double inches) {
	std::array<double, 4> mags = director(direction, inches / oneRot * 360);
	wheelH(wheels, mags);
	// double vSum = 0;
	// delay(100);
	// for (int i = 0; i < 4; i++) {
	// 	vSum += wheels[i].motor.get_actual_velocity();
	// }
	// while (vSum > 0.0) {
	// 	delay(10);
	// }
	delay(inches / oneRot * 700);
}

void competition_initialize() {}
void autonomous() {

	std::array<Wheel, 4> wheels{
		Wheel{Motor{10}, 315.0},
		Wheel{Motor{18}, 225.0},
		Wheel{Motor{1, true}, 225.0},
		Wheel{Motor{11, true}, 315.0},
	};

	for (const Wheel &wheel : wheels) {
		wheel.motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);
		wheel.motor.set_encoder_units(E_MOTOR_ENCODER_DEGREES);
		wheel.motor.set_gearing(E_MOTOR_GEARSET_18);
	}

	move(wheels, FORWARD, 56.0);
	move(wheels, RIGHT, 60.0);

	for (const Wheel &wheel : wheels) {
		wheel.motor.set_brake_mode(E_MOTOR_BRAKE_COAST);
	}
}

void opcontrol() {
	Controller master(E_CONTROLLER_MASTER);

	std::array<Wheel, 4> wheels{
		Wheel{Motor{10}, 315.0},
		Wheel{Motor{18}, 225.0},
		Wheel{Motor{1, true}, 225.0},
		Wheel{Motor{11, true}, 315.0},
	};

	std::array<Motor, 2> claws{Motor(9, true), Motor(19)};

	master.clear();
	master.print(0, 0, "lemon is happi :D");

	for (const Wheel &wheel : wheels) {
		wheel.motor.set_brake_mode(E_MOTOR_BRAKE_COAST);
		wheel.motor.set_encoder_units(E_MOTOR_ENCODER_DEGREES);
		wheel.motor.set_gearing(E_MOTOR_GEARSET_18);
	}

	for (const Motor &motor : claws) {
		motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);
		motor.set_encoder_units(E_MOTOR_ENCODER_DEGREES);
		motor.set_gearing(E_MOTOR_GEARSET_06);
	}

	int autonomousHold = 0;
	int tareHold = 0;

	while (true) {
		double lx = master.get_analog(E_CONTROLLER_ANALOG_LEFT_X) / 127.0;
		double ly = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y) / 127.0;
		double rx = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_X) / 127.0;

		std::array<double, 4> rots{rx, rx, -rx, -rx};

		double a = lx + ly;
		double b = lx - ly;
		std::array<double, 4> badValue{a, b, b, a};

		for (int i = 0; i < wheels.size(); i++) {
			wheels[i].motor.move(badValue[i] * 127.0 + rots[i] * 64.0);
		}

		if (master.get_digital(E_CONTROLLER_DIGITAL_B)) {
			autonomousHold++;
			if (autonomousHold > 400.0) {
				autonomousHold = 0.0;
				autonomous();
			}
		} else {
			autonomousHold = 0.0;
		}

		delay(10);
	}
}
