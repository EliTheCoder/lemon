#include "main.h"
using namespace pros;
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
	Controller master(E_CONTROLLER_MASTER);
	Motor mtr1(10);
	Motor mtr2(19);
	mtr2.set_reversed(true);
	Motor mtr3(1);
	Motor mtr4(11);
	mtr4.set_reversed(true);

	mtr1 = 255;
	mtr2 = -255;
	mtr3 = -255;
	mtr4 = 255;
	delay(800);

	mtr1 = -255;
	mtr2 = 255;
	mtr3 = 255;
	mtr4 = -255;
	delay(500);

	mtr1 = 0;
	mtr2 = 0;
	mtr3 = 0;
	mtr4 = 0;
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
	Controller master(E_CONTROLLER_MASTER);
	Motor mtr1(10); // 
	Motor mtr2(19); // 
	mtr2.set_reversed(true);
	Motor mtr3(1); // 
	Motor mtr4(11); // 
	mtr4.set_reversed(true);
	Motor mtr5(9); // intake
	mtr5.set_reversed(true);
	Motor mtr6(17); // intake
	Motor mtr7(18); // arm
	mtr7.set_reversed(true);

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
		int bl = master.get_digital(DIGITAL_LEFT);
		int br = master.get_digital(DIGITAL_RIGHT);
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

		if (bu && bd && bl && br) autonomous();
		
		if (br1) {
			mtr5 = 255;
			mtr6 = 255;
		}
		else if (br2) {
			mtr5 = -255;
			mtr6 = -255;
		}
		else {
			mtr5 = 0;
			mtr6 = 0;
		}


		mtr7 = ry/2;

		delay(20);
	}
}
