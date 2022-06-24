#ifndef _Euclidean_Distance_H
#define _Euclidean_Distance_H

#include "../pre-defs.h"
#include "IDistance.h"
#include <math.h>
#include <assert.h>

class Euclidean_Distance : public IDistance
{
	largesttype_un _counter = 0;
	dimtype dimension;
public:
	virtual inline dimtype get_dimension() const{ return dimension; }
	Euclidean_Distance(){ this->dimension = 0; }
	Euclidean_Distance(dimtype dimension){ this->dimension = dimension; }
	inline void set_dimension(dimtype dim)
	{
		assert(this->dimension == 0);
		assert(dim > 0);
		this->dimension = dim;
	}
	virtual inline distype measure(const coordtype* x, const coordtype* y)
	{
		return sqrt(dis2(x, y));
	}
	virtual inline distype measure_2(const coordtype* x, const coordtype* y)
	{
		return dis2(x, y);
	}
	virtual bool IsBetweenZero(const coordtype* x, const coordtype* y)
	{
		_counter++;
		distype dis2 = 0;
		for (dimtype i = 0; i < dimension; i++)
		{
			distype xydiff = x[i] - y[i];
			if (xydiff > 0.000000000001)
				return false;
			if (xydiff < -0.000000000001)
				return false;
		}
		return true;
	}
	virtual inline distype dis2(const coordtype* x, const coordtype* y)
	{
		_counter++;
		distype dis2 = 0;
		for (dimtype i = 0; i < dimension; i++)
		{
			distype xydiff = x[i] - y[i];
			dis2 = dis2 + xydiff * xydiff;
		}
		return dis2;
	}
	virtual inline largesttype_un counter(){ return _counter; }
	virtual inline void reset_counter() { _counter = 0; }
	void set_counter(largesttype_un counter){ _counter = counter; }
	virtual string name()const { return "(Euclidean)"; }
};

#endif
