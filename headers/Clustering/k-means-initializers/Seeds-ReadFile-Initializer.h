#ifndef _Seeds_ReadFile_Init_H
#define _Seeds_ReadFile_Init_H

#include <sstream>
#include <string>
using namespace std;

#include "Seeds-Random-Initializer.h"
#include "../../Utils/myString.h"

class Seeds_ReadFile_Init : public Seeds_Random_Init
{
private:
	const IInstance_Coordinated* inst;
	numclustype the_k;
	sizetype size;
	
	vector<sizetype> solution;
	vector<sizetype> all_from_file;
	size_t first_index = 0;
	size_t maxrun;
	
	vector<const coordtype*> seeds_coordinates;

	string my_name = "random";
	void make_solution()
	{
		solution.assign(the_k, -1);
		for (numclustype i = 0; i < the_k; i++)
		{
			solution[i] = all_from_file[i + first_index];
		}
		first_index = (first_index + the_k) % (the_k * maxrun);
		sort(solution.begin(), solution.end());
	}
public:
	virtual void set_the_k(numclustype the_k) { this->the_k = the_k; }
	virtual void set_instance(const IInstance_Coordinated* inst) { this->inst = inst; this->size = inst->size(); }
	virtual bool load_ifcan_path_run_thek(string instpath, size_t maxrun, numclustype the_k)
	{
		this->maxrun = maxrun;

		string FileName = remove_extension(extract_name(instpath));
		string centers_path_1 = "_initial-centers";
		string centers_path_2 = centers_path_1 + folder_sperator;
		
		string centers_path = centers_path_2 + "initC."
			+ std::to_string(maxrun) + "." + std::to_string(the_k) + "." + FileName + ".txt";
		ifstream _in(centers_path);
		if (_in.good())
		{
			//cout << "Ok! Initial centers.\n";

			string str_cl((istreambuf_iterator<char>(_in)),
				istreambuf_iterator<char>());
			_in.close();

			istringstream in_sstr(str_cl);
			while (!in_sstr.eof())
			{
				sizetype point;
				in_sstr >> point;
				all_from_file.push_back(point);
			}
			if (all_from_file.size() < maxrun * the_k)
			{
				all_from_file.clear();
				return false;
			}

			return true;
		}
		else
		{
			cout << "Initial centers do not exist in file.\n";
			cout << "the centers path is clear now!" << endl;
			_in.close();
			return false;
		}
	}
	virtual const vector<const coordtype*>& next_seeds()
	{
		make_solution();
		if (seeds_coordinates.size() != the_k)
			seeds_coordinates.assign(the_k, 0);
		for (numclustype i = 0; i < the_k; i++)
			seeds_coordinates[i] = (*inst)[solution[i]];
		return seeds_coordinates;
	}	
	virtual const sizetype* seeds_indexes_in_instance() { make_solution(); return solution.data(); }
	
	virtual const IInstance_Coordinated *get_inst() { return this->inst; }
	virtual numclustype get_the_k() { return this->the_k; }

	static string get_standard_path_initC(string instpath, numclustype the_k, size_t maxrun)
	{
		string FileName = remove_extension(extract_name(instpath));
		string centers_path_1 = "_initial-centers";
		string centers_path_2 = centers_path_1 + folder_sperator;

		string centers_path = centers_path_2 + "initC."
			+ std::to_string(maxrun) + "." + std::to_string(the_k) + "." + FileName + ".txt";

		return centers_path;
	}

	virtual string name() { return my_name; }
};

#endif