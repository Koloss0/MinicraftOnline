#pragma once

#include <random>

namespace Math
{
	void randomise();
	void set_seed(unsigned int seed);

	double random();
	double random(double low, double high);
	int randomi();
	int randomi(int low, int high);
}
