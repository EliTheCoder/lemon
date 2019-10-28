#include "main.h"

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::Motor mtr4(1);
	pros::Motor mtr3(2);
	mtr3.set_reversed(true);
	pros::Motor mtr2(11);
	mtr2.set_reversed(true);
	pros::Motor mtr1(12);

	mtr1 = 255;
	mtr2 = -255;
	mtr3 = -255;
	mtr4 = 255;

	pros::delay(2000);

	mtr1 = -255;
	mtr2 = 255;
	mtr3 = 255;
	mtr4 = -255;

	pros::delay(2000);

	mtr1 = 0;
	mtr2 = 0;
	mtr3 = 0;
	mtr4 = 0;

	pros::delay(100);
}

int smooth(int x) {
	return x > 255 ? 255 : x^2/255;
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol()
{
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::Motor mtr4(2);
	mtr4.set_reversed(true);
	pros::Motor mtr3(1);
	pros::Motor mtr2(11);
	mtr2.set_reversed(true);
	pros::Motor mtr1(12);
	pros::Motor mtrWall(3);
	
	// turn speed
	int ts = 50;
	master.clear();

	while (true) {
		int lx = master.get_analog(ANALOG_LEFT_X);
		int ly = master.get_analog(ANALOG_LEFT_Y);
		int rx = master.get_analog(ANALOG_RIGHT_X);
		int ry = master.get_analog(ANALOG_RIGHT_Y);
		int by = master.get_digital(DIGITAL_Y);
		int ba = master.get_digital(DIGITAL_A);
		int bu = master.get_digital(DIGITAL_UP);
		int bd = master.get_digital(DIGITAL_DOWN);
		int bl1 = master.get_digital(DIGITAL_L1);
		int bl2 = master.get_digital(DIGITAL_L2);
		int br1 = master.get_digital(DIGITAL_R1);
		int br2 = master.get_digital(DIGITAL_R2);

		int a = smooth(lx + ly);
		int b = smooth(ly - lx);

		int i1 = a;
		int i2 = b;
		int i3 = b;
		int i4 = a;

		int j1 = rx;
		int j2 = -rx;
		int j3 = rx;
		int j4 = -rx;

		mtr1 = smooth(i1 + j1);
		mtr2 = smooth(i2 + j2);
		mtr3 = smooth(i3 + j3);
		mtr4 = smooth(i4 + j4);
		
		if (bl1) mtrWall = 255;
		else if (bl2) mtrWall = -255;
		else mtrWall = 0;

		if (bu) autonomous();

		pros::delay(20);
	}
}
