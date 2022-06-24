#ifndef _Inst_Addable_H
#define _Inst_Addable_H

#include "Inst-CoordpLine.h"
#include <assert.h>

class Inst_Addable : public Inst_CoordpLine
{
private:
public:
	Inst_Addable(){ this->_dim = 0; }
	Inst_Addable(dimtype dim) { this->_dim = dim; _size = 0; }
	void set_dimension(dimtype dim)
	{
		vectors.clear();
		this->_size = 0;
		assert(dim > 0);
		this->_dim = dim;
	}
	void add_extraColumn_back(const vector<coordtype>& coordinates)
	{
		vector<coordtype> nwpoint(_dim);
		for (sizetype si = 0; si < _size; si++)
			vectors[si].push_back(coordinates[si]);
		vectors.push_back(nwpoint);
		_dim++;
	}
	void add_back(const coordtype* point)
	{
		vector<coordtype> nwpoint(_dim);
		for (dimtype di = 0; di < _dim; di++)
			nwpoint[di] = point[di];
		vectors.push_back(nwpoint);
		_size = vectors.size();
	}
	void update_ij(sizetype i, dimtype j, coordtype val)
	{		
		vectors[i][j] = val;
	}
	void pop_end()
	{ 
		vectors.pop_back();
		_size = vectors.size();
	}
	void clear()
	{
		vectors.clear();
		this->_size = 0;
	}
	virtual string name() const{ return "addable-instance"; }
};

#endif
