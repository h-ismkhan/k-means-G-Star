#ifndef _ISeed_Initializer_H
#define _ISeed_Initializer_H

#include <vector>
#include "../../Instance/IInstance-Coordinated.h"

class ISeed_Initializer
{
public:
	virtual void set_the_k(numclustype) = 0;
	virtual void set_instance(const IInstance_Coordinated *) = 0;

	virtual const IInstance_Coordinated *get_inst() = 0;
	virtual numclustype get_the_k() = 0;

	virtual const vector<const coordtype*>& next_seeds() = 0;
	virtual const sizetype* seeds_indexes_in_instance() { return 0; }

	virtual string name() = 0;
};

#endif