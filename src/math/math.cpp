#include "math.h"
#include <random>
#include <ctime>
#include <assert.h>

namespace Math
{
	void randomise()
	{
		time_t t = time(nullptr);
		
		assert(t != -1);

		srand(static_cast<unsigned int>(t));
	}

	void set_seed(unsigned int seed)
	{
		srand(seed);
	}

	double random()
	{
		return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
	}

	double random(double low, double high)
	{
		return static_cast<double>(rand()) / static_cast<double>(RAND_MAX) * (high-low) + low;
	}

	int randomi()
	{
		return rand();
	}

	int randomi(int low, int high)
	{
		return rand() % (high - low) + low;
	}
}
