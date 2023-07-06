#include <engine/math/math.hpp>
#include <engine/core/log.hpp>

#include <random>
#include <ctime>

namespace Math
{
	void randomise()
	{
		time_t t = time(nullptr);
		
		ASSERT(t != -1, "Failed to get current time.");

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
