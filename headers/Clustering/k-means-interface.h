#ifndef _k_means_interface_H
#define _k_means_interface_H

#include "IClustering.h"
#include "k-means-initializers/ISeed-Initializer.h"

class k_means_interface : public IClustering
{	
public:
	virtual inline void set_max_num_iteration(size_t ) = 0;
	
	virtual void set_seeds_initializer(ISeed_Initializer *) = 0; 

	virtual size_t get_consumed_iteration_count() const = 0;
	virtual const coordtype* get_starting_initial_centers(numclustype) const = 0;
	virtual vector<string> my_comments() const 
	{
		vector<string> out(1, "");
		return out;
	}
};

#endif
