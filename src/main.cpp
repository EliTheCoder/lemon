#include "main.h"
using namespace pros;

void initialize() {}
void disabled() {
	Controller master(E_CONTROLLER_MASTER);
	master.clear();
	master.rumble("...");
	master.print(0, 0, "lemon is angy");
	master.print(1, 0, "lemon can't move >:(");
}
void competition_initialize() {}
void autonomous() {}

void opcontrol() {
	Controller master(E_CONTROLLER_MASTER);

	class Wheel {
	  public:
		Motor motor;
		double angle;
		Wheel() : motor(1), angle(0) {}
		Wheel(Motor motor, double a) : motor(motor) { this->angle = a; };
		double getValue(double angle1, double magnitude) {
			double diff = fmod((angle1 - angle + 180.0), 360.0) - 180.0;
			double angleDist = diff < -180.0 ? diff + 360.0 : diff;
			angleDist = angleDist / -180.0 + 1.0;
			double returnVal = magnitude * angleDist;
			returnVal = returnVal > 1.0 ? 1.0 : returnVal;
			returnVal = returnVal < -1.0 ? -1.0 : returnVal;
			return returnVal;
		};
		Wheel operator=(Wheel other) { return Wheel(other.motor, other.angle); }
	};

	Wheel wheels[4];
	int wheelPorts[4] = {
		8,	// front-left
		16, // front-right
		1,	// back-left
		9	// back-right
	};
	double wheelAngles[4] = {315.0, 225.0, 225.0, 315.0};
	for (int i = 0; i < sizeof(wheels) / sizeof(*wheels); i++) {
		wheels[i] = Wheel(Motor(wheelPorts[i]), wheelAngles[i]);
	}

	const Motor lclaw(7);
	lclaw.set_reversed(true);
	const Motor rclaw(15);
	const Motor claws[2] = {lclaw, rclaw};

	master.clear();
	master.print(0, 0, "lemon is happi :D");

	// set all motors to brake mode of hold
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

	double lx = master.get_analog(ANALOG_LEFT_X);
	double ly = master.get_analog(ANALOG_LEFT_Y);
	double rx = master.get_analog(ANALOG_RIGHT_X);
	double ry = master.get_analog(ANALOG_RIGHT_Y);
	int by = master.get_digital(DIGITAL_Y);
	int ba = master.get_digital(DIGITAL_A);
	int bb = master.get_digital(DIGITAL_B);
	int bx = master.get_digital(DIGITAL_X);
	int bu = master.get_digital(DIGITAL_UP);
	int bd = master.get_digital(DIGITAL_DOWN);
	int bl = master.get_digital(DIGITAL_LEFT);
	int br = master.get_digital(DIGITAL_RIGHT);
	int bl1 = master.get_digital(DIGITAL_L1);
	int bl2 = master.get_digital(DIGITAL_L2);
	int br1 = master.get_digital(DIGITAL_R1);
	int br2 = master.get_digital(DIGITAL_R2);

	int autonomousHold = 0;
	int tareHold = 0;

	while (true) {
		double movementAngle = atan(ly / lx) * 180.0 / M_PI;
		double movementMagnitude = sqrt(lx * lx + ly * ly);

		double rots[4] = {rx, -rx, rx, -rx};

		for (int i = 0; i < sizeof(wheels) / sizeof(*wheels); i++) {
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

		if (bb) {
			autonomousHold++;
			if (autonomousHold > 400.0) {
				autonomousHold = 0.0;
				autonomous();
			}
		} else {
			autonomousHold = 0.0;
		}

		if (by) {
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

		wheels[0].angle = claws[0].get_position() + 315.0;
		wheels[1].angle = claws[1].get_position() + 225.0;

		int insideButtons[] = {bl1, br1};
		int outsideButtons[] = {bl2, br2};

		for (int i = 0; i < sizeof(insideButtons) / sizeof(*insideButtons);
			 i++) {
			if (!insideButtons[i] && !outsideButtons[i]) {
				claws[i].move_relative(0.0, 0);
			}
			if (insideButtons[i]) {
				claws[i].move_relative(25.0, 100.0);
			}
			if (outsideButtons[i]) {
				claws[i].move_relative(45.0, 100.0);
			}
			if (insideButtons[i] && outsideButtons[i]) {
				claws[i].move_relative(0.0, 0.0);
			}
		}
	}

	delay(10);
}
