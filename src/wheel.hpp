#include "main.h"
using namespace pros;

double clamp(double value, double min, double max) {
	return std::max(min, std::min(value, max));
}

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
		return clamp(returnVal, -1.0, 1.0);
	};
	double setAngle(double angle1) {
		double newAngle = fmod(angle1, 360.0);
		this->angle = newAngle < 0.0 ? newAngle + 360.0 : newAngle;
		return this->angle;
	}
};