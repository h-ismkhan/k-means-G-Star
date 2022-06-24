#ifndef _SSEDM_H
#define _SSEDM_H

#include <math.h>
#include "IEvaluate.h"

class SSEDM_Measure : public IEvaluate
{
protected:
	const IInstance_Coordinated *inst;
	IDistance *dis_metric = 0;
	const numclustype*clusid;
	sizetype size;
	dimtype dim;
public:	
	virtual void set_instance(const IInstance_Coordinated* inst)
	{
		this->inst = inst;
		this->size = inst->size();
		this->dim = inst->dimension();
	}
	virtual inline void set_distance_metric(IDistance* dis_metric) { this->dis_metric = dis_metric; }
	virtual void set_clusterIds(const numclustype* clusterIds)
	{
		clusid = clusterIds;
	}
	
	virtual distype measure() const
	{
		numclustype num_of_clusters = 0;
		for (sizetype si = 0; si < size; si++)
		{
			if (clusid[si] > num_of_clusters)
				num_of_clusters = clusid[si];
		}
		num_of_clusters++;

		vector<vector<coordtype>> ccenters;
		ccenters.assign(num_of_clusters, vector<coordtype>(dim));
		for (numclustype ci = 0; ci < num_of_clusters; ci++)
		{
			for (dimtype dj = 0; dj < dim; dj++)
				ccenters[ci][dj] = 0;
		}
		vector<sizetype>SizeOf(size, 0);
		for (sizetype si = 0; si < size; si++)
		{
			SizeOf[clusid[si]]++;
			numclustype c_id = clusid[si];
			const coordtype* row = (*inst)[si];
			for (dimtype dj = 0; dj < dim; dj++)
				ccenters[c_id][dj] = ccenters[c_id][dj] + row[dj];
		}
		for (numclustype ci = 0; ci < num_of_clusters; ci++)
		{
			if (SizeOf[ci] != 0)
			{
				for (dimtype dj = 0; dj < dim; dj++)
					ccenters[ci][dj] = ccenters[ci][dj] / SizeOf[ci];
			}
		}

		distype SSEDM = 0;
		for (sizetype si = 0; si < size; si++)
		{
			const coordtype *vec = (*inst)[si];
			distype dis = dis_metric->measure(vec, ccenters[clusid[si]].data());
			SSEDM = SSEDM + dis * dis;
		}
		return SSEDM;
	}
	
	virtual string name()const { return "SSEDM"; }
};

#endif