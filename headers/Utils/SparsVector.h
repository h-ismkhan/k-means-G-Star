#ifndef _SparsVector_H
#define _SparsVector_H

template<class type>
class SparsVector
{
private:
	type *arr = 0;
	mutable vector<size_t> addedIndexes;
	mutable vector<size_t> positions_in_addedIndexes;
	type gen_val;
	mutable size_t last_index = 0;
	type last_value;
	size_t size;
	size_t minus_1_equalent = -1;

	inline void drop_index_from_positions_and_addedIndexes(size_t index)const
	{
		if (positions_in_addedIndexes[index] == minus_1_equalent)
			return;
		if (positions_in_addedIndexes[index] == addedIndexes.size() - 1)
		{
			positions_in_addedIndexes[index] = minus_1_equalent;
			addedIndexes.pop_back();
			return;
		}

		positions_in_addedIndexes[addedIndexes[addedIndexes.size() - 1]] = positions_in_addedIndexes[index];
		addedIndexes[positions_in_addedIndexes[index]] = addedIndexes[addedIndexes.size() - 1];
		addedIndexes.pop_back();
		positions_in_addedIndexes[index] = minus_1_equalent;
	}
	inline void applyIndex(size_t index)const
	{
		if (arr[index] == gen_val)
		{
			drop_index_from_positions_and_addedIndexes(index);
			return;
		}
		if (arr[index] != gen_val)
		{
			if (positions_in_addedIndexes[index] != minus_1_equalent)
				return;

			addedIndexes.push_back(index);
			positions_in_addedIndexes[index] = addedIndexes.size() - 1;
		}
	}

	inline void set0(size_t index, type val)
	{
		if (arr[index] == gen_val && val != gen_val)
		{
			addedIndexes.push_back(index);
			positions_in_addedIndexes[index] = addedIndexes.size() - 1;

			arr[index] = val;
			return;
		}
		if (arr[index] == gen_val && val == gen_val)
		{
			return;
		}
		if (arr[index] != gen_val && val != gen_val)
		{
			arr[index] = val;
			return;
		}
		if (arr[index] != gen_val && val == gen_val)
		{
			if (positions_in_addedIndexes[index] == addedIndexes.size() - 1)
			{
				arr[index] = val;
				positions_in_addedIndexes[index] = minus_1_equalent;
				addedIndexes.pop_back();
				return;
			}

			positions_in_addedIndexes[addedIndexes[addedIndexes.size() - 1]] = positions_in_addedIndexes[index];
			addedIndexes[positions_in_addedIndexes[index]] = addedIndexes[addedIndexes.size() - 1];
			addedIndexes.pop_back();
			positions_in_addedIndexes[index] = minus_1_equalent;
			arr[index] = val;
			return;
		}
	}
public:
	~SparsVector()
	{
		if (arr)
		{
			arr = 0;
			delete arr;
		}
	}
	SparsVector(){}
	SparsVector(size_t size, type general_val)
	{
		assign(size, general_val);
	}
	void assign(size_t size, type general_val)
	{
		last_index = minus_1_equalent;

		/*if (arr.size() != size)
		arr.assign(size, general_val);*/
		if (arr == 0)
			delete arr;
		arr = new type[size];
		for (size_t i = 0; i < size; i++)
			arr[i] = general_val;

		this->size = size;
		this->gen_val = general_val;

		positions_in_addedIndexes.assign(size, minus_1_equalent);
		addedIndexes.clear();
	}

	inline type& operator[](size_t index) const
	{
		if (!(index < size && index != minus_1_equalent))
		{
			cout << "index is: " << index << ", but it should be smaller than " << size << endl;
			assert(index < size && index != minus_1_equalent);
		}

		if (last_index != minus_1_equalent)
			applyIndex(last_index);

		//last_value = arr[index];
		last_index = index;

		/*return last_value;*/

		return arr[index];
	}
	void reset()
	{
		if (!arr)
			return;
		if (last_index != minus_1_equalent)
			applyIndex(last_index);
		/*if (last_value != arr[last_index])
		this->set(last_index, last_value);*/
		last_index = minus_1_equalent;

		for (size_t i = 0; i < addedIndexes.size(); i++)
		{
			arr[addedIndexes[i]] = this->gen_val;
			positions_in_addedIndexes[addedIndexes[i]] = minus_1_equalent;
		}
		addedIndexes.clear();
	}
	size_t length() const
	{
		return size;
	}
	size_t countActive()const
	{
		if (last_index != minus_1_equalent)
			applyIndex(last_index);
		/*if (last_value != arr[last_index])
		this->set(last_index, last_value);*/
		return addedIndexes.size();
	}
	const vector<size_t>& getActiveIndexes()const
	{
		if (last_index != minus_1_equalent)
			applyIndex(last_index);
		/*if (last_value != arr[last_index])
		this->set(last_index, last_value);*/
		return addedIndexes;
	}
};

#endif