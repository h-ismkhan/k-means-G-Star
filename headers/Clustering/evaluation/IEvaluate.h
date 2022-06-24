#ifndef _IEvaluate_H
#define _IEvaluate_H

#include "../../Instance/IInstance-Coordinated.h"

class IEvaluate
{
public:
	virtual void set_instance(const IInstance_Coordinated*) = 0;
	virtual void set_clusterIds(const numclustype*) = 0;
	virtual inline void set_distance_metric(IDistance*) = 0;

	virtual distype measure() const = 0;

	virtual string name()const = 0;
};

#endif