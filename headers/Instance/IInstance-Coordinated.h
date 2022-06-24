#ifndef _IInstance_Coordinated_H
#define _IInstance_Coordinated_H

#include "../pre-defs.h"
#include <fstream>


#include  "../Utils/myString.h"

class IInstance_Coordinated
{
public:
	virtual dimtype dimension() const = 0;
	virtual sizetype size() const = 0;
	virtual const coordtype* operator[](sizetype index) const = 0;
	virtual numclustype get_actual_clusters_num() const = 0;
	virtual numclustype get_actual_cid_of(sizetype index) const = 0;

	static pair<sizetype, dimtype> read_size_dim(string path) 
	{
		ifstream in(path);
		sizetype size;
		dimtype dim;
		in >> size;
		in >> dim;
		return make_pair(size, dim);
	}
	static string name(string path)	{ return remove_extension(extract_name(path)); }

	virtual string name() const = 0;

#ifdef inst_del
	virtual void clear(sizetype point) const = 0;
	virtual void clearAll() const = 0;
#endif

};

#endif
