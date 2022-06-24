#ifndef _IDistance_H
#define _IDistance_H

#include "../pre-defs.h"
#include <string>

class IDistance
{
public:
	virtual dimtype get_dimension() const = 0;
	virtual distype measure(const coordtype*, const coordtype*) = 0;
	virtual bool IsBetweenZero(const coordtype*, const coordtype*) = 0;
	virtual distype measure_2(const coordtype*, const coordtype*) = 0;
	virtual largesttype_un counter() = 0;
	virtual void reset_counter() = 0;
	virtual void set_counter(largesttype_un) = 0;
	virtual std::string name()const = 0;
};

#endif
