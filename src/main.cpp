#include "main.h"
using namespace pros;

void initialize() {
	Controller master(E_CONTROLLER_MASTER);
	lcd::initialize();
	opcontrol();
}
void disabled() {
	Controller master(E_CONTROLLER_MASTER);
	master.clear();
	master.rumble("_..");
	master.print(0, 0, "lemon is angy");
	master.print(1, 0, "lemon can't move >:(");
}
void competition_initialize() {
	Controller master(E_CONTROLLER_MASTER);
	master.rumble(".-.-");
	opcontrol();
}
void autonomous() {}

void opcontrol() {
	lcd::print(0, 0, "opcontrol");
	Controller master(E_CONTROLLER_MASTER);

	class Wheel {
	  public:
		Motor motor;
		double angle;
		Wheel(Motor motor, double a) : motor(motor), angle(a){};
		double getValue(double angle1, double magnitude) {
			double diff = fmod((angle1 - angle + 180.0), 360.0) - 180.0;
			double angleDist = diff < -180.0 ? diff + 360.0 : diff;
			angleDist = angleDist / -180.0 + 1.0;
			double returnVal = magnitude * angleDist;
			returnVal = returnVal > 1.0 ? 1.0 : returnVal;
			returnVal = returnVal < -1.0 ? -1.0 : returnVal;
			return returnVal;
		};
		double setAngle(double angle1) {
			double newAngle = fmod(angle1, 360.0);
			this->angle = newAngle < 0.0 ? newAngle + 360.0 : newAngle;
			return this->angle;
		}
	};

	std::array<Wheel, 4> wheels{
		Wheel{Motor{10}, 315.0},
		Wheel{Motor{20}, 225.0},
		Wheel{Motor{1}, 225.0},
		Wheel{Motor{11}, 315.0},
	};

	std::array<Motor, 2> claws{Motor(9, true), Motor(19)};

	master.clear();
	master.print(0, 0, "lemon is happi :D");

	for (const Wheel &wheel : wheels) {
		wheel.motor.set_brake_mode(E_MOTOR_BRAKE_BRAKE);
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
		double lx = master.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
		double ly = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
		double rx = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);

		double movementAngle = atan(ly / lx) * 180.0 / M_PI;
		double movementMagnitude = sqrt(lx * lx + ly * ly);

		double rots[4] = {rx, -rx, rx, -rx};

		for (int i = 0; i < wheels.size(); i++) {
			float value =
				(wheels[i].getValue(movementAngle, movementMagnitude) +
				 rots[i]) /
				2.0;
			value = value > 1.0 ? 1.0 : value;
			value = value < -1.0 ? -1.0 : value;
			if (abs(value) > 0.001) {
				wheels[i].motor.move_relative(signbit(value) * 45.0, value);
			} else {
				wheels[i].motor.move_relative(0.0, 0.0);
			}
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

		if (master.get_digital(E_CONTROLLER_DIGITAL_Y)) {
			tareHold++;
			if (tareHold > 100.0) {
				tareHold = 0.0;
				for (const Motor &motor : claws) {
					motor.tare_position();
				}
			}
		} else {
			tareHold = 0;
		}

		if (master.get_digital(E_CONTROLLER_DIGITAL_X)) {
			for (const Motor &motor : claws) {
				motor.move_absolute(0.0, 100.0);
			}
		}

		if (master.get_digital(E_CONTROLLER_DIGITAL_A)) {
			for (const Wheel &wheel : wheels) {
				wheel.motor.set_brake_mode(E_MOTOR_BRAKE_HOLD);
				wheel.motor.move_relative(0.0, 0.0);
			}
		} else {
			for (const Wheel &wheel : wheels) {
				wheel.motor.set_brake_mode(E_MOTOR_BRAKE_COAST);
			}
		}

		wheels[0].setAngle(claws[0].get_position() + 315.0);
		wheels[1].setAngle(claws[1].get_position() + 225.0);

		int insideButtons[2] = {master.get_digital(E_CONTROLLER_DIGITAL_L1),
								master.get_digital(E_CONTROLLER_DIGITAL_R1)};
		int outsideButtons[2] = {master.get_digital(E_CONTROLLER_DIGITAL_R2),
								 master.get_digital(E_CONTROLLER_DIGITAL_L2)};

		for (int i = 0; i < sizeof(insideButtons) / sizeof(*insideButtons);
			 i++) {
			if (!insideButtons[i] && !outsideButtons[i]) {
				wheels[i].motor.set_brake_mode(E_MOTOR_BRAKE_COAST);
				claws[i].move_absolute(0.0, 0);
			}
			if (insideButtons[i]) {
				wheels[i].motor.set_brake_mode(E_MOTOR_BRAKE_COAST);
				claws[i].move_absolute(25.0, 100.0);
			} else {
			}
			if (outsideButtons[i]) {
				wheels[i].motor.set_brake_mode(E_MOTOR_BRAKE_COAST);
				claws[i].move_absolute(45.0, 100.0);
			}
			if (insideButtons[i] && outsideButtons[i]) {
				claws[i].move_absolute(0.0, 0.0);
			}
		}
	}

	delay(10);
}
