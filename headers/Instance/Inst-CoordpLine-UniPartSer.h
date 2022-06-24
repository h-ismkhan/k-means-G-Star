#ifndef _Inst_CoordpLine_UniPartSer_H
#define _Inst_CoordpLine_UniPartSer_H

#include "Inst-CoordpLine.h"
#include <typeinfo>

class Inst_CoordpLine_UniPartSer : public Inst_CoordpLine
{
private:
	inline bool exists_file(const std::string& name) {
		ifstream f(name.c_str());
		bool ret = f.good();
		f.close();
		return ret;
	}
public:
	Inst_CoordpLine_UniPartSer(const string path){ ReadfromFile(path); }
protected:
	virtual void ReadfromFile(const string path)
	{
		this->_path = path;
		string clfile_name = extract_name(path);
		string abs_clfile_n = remove_extension(clfile_name);
		
		int sys_result = system("mkdir _my-pcl-files");
		if(sys_result < 0) cout << "";
		string path_upser = "_my-pcl-files/" + abs_clfile_n + "." + typeid(coordtype).name() + "._cl_upser";

		if (!exists_file(path_upser))
		{
			Inst_CoordpLine::ReadfromFile(path);			

			fstream file(path_upser.data(), fstream::out);
			file.close();
			
			vector <largesttype> inst_size_inf;
			inst_size_inf.push_back(_size);
			inst_size_inf.push_back(_dim);
			if (actualK > 0)
				inst_size_inf.push_back(actualK);
			else inst_size_inf.push_back(-1);

			vector<coordtype> linear_vector;
			for (sizetype i = 0; i < _size; i++)
			{
				for (dimtype j = 0; j < _dim; j++)
					linear_vector.push_back(vectors[i][j]);
			}
			if (linear_vector.size() > 0)
			{
				fstream file;
				file.open(path_upser.data(), fstream::in | fstream::out | fstream::binary);
				file.write((char*)inst_size_inf.data(), inst_size_inf.size() * sizeof(largesttype));
				file.write((char*)linear_vector.data(), linear_vector.size() * sizeof(coordtype));
				linear_vector.clear();	
				if (actualK > 0)
					file.write((char*)actualClusIds.data(), actualClusIds.size() * sizeof(numclustype));
				file.close();
			}		
		}
		else
		{
			fstream partloadfile(path_upser.data(), ios::in | ios::binary);

			largesttype* inst_size_inf = new largesttype[3];
			partloadfile.read((char*)inst_size_inf, 3 * sizeof(largesttype));
			_size = (sizetype)inst_size_inf[0];
			_dim = (dimtype)inst_size_inf[1];
			actualK = (numclustype)inst_size_inf[2];
			delete inst_size_inf;

			coordtype *linear_vector = new coordtype[_size * _dim];
			partloadfile.read((char*)linear_vector, _size * _dim * sizeof(coordtype));

			numclustype* actualClusIds_fromFile = 0;
			if (actualK > 0)
			{
				actualClusIds_fromFile = new numclustype[_size];
				partloadfile.read((char*)actualClusIds_fromFile, _size * sizeof(numclustype));
				actualClusIds.clear();
				for (sizetype i = 0; i < _size; i++)
					actualClusIds.push_back(actualClusIds_fromFile[i]);
				delete actualClusIds_fromFile;
			}
			partloadfile.close();

			vectors.clear();
			sizetype  c = 0;
			for (sizetype i = 0; c < _size * _dim; i++)
			{
				vector<coordtype>vec_inst(_dim, 0);
				for (dimtype j = 0; j < _dim; j++)
				{
					vec_inst[j] = linear_vector[c];
					c++;
				}
				vectors.push_back(vec_inst);
			}
			delete linear_vector;
		}
		
	}
};

#endif
