#ifndef _MY_RANDOM_H
#define _MY_RANDOM_H

#include <stdlib.h>
#include <math.h>

unsigned long long pow_arr[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000,
10000000000, 100000000000, 1000000000000, 10000000000000, 100000000000000, 1000000000000000, 10000000000000000,
100000000000000000, 1000000000000000000 };

#define num_digit9 18
#define myRAND_MAX pow_arr[num_digit9] - 1
unsigned long long myrand()
{
	unsigned int digits[num_digit9];

	for (int i = 0; i < num_digit9; i++)
	{
		digits[i] = rand() % 10;
	}
	unsigned long long randnumber = 0;
	for (int i = 0; i < num_digit9; i++)
	{
		unsigned long long power = pow_arr[i];

		//if (power % 2 != 0 && power != 1)     //eliminates "bug" (which comes from long long power is not a float))
		//	power++;

		randnumber += power * digits[i];
	}
	return randnumber;
}

long long Random(long long a, long long b)
{
	return (myrand() % (b - a + 1)) + a;
}

int Random(int a, int b)
{
	return (myrand() % (b - a + 1)) + a;
}

size_t Random(size_t a, size_t b)
{
	return (myrand() % (b - a + 1)) + a;
}

#include <vector>
// changes places of original contents, randomly.
template <class type>
void disperseVectorRandomly(vector<type> &output)
{
	size_t Dimension = output.size();
	for (size_t i = 0; i < Dimension; i++)
	{
		size_t index = Random((size_t)0, Dimension - 1);
		type temp = output[i];
		output[i] = output[index];
		output[index] = temp;
	}
}

// makes contents 0 to size(output) - 1
template <class type>
void makeVectorRandom_0toSize(vector<type> &output)
{
	size_t dimension = output.size();
	output.assign(dimension, 0);
	for (size_t i = 0; i < dimension; i++)
		output[i] = i;
	disperseVectorRandomly(output);
}

// makes contents 0 to dimension - 1
template <class type>
void makeVectorRandom_0toInputDim(vector<type> &output, size_t dimension)
{
	output.assign(dimension, 0);
	for (size_t i = 0; i < dimension; i++)
		output[i] = i;
	disperseVectorRandomly(output);
}

double Random01()
{
	double x = (double)myrand();
	double y = (double)myRAND_MAX;
	return x / y;
}
double Random(double min, double max)
{
	return min + (Random01() * (max - min));
}
double RandomGaussian(){
	// Use Box-Muller transform to generate a point from normal
	// distribution.
	double x1, x2;
	do{
		x1 = Random01();
	} while (x1 == 0); // cannot take log of 0.
	x2 = Random01();
	double z;
	z = sqrt(-2.0 * log(x1)) * cos(2.0 * 3.14 * x2);
	return z;
}

#endif
