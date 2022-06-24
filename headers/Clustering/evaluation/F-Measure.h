#ifndef _F_Measure_H
#define _F_Measure_H

#include "../../Instance/IInstance-Coordinated.h"

class F_Measure
{
protected:
	vector<numclustype> clusterids, classids;
	vector<vector<numclustype>> clusters, classes;
protected:
	//Max Fij
	double Max_Fij(int i)
	{
		numclustype numofclusters = clusters.size()/*, numofclasses = classes.size()*/;
		double Fij = 0, MaxFij = 0;
		double ni = classes[i].size();
		double nij = 0;
		double Precision_ij = 0, Recall_ij = 0;

		for (numclustype j = 0; j < numofclusters; j++)
		{
			sizetype jth_iter = 0;
			sizetype ith_iter = 0;
			sizetype nj = clusters[j].size();
			nij = 0;
			while (jth_iter < nj && ith_iter < ni)
			{
				if (clusters[j][jth_iter] == classes[i][ith_iter])
				{
					nij = nij + 1;
					jth_iter++;
					ith_iter++;
				}
				else if (clusters[j][jth_iter] > classes[i][ith_iter])
					ith_iter++;
				else
					jth_iter++;
			}
			Recall_ij = nij / ni;
			Precision_ij = nij / nj;
			Fij = (2 * Recall_ij * Precision_ij) / (Precision_ij + Recall_ij);
			if (Fij > MaxFij)
				MaxFij = Fij;
		}
		return MaxFij;
	}
public:
	virtual void setClusterIds(const vector<numclustype> &cluster_ids)
	{
		sizetype size = cluster_ids.size();
		this->clusterids = cluster_ids;

		//initialize clusters:
		vector<numclustype> used_ids;
		for (sizetype i = 0; i < size; i++)
		{
			if (!binary_search(used_ids.begin(), used_ids.end(), cluster_ids[i]))
			{
				used_ids.push_back(cluster_ids[i]);
				sort(used_ids.begin(), used_ids.end());
			}
		}
		numclustype numofclusters = used_ids.size();

		vector<numclustype> map(used_ids[used_ids.size() - 1] + 1);
		for (size_t i = 0; i < used_ids.size(); i++)
			map[used_ids[i]] = i;

		vector<numclustype> t;
		clusters.assign(numofclusters, t);

		for (sizetype i = 0; i < size; i++)
		{
			clusters[map[cluster_ids[i]]].push_back(i);
		}
	}
	virtual void setClassIds(const vector<numclustype>& class_ids)
	{
		sizetype size = class_ids.size();
		this->classids = class_ids;

		//initialize clusters:
		vector<numclustype> used_ids;
		for (sizetype i = 0; i < size; i++)
		{
			if (!binary_search(used_ids.begin(), used_ids.end(), class_ids[i]))
			{
				used_ids.push_back(class_ids[i]);
				sort(used_ids.begin(), used_ids.end());
			}
		}
		numclustype numofclasses = used_ids.size();

		vector<numclustype> map(used_ids[used_ids.size() - 1] + 1);
		for (size_t i = 0; i < used_ids.size(); i++)
			map[used_ids[i]] = i;

		vector<numclustype> t;
		classes.assign(numofclasses, t);

		for (sizetype i = 0; i < size; i++)
		{
			classes[map[class_ids[i]]].push_back(i);
		}
	}
	virtual double measure()
	{
		assert(clusterids.size() == classids.size());

		double F = 0;
		double n = classids.size(), numofclasses = classes.size();
		for (numclustype i = 0; i < numofclasses; i++)
		{
			double ni = classes[i].size();
			F = F + ni * Max_Fij(i) / n;
		}
		return F;
	}
};

#endif