#ifndef _Inst_CoordpLine_H
#define _Inst_CoordpLine_H

#include <vector>

#include <sstream>
#include <string>
using namespace std;

#include "Instance-Coordinated.h"

class Inst_CoordpLine : public Instance_Coordinated
{
protected:
	string _path;
protected:	
public:
	Inst_CoordpLine(){}
	Inst_CoordpLine(const string path)	{ ReadfromFile(path); }
protected:
	virtual void ReadfromFile(const string path)
	{
		this->_path = path;
		ifstream _in(path);
		string str_cl((istreambuf_iterator<char>(_in)),
			istreambuf_iterator<char>());
		_in.close();

		istringstream in_sstr(str_cl);

		in_sstr >> _size;
		in_sstr >> _dim;

		vectors.assign(_size, vector<coordtype>(_dim));

		for (sizetype i = 0; i < _size; i++)
		{
			for (dimtype j = 0; j < _dim; j++)
				in_sstr >> vectors[i][j];
		}
		if (!in_sstr.eof())
		{
			numclustype minK = 1, maxK = 0;
			this->actualK = 0;
			actualClusIds.assign(_size, 0);
			for (sizetype j = 0; j < _size; j++)
			{
				if (in_sstr.eof())
				{
					this->actualK = 0;
					maxK = 0;
					minK = 0;
					break;
				}
				try
				{
					in_sstr >> actualClusIds[j];
					if (actualClusIds[j] > maxK)
						maxK = actualClusIds[j];
					if (actualClusIds[j] < minK)
						minK = actualClusIds[j];
				}
				catch (exception e)
				{
					maxK = 0;
					minK = 0;
					break;
				}
			}
			if (minK == 0)
				actualK = maxK + 1;
			else
				actualK = maxK;
		}
		//UpdateStatistics();
	}
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

	virtual string name() const{ return remove_extension(extract_name(_path)); }
	virtual string path() const{ return _path; }
};

#endif