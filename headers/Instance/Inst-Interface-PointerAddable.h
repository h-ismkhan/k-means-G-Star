#ifndef _Inst_Interface_PointerAddable_H
#define _Inst_Interface_PointerAddable_H

#include "IInstance-Coordinated.h"
#include <vector>

class Inst_AddablePointerOnly : public Instance_Coordinated
{
private:
	vector<const coordtype*> pointers;
	sizetype _size;
	dimtype _dim;
public:
	Inst_AddablePointerOnly(){ this->_dim = 0; _size = 0; actualK = 0; }
	Inst_AddablePointerOnly(dimtype dim) { this->_dim = dim; _size = 0; actualK = 0; }
	void add_back(const coordtype* point)
	{
		pointers.push_back(point);
		_size = pointers.size();
	}
	void pop_end(){ pointers.pop_back(); _size = pointers.size(); }

	virtual numclustype get_actual_clusters_num() const { return (numclustype)-1; }
	inline virtual sizetype size() const { return _size; }
	inline virtual dimtype dimension() const { return _dim; }
	virtual const coordtype* operator[](sizetype index) const  {return pointers[index]; }

	virtual string name() const{ return "no-path"; }
};

#endif
