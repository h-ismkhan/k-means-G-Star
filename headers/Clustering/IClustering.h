#ifndef _IClustering_H
#define _IClustering_H

#include "../pre-defs.h"
#include "../Distance/IDistance.h"
#include "../Instance/IInstance-Coordinated.h"
#include <vector>

class IClustering
{
public:
	virtual const vector<numclustype>& get_results() const = 0;
	virtual inline numclustype get_num_of_clusters() const = 0;
	virtual void set_parameters_atomatically() {}

	virtual inline void set_instance(const IInstance_Coordinated *) = 0;
	virtual void set_num_of_clusters(numclustype ) = 0;
	virtual inline void set_distance_metric(IDistance*) = 0;

	virtual const vector<numclustype>& apply() = 0;
	virtual string name()const = 0;
};

#endif