#ifndef _NMI_Measure_H
#define _NMI_Measure_H

#include "../../Instance/IInstance-Coordinated.h"

class NMI_Measure
{
protected:
	vector<numclustype> pa, ground_truth;
	vector<vector<numclustype>> clusters, classes;
public:
	virtual void setClusterIds(const vector<numclustype> &cluster_ids)
	{
		this->pa = cluster_ids;
	}
	virtual void setClassIds(const vector<numclustype>& class_ids)
	{
		this->ground_truth = class_ids;
	}
	virtual double measure()
	{
		size_t n = ground_truth.size();
		
		double nmi = 0;
		int qa = pa[0];// -1;
		int qb = ground_truth[0];// -1;
		vector <int > ga; // Group A
		vector <int > gb; // Group B
		for (int i = 0; i < n; i++) {
			if (qa < pa[i]) qa = pa[i];
			if (qb < ground_truth[i]) qb = ground_truth[i];
		}
		qa++;
		qb++;

		if (qa == 1 && qb == 1) {
			nmi = 0.0;
		}
		else {
			ga.resize(qa);
			for (int q = 0; q < qa; q++) ga[q] = 0;
			gb.resize(qb);
			for (int q = 0; q < qb; q++) gb[q] = 0;

			vector< vector<int> > A;
			vector< vector<int> > B;
			A.resize(qa); // Existing structure
			B.resize(qa); // Counting structure
			for (int i = 0; i < n; i++) {
				int q = pa[i];
				int t = ground_truth[i];
				ga[q]++;
				gb[t]++;
				int idx = -1;
				for (int j = 0; j < A[q].size(); j++) {
					if (A[q][j] == t) {
						idx = j;
						break;
					}
				}
				if (idx == -1) { // Pair [x y] did not show up
					A[q].push_back(t);
					B[q].push_back(1);
				}
				else { // [x y] is there
					B[q][idx] += 1;
				}
			}
			double Ha = 0;
			for (int q = 0; q < qa; q++) {
				if (ga[q] == 0) continue;
				double prob = 1.0*ga[q] / n;
				Ha += prob*log(prob);
			}
			double Hb = 0;
			for (int q = 0; q < qb; q++) {
				if (gb[q] == 0) continue;
				double prob = 1.0*gb[q] / n;
				Hb += prob*log(prob);
			}
			double Iab = 0;
			for (int q = 0; q < qa; q++) {
				for (int idx = 0; idx < A[q].size(); idx++) {
					double prob = 1.0*B[q][idx] / n;
					int t = A[q][idx];
					Iab += prob*log(prob / (1.0*ga[q] / n*gb[t] / n));
				}
			}

			nmi = -2.0*Iab / (Ha + Hb);
		}
		return nmi;
	}
};

#endif