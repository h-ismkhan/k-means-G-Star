#ifndef _Seeds_Random_Init_H
#define _Seeds_Random_Init_H

#include "../../Utils/myRandom.h"
#include "../../Utils/myString.h"
#include "ISeed-Initializer.h"
#include <algorithm>

class Seeds_Random_Init : public ISeed_Initializer
{
private:
	numclustype the_k;
	sizetype size;
	const IInstance_Coordinated* inst = 0;
	vector<sizetype> solution;
	vector<const coordtype*> seeds_coordinates;
	void make_solution()
	{
		vector<sizetype> nodes(size);
		makeVectorRandom_0toSize<sizetype>(nodes);

		solution.assign(the_k, -1);
		for (numclustype i = 0; i < the_k; i++)
		{
			solution[i] = nodes[i];
		}
		sort(solution.begin(), solution.end());
	}
public:
	Seeds_Random_Init(){}
	Seeds_Random_Init(sizetype inst_size){ this->size = inst_size; }
	virtual void set_the_k(numclustype the_k) { this->the_k = the_k;  }
	virtual void set_instance(const IInstance_Coordinated* inst) { this->inst = inst; this->size = inst->size(); }
	virtual const vector<const coordtype*>& next_seeds()
	{
		make_solution();
		if (seeds_coordinates.size() != the_k)
			seeds_coordinates.assign(the_k, 0);
		if (inst != 0)
		{
			for (numclustype i = 0; i < the_k; i++)
				seeds_coordinates[i] = (*inst)[solution[i]];
		}
		return seeds_coordinates;
	}	
	virtual const sizetype* seeds_indexes_in_instance() { make_solution(); return solution.data(); }

	virtual const IInstance_Coordinated *get_inst() { return this->inst; }
	virtual numclustype get_the_k() { return this->the_k; }

	virtual string name() { return "random"; }
};

#endif