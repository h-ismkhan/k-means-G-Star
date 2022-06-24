#ifndef _Custom_Measures_H
#define _Custom_Measures_H

#include "../../Instance/IInstance-Coordinated.h"

class F_Measure_Orig
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
		numclustype numofclusters = 0;
		for (sizetype i = 0; i < size; i++)
		{
			if (cluster_ids[i] > numofclusters)
				numofclusters = cluster_ids[i];
		}
		numofclusters++;
		vector<numclustype> t;
		clusters.assign(numofclusters, t);

		for (sizetype i = 0; i < size; i++)
		{
			clusters[cluster_ids[i]].push_back(i);
		}
	}
	virtual void setClassIds(const vector<numclustype>& class_ids)
	{
		sizetype size = class_ids.size();
		this->classids = class_ids;

		//initialize clusters:
		numclustype numofclasses = 0;
		for (sizetype i = 0; i < size; i++)
		{
			if (class_ids[i] > numofclasses)
				numofclasses = class_ids[i];
		}
		numofclasses++;
		vector<numclustype> t;
		classes.assign(numofclasses, t);

		for (sizetype i = 0; i < size; i++)
		{
			classes[class_ids[i]].push_back(i);
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

class F_Measure_Custom
{
	sizetype N;
	numclustype the_k;
	vector<vector<numclustype>> classes, clusters;
	vector<numclustype> ground_truth;
public:
	virtual void setClusterIds(const vector<numclustype> &cluster_ids)
	{
		size_t num_clus = 0;
		vector<numclustype> clus_orig_IDs;
		for (size_t i = 0; i < cluster_ids.size(); i++)
		{
			if (binary_search(clus_orig_IDs.begin(), clus_orig_IDs.end(), cluster_ids[i]))
				continue;
			clus_orig_IDs.push_back(cluster_ids[i]);
			if (clus_orig_IDs.size() == 1)
				continue;
			if (clus_orig_IDs[clus_orig_IDs.size() - 1] > clus_orig_IDs[clus_orig_IDs.size() - 2])
				continue;
			sort(clus_orig_IDs.begin(), clus_orig_IDs.end());
		}
		vector<numclustype> clus_position(clus_orig_IDs[clus_orig_IDs.size() - 1] + 1);
		for (size_t i = 0; i < clus_orig_IDs.size(); i++)
			clus_position[clus_orig_IDs[i]] = i;

		clusters = vector<vector<numclustype>>(clus_orig_IDs.size());
		for (size_t i = 0; i < cluster_ids.size(); i++)
			clusters[clus_position[cluster_ids[i]]].push_back(i);

		for (size_t i = 0; i < clusters.size(); i++)
			sort(clusters[i].begin(), clusters[i].end());
	}
	virtual void setClassIds(const vector<numclustype>& class_ids)
	{
		ground_truth = class_ids;
		N = class_ids.size();
		size_t num_clus = 0;
		vector<numclustype> clus_orig_IDs;
		for (size_t i = 0; i < class_ids.size(); i++)
		{
			if (binary_search(clus_orig_IDs.begin(), clus_orig_IDs.end(), class_ids[i]))
				continue;
			clus_orig_IDs.push_back(class_ids[i]);
			if (clus_orig_IDs.size() == 1)
				continue;
			if (clus_orig_IDs[clus_orig_IDs.size() - 1] > clus_orig_IDs[clus_orig_IDs.size() - 2])
				continue;
			sort(clus_orig_IDs.begin(), clus_orig_IDs.end());
		}
		vector<numclustype> clus_position(clus_orig_IDs[clus_orig_IDs.size() - 1] + 1);
		for (size_t i = 0; i < clus_orig_IDs.size(); i++)
			clus_position[clus_orig_IDs[i]] = i;

		the_k = clus_orig_IDs.size();

		classes = vector<vector<numclustype>>(clus_orig_IDs.size());
		for (size_t i = 0; i < class_ids.size(); i++)
			classes[clus_position[class_ids[i]]].push_back(i);

		for (size_t i = 0; i < classes.size(); i++)
			sort(classes[i].begin(), classes[i].end());
	}
	virtual double measure()
	{
		vector<vector<double>> score_mat(clusters.size(), vector<double>(the_k, 0));

		for (size_t ci = 0; ci < clusters.size(); ci++)
		{
			for (size_t j = 0; j < clusters[ci].size(); j++)
			for (size_t k = 0; k < classes.size(); k++)
			if (binary_search(classes[k].begin(), classes[k].end(), clusters[ci][j]))
			{
				score_mat[ci][k] = score_mat[ci][k] + 1;
				break;
			}
		}

		vector<bool> is_assigned_to_aclass(clusters.size(), false);
		vector<numclustype> class_owner(the_k, -1);

		for (size_t k = 0; k < classes.size(); k++)
		{
			double score = 0;
			for (size_t ci = 0; ci < clusters.size(); ci++)
			if (score_mat[ci][k] > score && !is_assigned_to_aclass[ci])
			{
				score = score_mat[ci][k];
				class_owner[k] = ci;
			}

			if (score == 0)
				return 0.0;
			is_assigned_to_aclass[class_owner[k]] = true;
		}

		vector<bool> is_in_final_pIDs(N, false);
		vector<pair<sizetype, numclustype>> what_in_final_pIDs;
		for (size_t k = 0; k < class_owner.size(); k++)
		for (size_t j = 0; j < clusters[class_owner[k]].size(); j++)
		{
			is_in_final_pIDs[clusters[class_owner[k]][j]] = true;
			what_in_final_pIDs.push_back(make_pair(clusters[class_owner[k]][j], k));
		}
		sort(what_in_final_pIDs.begin(), what_in_final_pIDs.end());

		vector<numclustype> custom_ground_truth, csutom_clus_ids;
		for (size_t i = 0; i < what_in_final_pIDs.size(); i++)
		{
			csutom_clus_ids.push_back(what_in_final_pIDs[i].second);
			custom_ground_truth.push_back(ground_truth[what_in_final_pIDs[i].first]);
		}
		F_Measure_Orig f_orig;
		f_orig.setClusterIds(csutom_clus_ids);
		f_orig.setClassIds(custom_ground_truth);

		return f_orig.measure();
	}
};

class NMI_Measure_Custom
{
	sizetype N;
	numclustype the_k;
	vector<vector<numclustype>> classes, clusters;
	vector<numclustype> ground_truth;
public:
	virtual void setClusterIds(const vector<numclustype> &cluster_ids)
	{
		size_t num_clus = 0;
		vector<numclustype> clus_orig_IDs;
		for (size_t i = 0; i < cluster_ids.size(); i++)
		{
			if (binary_search(clus_orig_IDs.begin(), clus_orig_IDs.end(), cluster_ids[i]))
				continue;
			clus_orig_IDs.push_back(cluster_ids[i]);
			if (clus_orig_IDs.size() == 1)
				continue;
			if (clus_orig_IDs[clus_orig_IDs.size() - 1] > clus_orig_IDs[clus_orig_IDs.size() - 2])
				continue;
			sort(clus_orig_IDs.begin(), clus_orig_IDs.end());
		}
		vector<numclustype> clus_position(clus_orig_IDs[clus_orig_IDs.size() - 1] + 1);
		for (size_t i = 0; i < clus_orig_IDs.size(); i++)
			clus_position[clus_orig_IDs[i]] = i;

		clusters = vector<vector<numclustype>>(clus_orig_IDs.size());
		for (size_t i = 0; i < cluster_ids.size(); i++)
			clusters[clus_position[cluster_ids[i]]].push_back(i);

		for (size_t i = 0; i < clusters.size(); i++)
			sort(clusters[i].begin(), clusters[i].end());
	}
	virtual void setClassIds(const vector<numclustype>& class_ids)
	{
		ground_truth = class_ids;
		N = class_ids.size();
		size_t num_clus = 0;
		vector<numclustype> clus_orig_IDs;
		for (size_t i = 0; i < class_ids.size(); i++)
		{
			if (binary_search(clus_orig_IDs.begin(), clus_orig_IDs.end(), class_ids[i]))
				continue;
			clus_orig_IDs.push_back(class_ids[i]);
			if (clus_orig_IDs.size() == 1)
				continue;
			if (clus_orig_IDs[clus_orig_IDs.size() - 1] > clus_orig_IDs[clus_orig_IDs.size() - 2])
				continue;
			sort(clus_orig_IDs.begin(), clus_orig_IDs.end());
		}
		vector<numclustype> clus_position(clus_orig_IDs[clus_orig_IDs.size() - 1] + 1);
		for (size_t i = 0; i < clus_orig_IDs.size(); i++)
			clus_position[clus_orig_IDs[i]] = i;

		the_k = clus_orig_IDs.size();

		classes = vector<vector<numclustype>>(clus_orig_IDs.size());
		for (size_t i = 0; i < class_ids.size(); i++)
			classes[clus_position[class_ids[i]]].push_back(i);

		for (size_t i = 0; i < classes.size(); i++)
			sort(classes[i].begin(), classes[i].end());
	}
	virtual double measure()
	{
		vector<vector<double>> score_mat(clusters.size(), vector<double>(the_k, 0));

		for (size_t ci = 0; ci < clusters.size(); ci++)
		{
			for (size_t j = 0; j < clusters[ci].size(); j++)
			for (size_t k = 0; k < classes.size(); k++)
			if (binary_search(classes[k].begin(), classes[k].end(), clusters[ci][j]))
			{
				score_mat[ci][k] = score_mat[ci][k] + 1;
				break;
			}
		}

		vector<bool> is_assigned_to_aclass(clusters.size(), false);
		vector<numclustype> class_owner(the_k, -1);

		for (size_t k = 0; k < classes.size(); k++)
		{
			double score = 0;
			for (size_t ci = 0; ci < clusters.size(); ci++)
			if (score_mat[ci][k] > score && !is_assigned_to_aclass[ci])
			{
				score = score_mat[ci][k];
				class_owner[k] = ci;
			}

			if (score == 0)
				return 0.0;
			is_assigned_to_aclass[class_owner[k]] = true;
		}

		vector<bool> is_in_final_pIDs(N, false);
		vector<pair<sizetype, numclustype>> what_in_final_pIDs;
		for (size_t k = 0; k < class_owner.size(); k++)
		for (size_t j = 0; j < clusters[class_owner[k]].size(); j++)
		{
			is_in_final_pIDs[clusters[class_owner[k]][j]] = true;
			what_in_final_pIDs.push_back(make_pair(clusters[class_owner[k]][j], k));
		}
		sort(what_in_final_pIDs.begin(), what_in_final_pIDs.end());

		vector<numclustype> custom_ground_truth, csutom_clus_ids;
		for (size_t i = 0; i < what_in_final_pIDs.size(); i++)
		{
			csutom_clus_ids.push_back(what_in_final_pIDs[i].second);
			custom_ground_truth.push_back(ground_truth[what_in_final_pIDs[i].first]);
		}
		NMI_Measure nmi_orig;
		nmi_orig.setClusterIds(csutom_clus_ids);
		nmi_orig.setClassIds(custom_ground_truth);

		return nmi_orig.measure();
	}
};

class Remained_Measure_Custom
{
	sizetype N;
	numclustype the_k;
	vector<vector<numclustype>> classes, clusters;
	vector<numclustype> ground_truth;
public:
	virtual void setClusterIds(const vector<numclustype> &cluster_ids)
	{
		size_t num_clus = 0;
		vector<numclustype> clus_orig_IDs;
		for (size_t i = 0; i < cluster_ids.size(); i++)
		{
			if (binary_search(clus_orig_IDs.begin(), clus_orig_IDs.end(), cluster_ids[i]))
				continue;
			clus_orig_IDs.push_back(cluster_ids[i]);
			if (clus_orig_IDs.size() == 1)
				continue;
			if (clus_orig_IDs[clus_orig_IDs.size() - 1] > clus_orig_IDs[clus_orig_IDs.size() - 2])
				continue;
			sort(clus_orig_IDs.begin(), clus_orig_IDs.end());
		}
		vector<numclustype> clus_position(clus_orig_IDs[clus_orig_IDs.size() - 1] + 1);
		for (size_t i = 0; i < clus_orig_IDs.size(); i++)
			clus_position[clus_orig_IDs[i]] = i;

		clusters = vector<vector<numclustype>>(clus_orig_IDs.size());
		for (size_t i = 0; i < cluster_ids.size(); i++)
			clusters[clus_position[cluster_ids[i]]].push_back(i);

		for (size_t i = 0; i < clusters.size(); i++)
			sort(clusters[i].begin(), clusters[i].end());
	}
	virtual void setClassIds(const vector<numclustype>& class_ids)
	{
		ground_truth = class_ids;
		N = class_ids.size();
		size_t num_clus = 0;
		vector<numclustype> clus_orig_IDs;
		for (size_t i = 0; i < class_ids.size(); i++)
		{
			if (binary_search(clus_orig_IDs.begin(), clus_orig_IDs.end(), class_ids[i]))
				continue;
			clus_orig_IDs.push_back(class_ids[i]);
			if (clus_orig_IDs.size() == 1)
				continue;
			if (clus_orig_IDs[clus_orig_IDs.size() - 1] > clus_orig_IDs[clus_orig_IDs.size() - 2])
				continue;
			sort(clus_orig_IDs.begin(), clus_orig_IDs.end());
		}
		vector<numclustype> clus_position(clus_orig_IDs[clus_orig_IDs.size() - 1] + 1);
		for (size_t i = 0; i < clus_orig_IDs.size(); i++)
			clus_position[clus_orig_IDs[i]] = i;

		the_k = clus_orig_IDs.size();

		classes = vector<vector<numclustype>>(clus_orig_IDs.size());
		for (size_t i = 0; i < class_ids.size(); i++)
			classes[clus_position[class_ids[i]]].push_back(i);

		for (size_t i = 0; i < classes.size(); i++)
			sort(classes[i].begin(), classes[i].end());
	}
	virtual double measure()
	{
		vector<vector<double>> score_mat(clusters.size(), vector<double>(the_k, 0));

		for (size_t ci = 0; ci < clusters.size(); ci++)
		{
			for (size_t j = 0; j < clusters[ci].size(); j++)
			for (size_t k = 0; k < classes.size(); k++)
			if (binary_search(classes[k].begin(), classes[k].end(), clusters[ci][j]))
			{
				score_mat[ci][k] = score_mat[ci][k] + 1;
				break;
			}
		}

		vector<bool> is_assigned_to_aclass(clusters.size(), false);
		vector<numclustype> class_owner(the_k, -1);

		for (size_t k = 0; k < classes.size(); k++)
		{
			double score = 0;
			for (size_t ci = 0; ci < clusters.size(); ci++)
			if (score_mat[ci][k] > score && !is_assigned_to_aclass[ci])
			{
				score = score_mat[ci][k];
				class_owner[k] = ci;
			}

			if (score == 0)
				return 0.0;
			is_assigned_to_aclass[class_owner[k]] = true;
		}

		vector<bool> is_in_final_pIDs(N, false);
		vector<pair<sizetype, numclustype>> what_in_final_pIDs;
		for (size_t k = 0; k < class_owner.size(); k++)
		for (size_t j = 0; j < clusters[class_owner[k]].size(); j++)
		{
			is_in_final_pIDs[clusters[class_owner[k]][j]] = true;
			what_in_final_pIDs.push_back(make_pair(clusters[class_owner[k]][j], k));
		}
		sort(what_in_final_pIDs.begin(), what_in_final_pIDs.end());

		vector<numclustype> custom_ground_truth, csutom_clus_ids;
		for (size_t i = 0; i < what_in_final_pIDs.size(); i++)
		{
			csutom_clus_ids.push_back(what_in_final_pIDs[i].second);
			custom_ground_truth.push_back(ground_truth[what_in_final_pIDs[i].first]);
		}
		NMI_Measure nmi_orig;
		nmi_orig.setClusterIds(csutom_clus_ids);
		nmi_orig.setClassIds(custom_ground_truth);

		return csutom_clus_ids.size() / (distype)N;
	}
};

string string_round(double val, size_t k)
{
	double valt = val;
	string result = "";
	
	int A = val;
	result = result + to_string(A) + ".";
	
	for (size_t i = 0; i < k; i++)
	{
		val = val - A;
		val = val * 10;
		A = val;
		result = result + to_string(A);
	}
	return result;
}

class MyOwn_PrintingMeasures
{
protected:
	vector<numclustype> clusterids, classids;
	vector<vector<numclustype>> clusters, classes;
protected:
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
	virtual double measure(string path = "my-own-measures.txt")
	{
		assert(clusterids.size() == classids.size());

		for (numclustype ci = 0; ci < clusters.size(); ci++)
			sort(clusters[ci].begin(), clusters[ci].end());
		for (numclustype ci = 0; ci < classes.size(); ci++)
			sort(classes[ci].begin(), classes[ci].end());

		vector<vector<double>> sims(clusters.size(), vector<double>(classes.size(), 0));
		vector<vector<double>> clus_not_in_gt(clusters.size(), vector<double>(classes.size(), 0));
		vector<vector<double>> gt_not_in_clus(classes.size(), vector<double>(clusters.size(), 0));
		for (numclustype ri = 0; ri < clusters.size(); ri++)
		{
			for (numclustype rj = 0; rj < classes.size(); rj++)
			{
				for (numclustype ci = 0; ci < clusters[ri].size(); ci++)
				if (binary_search(classes[rj].begin(), classes[rj].end(), clusters[ri][ci]))
					sims[ri][rj] = sims[ri][rj] + 1.0;
			}
		}
		for (numclustype ri = 0; ri < clusters.size(); ri++)
		{
			for (numclustype rj = 0; rj < classes.size(); rj++)
			{
				for (numclustype ci = 0; ci < clusters[ri].size(); ci++)
				if (!binary_search(classes[rj].begin(), classes[rj].end(), clusters[ri][ci]))
					clus_not_in_gt[ri][rj] = clus_not_in_gt[ri][rj] + 1.0;
			}
		}
		for (numclustype ri = 0; ri < classes.size(); ri++)
		{
			for (numclustype rj = 0; rj < clusters.size(); rj++)
			{
				for (numclustype ci = 0; ci < classes[ri].size(); ci++)
				if (!binary_search(clusters[rj].begin(), clusters[rj].end(), classes[ri][ci]))
					gt_not_in_clus[ri][rj] = gt_not_in_clus[ri][rj] + 1.0;
			}
		}
		vector<numclustype> my_owner_cluster(classes.size(), 0);
		for (numclustype rj = 0; rj < classes.size(); rj++)
		for (numclustype ri = 0; ri < clusters.size(); ri++)
		if (sims[ri][rj] > sims[my_owner_cluster[rj]][rj])
			my_owner_cluster[rj] = ri;

		vector<double> owner_sims(classes.size()), owner_clus_not_in_gt(classes.size())
			, owner_gt_not_in_clus(classes.size()), overal_sims(classes.size());
		double sim_overal = 0;
		for (numclustype rj = 0; rj < classes.size(); rj++)
		{
			owner_sims[rj] = sims[my_owner_cluster[rj]][rj] / (double)clusters[my_owner_cluster[rj]].size();
			owner_clus_not_in_gt[rj] = clus_not_in_gt[my_owner_cluster[rj]][rj] / (double)clusters[my_owner_cluster[rj]].size();
			owner_gt_not_in_clus[rj] = gt_not_in_clus[rj][my_owner_cluster[rj]] / (double)classes[rj].size();
			overal_sims[rj] = owner_sims[rj] - owner_clus_not_in_gt[rj];
			sim_overal = sim_overal + overal_sims[rj];
		}

		cout << endl << "owners           :	";
		for (numclustype rj = 0; rj < classes.size(); rj++)
			cout << my_owner_cluster[rj] << "	";

		cout << endl << "similarities     :	";
		for (numclustype rj = 0; rj < classes.size(); rj++)
			cout << string_round(owner_sims[rj], 2) << "	";

		cout << endl << "C mmbr not in GT :	";
		for (numclustype rj = 0; rj < classes.size(); rj++)
			cout << string_round(owner_clus_not_in_gt[rj], 2) << "	";

		cout << endl << "GT not in C mmbr :	";
		for (numclustype rj = 0; rj < classes.size(); rj++)
			cout << string_round(owner_gt_not_in_clus[rj], 2) << "	";
		cout << endl;

		{
			ofstream out(path, std::ios_base::app);
			out << endl << "owners           :	";
			for (numclustype rj = 0; rj < classes.size(); rj++)
				out << my_owner_cluster[rj] << "	";

			out << endl << "similarities     :	";
			for (numclustype rj = 0; rj < classes.size(); rj++)
				out << string_round(owner_sims[rj], 2) << "	";

			out << endl << "C mmbr not in GT :	";
			for (numclustype rj = 0; rj < classes.size(); rj++)
				out << string_round(owner_clus_not_in_gt[rj], 2) << "	";

			out << endl << "GT not in C mmbr :	";
			for (numclustype rj = 0; rj < classes.size(); rj++)
				out << string_round(owner_gt_not_in_clus[rj], 2) << "	";
			out << endl;
			out.close();
		}

		return sim_overal;
	}
};

#endif