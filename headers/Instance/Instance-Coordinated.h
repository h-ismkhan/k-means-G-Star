#ifndef _Instance_Coordinated_H
#define _Instance_Coordinated_H

#include "IInstance-Coordinated.h"
#include <fstream>

#include <vector>
#include <string>
using namespace std;

class Instance_Coordinated : public IInstance_Coordinated
{
protected:
	sizetype _size;
	dimtype _dim;
	numclustype actualK = 0;
	mutable vector<vector<coordtype>> vectors;
	mutable vector<numclustype> actualClusIds;
public:
	inline virtual sizetype size() const
	{
		return _size;
	}
	inline virtual dimtype dimension() const
	{
		return _dim;
	}
	inline virtual const coordtype* operator[](sizetype node) const
	{
		return vectors[node].data();
	}

	virtual numclustype get_actual_clusters_num() const{ return this->actualK; }
	virtual numclustype get_actual_cid_of(sizetype index) const{ return this->actualClusIds[index]; }

#ifdef inst_del
	virtual void clear(sizetype point) const 
	{
		vectors[point].clear();
	}	
	virtual void clearAll() const
	{
		vectors.clear();
		actualClusIds.clear();
	}
#endif
};

#endif
