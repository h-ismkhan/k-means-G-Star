#ifndef _kmean_gs_withFP_H
#define _kmean_gs_withFP_H

#include "k-means-interface.h"
#include "k-means-2nd-Geom.h"
#include "../Utils/myTimer.h"
#include "../Utils/myRandom.h"
#include "../Utils/SparsVector.h"
#include <algorithm>
#include <assert.h>
#include <random>
#include "kdtree.h"

#include "../Distance/Euclidean-Distance.h"

//KM-G* with the Forcing Point
class k_means_gs_withFP : public k_means_interface
{

	enum axis{ x_coord, y_coord, z_coord };
	numclustype maxnumclus = -1;

	sizetype size;
	dimtype dim;
	numclustype the_k;

	vector<vector<coordtype>> starting_initial_centers;
	vector<vector<coordtype>> centers;

	const IInstance_Coordinated *inst;
	vector<numclustype> center_1st;
	vector<distype> center_1st_dis;
	vector<sizetype> sizeOf;

	ISeed_Initializer *seedInitializer = 0;
	size_t max_num_iter = SIZE_MAX;

	IDistance *dis_metric = 0;
private:
	vector<distype> disPointC_atleast, disPointC_atmost, deltaC, pc_dis;
	vector<vector<distype>> ccprime, cc, cprime_cprime;
	vector<vector<coordtype>> old_centers;

	vector<numclustype> center_2nd;
	vector<distype> center_2nd_dis;
private:

private:
	virtual void initiate_centers()
	{
		if (this->seedInitializer)
		{
			this->seedInitializer->set_instance(inst);
			this->seedInitializer->set_the_k(the_k);
		}
		else assert(this->seedInitializer != 0);
		centers.assign(the_k, vector<coordtype>(dim));
		const vector<const coordtype*>& seeds_coords = this->seedInitializer->next_seeds();
		for (numclustype ci = 0; ci < the_k; ci++)
		{
			for (dimtype di = 0; di < dim; di++)
				centers[ci][di] = seeds_coords[ci][di];
		}
		starting_initial_centers = centers;
	}
	virtual void AssignCentersMem()
	{
		centers.assign(the_k, vector<coordtype>(dim));
	}
	virtual void UpdateCenters()
	{
		old_centers = centers;
		cc = cprime_cprime;

		for (numclustype i = 0; i < the_k; i++)
		{
			sizeOf[i] = 0;
			for (dimtype j = 0; j < dim; j++)
				centers[i][j] = 0;
		}
		for (sizetype i = 0; i < size; i++)
		{
			sizeOf[center_1st[i]]++;
			numclustype c_id = center_1st[i];
			const coordtype* row = (*inst)[i];
			for (dimtype j = 0; j < dim; j++)
				centers[c_id][j] = centers[c_id][j] + row[j];
		}
		for (numclustype i = 0; i < the_k; i++)
		{
			if (sizeOf[i])
			{
				for (dimtype j = 0; j < dim; j++)
					centers[i][j] = centers[i][j] / sizeOf[i];
			}
		}

		for (numclustype i = 0; i < the_k; i++)
			deltaC[i] = this->dis_metric->measure(old_centers[i].data(), centers[i].data());

		for (numclustype i = 0; i < the_k; i++)
		for (numclustype j = 0; j < the_k; j++)
		{
			if (i == j)
			{
				ccprime[i][j] = deltaC[i];
				continue;
			}
			if (deltaC[j] == 0)
			{
				ccprime[i][j] = cc[i][j];
				continue;
			}
			ccprime[i][j] = this->dis_metric->measure(old_centers[i].data(), centers[j].data());
		}

		for (numclustype i = 0; i < the_k; i++)
		for (numclustype j = i + 1; j < the_k; j++)
		{
			if (i == j)
			{
				cprime_cprime[i][j] = 0;
				continue;
			}
			if (deltaC[i] == 0)
			{
				cprime_cprime[i][j] = cprime_cprime[j][i] = ccprime[i][j];
				continue;
			}
			if (deltaC[j] == 0)
			{
				cprime_cprime[i][j] = cprime_cprime[j][i] = ccprime[j][i];
				continue;
			}
			cprime_cprime[i][j] = cprime_cprime[j][i] = this->dis_metric->measure(centers[i].data(), centers[j].data());
		}


	}

	void nearest_centers_listOf(vector<vector<numclustype>> &sorted_centers_listOf)
	{
		sorted_centers_listOf.clear();
		for (numclustype the_C = 0; the_C < the_k; the_C++)
		{
			vector<pair<distype, numclustype>> list_of_c;
			for (numclustype the_O = 0; the_O < the_k; the_O++)
			{
				if (the_O == the_C)
					continue;
				list_of_c.push_back(make_pair(cprime_cprime[the_C][the_O], the_O));
			}
			sort(list_of_c.begin(), list_of_c.end());
			vector<numclustype> sortedlist;
			for (numclustype oi = 0; oi < list_of_c.size(); oi++)
				sortedlist.push_back(list_of_c[oi].second);
			sorted_centers_listOf.push_back(sortedlist);
		}
	}
	inline bool does_have_invalid_value(const vector<coordtype> &input)
	{
		for (size_t i = 0; i < input.size(); i++)
		{
			if (GeoUtil::isInvalid(input[i]))
				return true;
		}
		return false;
	}

	virtual short InitialPhase()
	{
		disPointC_atleast.assign(size, 0);
		disPointC_atmost = disPointC_atleast;
		deltaC.assign(the_k, 0);
		ccprime.assign(the_k, deltaC);
		cprime_cprime = ccprime;

		for (numclustype i = 0; i < the_k; i++)
		for (numclustype j = i + 1; j < the_k; j++)
			this->cprime_cprime[i][j] = this->cprime_cprime[j][i] =
			this->dis_metric->measure(centers[i].data(), centers[j].data());

		short IsChanged = 0;
		vector<vector<numclustype>> sorted_centers_listOf;
		nearest_centers_listOf(sorted_centers_listOf);
		SparsVector<bool> is_considered(the_k, false);

		numclustype the_C = Random((size_t)0, (size_t)the_k - 1);
		numclustype the_D = sorted_centers_listOf[the_C][the_k / 2];
		distype CD = cprime_cprime[the_C][the_D];
		vector<vector<coordtype>> centers_3d_CD;
		for (numclustype the_O = 0; the_O < the_k; the_O++)
		{
			distype OD = cprime_cprime[the_O][the_D],
				OC = cprime_cprime[the_C][the_O];

			vector<coordtype> O3d_CD = get_point_as2DpointVec(CD, OC, OD);
			centers_3d_CD.push_back(O3d_CD);
		}

		Euclidean_Distance dis_3D_euc(3), dis_2D_euc(2);
		for (sizetype pi = 0; pi < size; pi++)
		{
			sizetype the_F = -1;
			distype pF = dismax;

			is_considered.reset();

			distype pC = this->dis_metric->measure(centers[the_C].data(), (*inst)[pi]);
			distype pD = this->dis_metric->measure(centers[the_D].data(), (*inst)[pi]);
			is_considered[the_C] = true;
			is_considered[the_D] = true;

			distype target_dis = pC;
			numclustype target_c = the_C;
			if (pD < pC || (pD == pC && the_D < target_c))
			{
				target_dis = pD;
				target_c = the_D;
			}

			vector<coordtype>point3d_CD = get_point_as2DpointVec(CD, pC, pD);
			for (numclustype oi = 0; oi < the_k; oi++)
			{
				numclustype the_O = oi;
				if (is_considered[the_O])
					continue;
				is_considered[the_O] = true;

				distype the_LB = dis_2D_euc.measure(point3d_CD.data(), centers_3d_CD[the_O].data());

				if (the_LB > target_dis)
					continue;

				distype CO, DO, CF, DF, FO;
				if (the_F != -1)
				{
					vector<coordtype> point3d_CD_backup = point3d_CD;
					CO = cprime_cprime[the_O][the_C], DO = cprime_cprime[the_O][the_D],
						CF = cprime_cprime[the_F][the_C], DF = cprime_cprime[the_F][the_D],
						FO = cprime_cprime[the_O][the_F];
					update_yz_of_point(CD, CO, DO
						, CF, DF, FO, pF, centers_3d_CD[the_O], point3d_CD);
					distype the_LB = dis_3D_euc.measure(point3d_CD.data(), centers_3d_CD[the_O].data());
					point3d_CD = point3d_CD_backup;

					if (the_LB > target_dis)
						continue;
				}

				distype cur_dis = this->dis_metric->measure((*inst)[pi], centers[the_O].data());
				if (the_F == -1)
				{
					the_F = the_O;
					pF = cur_dis;
				}
				else if (CF * DF < FO * DO && pF < cur_dis)
				{
					the_F = the_O;
					pF = cur_dis;
				}

				if (cur_dis < target_dis || (cur_dis == target_dis && target_c > the_O))
				{
					target_dis = cur_dis;
					target_c = the_O;
				}
			}
			center_1st_dis[pi] = target_dis;
			disPointC_atleast[pi] = disPointC_atmost[pi] = target_dis;

			if (target_c != center_1st[pi])
			{
				IsChanged = 1;
				center_1st[pi] = target_c;
			}

			//______t
			/*distype min_dis = dismax;
			numclustype index = -1;
			for (numclustype cj = 0; cj < the_k; cj++)
			{
			distype cur_dis = dis_metric->measure((*inst)[pi], centers[cj].data());

			if (cur_dis < min_dis)
			{
			min_dis = cur_dis;
			index = cj;
			}
			}
			if (target_c != index)
			cout << "" << endl;*/
		}
		UpdateCenters();

		//cout << "#dis:	" << dis_metric->counter() << endl;
		return IsChanged;
	}

	void get_popri_specialcases(distype delta_c, distype co, distype delta_o, distype cpri_opri,
		distype copri, distype pc, distype p_cpri, distype &p_opri)
	{
		if (delta_c == 0)
		{
			if (delta_o == 0)
			{
				p_opri = dismax;
				return;
			}
			distype mylb = abs(pc - copri);
			if (mylb > p_opri)
				p_opri = mylb;
			if (p_opri > p_cpri)
				return;
			get_popri_deltaC_isZero(delta_o, co, copri, pc, p_cpri, p_opri);
			if (p_opri > p_cpri)
				return;
		}
		if (delta_o == 0 && pc >= p_cpri)
		{
			p_opri = dismax;
			return;
		}
		if (pc == 0)
		{
			if (p_opri < copri)
				p_opri = copri;
			return;
		}
		if (p_cpri == 0)
		{
			if (p_opri < cpri_opri)
				p_opri = cpri_opri;
			return;
		}
	}
	inline void get_popri_deltaC_isZero(distype delta_o, distype co,
		distype copri, distype pc, distype p_cpri, distype &p_opri)
	{
		//the algorithm: po >= pc then LB for p_c_o is ready and o_c_opri is known.
		if (delta_o == 0)
		{
			p_opri = dismax;
			return;
		}
		if (co - delta_o > pc * 2)
		{
			p_opri = dismax;
			return;
		}

		p_opri = abs(pc - copri);
		if (p_opri > p_cpri)
			return;
		distype cos_o_c_opri = GeoUtil::cos_between_ab(co, copri, delta_o);
		if (p_cpri < co / 2)
		{
			//consider p on segment co.
			p_opri = GeoUtil::front_angle_magnitude(pc, copri, cos_o_c_opri);
			return;
		}
		//teta(o_c_opri) should be increased!
		distype o_c_opri = acos(cos_o_c_opri);
		distype cos_o_c_p = GeoUtil::cos_between_ab(co, pc, pc);
		distype o_c_p = acos(cos_o_c_p);
		/*if (o_c_p < o_c_opri)
		{
		cout << "o_c_p:	" << o_c_p << endl;
		cout << "o_c_opri:	" << o_c_opri << endl;
		getch();
		}*/
		//the algorithm: po >= pc then LB for p_c_o is ready and o_c_opri is known.
		//assert(o_c_p >= o_c_opri);//why? I change it from o_c_p > o_c_opri.
		distype opri_c_p = abs(o_c_p - o_c_opri);
		p_opri = GeoUtil::front_angle_magnitude(pc, copri, cos(opri_c_p));
		//if (p_opri )

		//strategy 2: p, o and o' are on a line. if they are on a line=>coo' = cop
	}

	inline void update_yz_of_point(const distype MN, const distype MO, const distype NO
		, const distype MF, const distype NF, const distype FO, const distype pF
		, vector<coordtype> &O2d_MN, vector<coordtype> &p2d_MN)
	{
		distype rP_or_pL = p2d_MN[y_coord];
		if (rP_or_pL == 0)
		{
			//pO can be exactly calculated.
			p2d_MN[z_coord] = 0;
			return;
		}
		vector<coordtype> F2d_MN = get_point_as2DpointVec(MN, MF, NF);
		distype rF_or_FL = F2d_MN[y_coord];
		distype rF_or_FL_2 = rF_or_FL * rF_or_FL;
		if (rF_or_FL == 0)
		{
			p2d_MN[z_coord] = 0;
			return;
		}

		distype pF_xdis = abs(p2d_MN[x_coord] - F2d_MN[x_coord]);
		distype pF_2 = pF * pF;
		distype pF_on_VerticalPage_2 = pF_2 - pF_xdis * pF_xdis;
		distype pF_on_VerticalPage = sqrt(pF_on_VerticalPage_2);
		distype cos_FL_with_pMN = GeoUtil::cos_between_ab(rF_or_FL, rP_or_pL, pF_on_VerticalPage);

		distype angle_FL_with_pMN = acos(cos_FL_with_pMN);

		distype OF_xdis = abs(O2d_MN[x_coord] - F2d_MN[x_coord]);
		distype OF_2 = FO * FO;
		distype OF_on_VerticalPage_2 = OF_2 - OF_xdis * OF_xdis;
		distype OF_on_VerticalPage = sqrt(OF_on_VerticalPage_2);
		distype OL = O2d_MN[y_coord];

		distype cos_OLF_on_VerticalPage = GeoUtil::cos_between_ab(rF_or_FL, OL, OF_on_VerticalPage);
		distype OLF_on_VerticalPage = acos(cos_OLF_on_VerticalPage);

		distype angle_pL_with_MNO = abs(OLF_on_VerticalPage - angle_FL_with_pMN);


		p2d_MN[y_coord] = rP_or_pL * cos(angle_pL_with_MNO);
		p2d_MN[z_coord] = rP_or_pL * sin(angle_pL_with_MNO);
	}
	inline void update_yz_of_point(const distype MN, const distype MO, const distype NO
		, const distype MF, const distype NF, const distype FO, const distype pF
		, const MyPoint &O2d_MN, MyPoint &p2d_MN)
	{
		distype rP_or_pL = p2d_MN[y_coord];
		if (rP_or_pL == 0)
		{
			//pO can be exactly calculated.
			p2d_MN[z_coord] = 0;
			return;
		}
		vector<coordtype> F2d_MN = get_point_as2DpointVec(MN, MF, NF);
		distype rF_or_FL = F2d_MN[y_coord];
		distype rF_or_FL_2 = rF_or_FL * rF_or_FL;
		if (rF_or_FL == 0)
		{
			p2d_MN[z_coord] = 0;
			return;
		}

		distype pF_xdis = abs(p2d_MN[x_coord] - F2d_MN[x_coord]);
		distype pF_2 = pF * pF;
		distype pF_on_VerticalPage_2 = pF_2 - pF_xdis * pF_xdis;
		distype pF_on_VerticalPage = sqrt(pF_on_VerticalPage_2);
		distype cos_FL_with_pMN = GeoUtil::cos_between_ab(rF_or_FL, rP_or_pL, pF_on_VerticalPage);

		distype angle_FL_with_pMN = acos(cos_FL_with_pMN);

		distype OF_xdis = abs(O2d_MN[x_coord] - F2d_MN[x_coord]);
		distype OF_2 = FO * FO;
		distype OF_on_VerticalPage_2 = OF_2 - OF_xdis * OF_xdis;
		distype OF_on_VerticalPage = sqrt(OF_on_VerticalPage_2);
		distype OL = O2d_MN[y_coord];

		distype cos_OLF_on_VerticalPage = GeoUtil::cos_between_ab(rF_or_FL, OL, OF_on_VerticalPage);
		distype OLF_on_VerticalPage = acos(cos_OLF_on_VerticalPage);

		distype angle_pL_with_MNO = abs(OLF_on_VerticalPage - angle_FL_with_pMN);


		p2d_MN[y_coord] = rP_or_pL * cos(angle_pL_with_MNO);
		p2d_MN[z_coord] = rP_or_pL * sin(angle_pL_with_MNO);
	}

	void get_clusters_upd_1stDis(vector<vector<pair<distype, sizetype >>> &cluster_mem)
	{
		cluster_mem.clear();
		cluster_mem.assign(the_k, vector<pair<distype, sizetype >>());

		for (sizetype point = 0; point < size; point++)
		{
			numclustype cid = center_1st[point];
			if (deltaC[cid] > 0)
				this->center_1st_dis[point] = this->dis_metric->measure((*inst)[point], this->centers[cid].data());
			cluster_mem[cid].push_back(make_pair(center_1st_dis[point], point));
		}
	}
	MyPoint get_point_as2Dpoint(distype CD, distype xC, distype xD)
	{
		coordtype x, y, z = 0;
		if (xD == 0)
		{
			x = CD;
			y = 0;
			/*vector<coordtype> point;
			point.push_back(x);
			point.push_back(y);
			point.push_back(z);
			return point;*/
			return MyPoint(x, y, z);
		}
		if (xC == 0)
		{
			x = 0;
			y = 0;
			/*vector<coordtype> point;
			point.push_back(x);
			point.push_back(y);
			point.push_back(z);
			return point;*/
			return MyPoint(x, y, z);
		}
		distype cos_xccpri = GeoUtil::cos_between_ab(xC, CD, xD);
		if (GeoUtil::isInvalid(cos_xccpri))
		{
			y = 0;
			if (xC > xD || (xC < CD && xD < CD))
				x = xC;
			else x = -xC;
		}
		else
		{
			x = cos_xccpri * xC;
			y = sqrt(xC * xC - x * x);
		}
		/*vector<coordtype> point;
		point.push_back(x);
		point.push_back(y);
		point.push_back(z);

		return point;*/
		return MyPoint(x, y, z);
	}
	vector<coordtype> get_point_as2DpointVec(distype CD, distype xC, distype xD)
	{
		coordtype x, y, z = 0;
		if (xD == 0)
		{
			x = CD;
			y = 0;
			vector<coordtype> point;
			point.push_back(x);
			point.push_back(y);
			point.push_back(z);
			return point;
		}
		if (xC == 0)
		{
			x = 0;
			y = 0;
			vector<coordtype> point;
			point.push_back(x);
			point.push_back(y);
			point.push_back(z);
			return point;
		}
		distype cos_xccpri = GeoUtil::cos_between_ab(xC, CD, xD);
		if (GeoUtil::isInvalid(cos_xccpri))
		{
			y = 0;
			if (xC > xD || (xC < CD && xD < CD))
				x = xC;
			else x = -xC;
		}
		else
		{
			x = cos_xccpri * xC;
			y = sqrt(xC * xC - x * x);
		}
		vector<coordtype> point;
		point.push_back(x);
		point.push_back(y);
		point.push_back(z);

		return point;
	}

	virtual bool UpdateClustersID()
	{
		vector<vector<pair<distype, sizetype >>> cluster_mem;
		vector<distype> old_c1st_dis = center_1st_dis;
		get_clusters_upd_1stDis(cluster_mem);

		vector<vector<numclustype>> sorted_centers_listOf;
		nearest_centers_listOf(sorted_centers_listOf);

		Euclidean_Distance dis_3D_euc(3), dis_2D_euc(2);
		SparsVector<size_t> IINDX_O_in_centers3D(the_k, -1);
		bool IsChanged = false;
		for (numclustype the_C = 0; the_C < the_k; the_C++)
		{
			distype dlt_c = deltaC[the_C];
			numclustype the_D = sorted_centers_listOf[the_C][the_k / 2];
			distype CpriDpri = cprime_cprime[the_C][the_D];

			IINDX_O_in_centers3D.reset();
			vector<MyPoint> centers_3d_CD, centers_3d_CCpri;
			vector<size_t> mn_act_cent, dlt_act_cent;
			for (numclustype the_O = 0; the_O < the_k; the_O++)
			{
				if (the_O == the_C)
					continue;
				if (dlt_c == 0 && deltaC[the_O] == 0)
					continue;

				distype DpriOpri = cprime_cprime[the_O][the_D], CpriOpri = cprime_cprime[the_C][the_O];
				distype COpri = ccprime[the_C][the_O];

				MyPoint O3d_CD = get_point_as2Dpoint(CpriDpri, CpriOpri, DpriOpri);
				centers_3d_CD.push_back(O3d_CD);
				mn_act_cent.push_back(the_O);
				if (dlt_c > 0)
				{
					MyPoint O3d_dltC = get_point_as2Dpoint(dlt_c, COpri, CpriOpri);
					centers_3d_CCpri.push_back(O3d_dltC);
				}
				IINDX_O_in_centers3D[the_O] = centers_3d_CD.size() - 1;
			}
			kdt::KDTree<MyPoint> kdtree2D_CD;
			kdtree2D_CD.build(centers_3d_CD);
			SparsVector<distype> lbl2_of_Cands(the_k, 0);
			for (size_t pi = 0; pi < cluster_mem[the_C].size(); pi++)
			{
				sizetype point = cluster_mem[the_C][pi].second;
				distype pC = old_c1st_dis[point];
				distype pCpri = center_1st_dis[point];
				distype pD = this->dis_metric->measure((*inst)[point], centers[the_D].data());

				MyPoint point2d_CD = get_point_as2Dpoint(CpriDpri, pCpri, pD);
				vector<size_t> answer = kdtree2D_CD.radiusSearch(point2d_CD, pCpri);

				lbl2_of_Cands.reset();
				distype pCpri_2 = pCpri * pCpri;
				for (size_t i = 0; i < answer.size(); i++)
					lbl2_of_Cands[mn_act_cent[answer[i]]] = dis_2D_euc.dis2(point2d_CD.data(), centers_3d_CD[answer[i]].data());

				MyPoint point2d_dltC;
				if (dlt_c != 0)
				{
					point2d_dltC = get_point_as2Dpoint(dlt_c, pC, pCpri);

					vector<size_t> cands_indx = lbl2_of_Cands.getActiveIndexes();
					for (size_t oi = 0; oi < cands_indx.size(); oi++)
					{
						numclustype the_O = cands_indx[oi];
						MyPoint O2D_dltC = centers_3d_CCpri[IINDX_O_in_centers3D[the_O]];
						distype dis2 = dis_2D_euc.dis2(point2d_dltC.data(), O2D_dltC.data());
						if (dis2 > lbl2_of_Cands[the_O])
							lbl2_of_Cands[the_O] = dis2;
					}
				}

				vector<pair<distype, numclustype>> cands_fullDis;
				vector<size_t> cands_indx = lbl2_of_Cands.getActiveIndexes();
				for (size_t oi = 0; oi < cands_indx.size(); oi++)
				{
					numclustype the_O = cands_indx[oi];
					distype lbl2 = lbl2_of_Cands[the_O];
					if (lbl2 > pCpri_2)
						continue;
					cands_fullDis.push_back(make_pair(lbl2, the_O));
				}
				sort(cands_fullDis.begin(), cands_fullDis.end());

				numclustype selected_center = the_C;
				distype min_dis2 = pCpri_2;

				for (size_t j = 0; j < cands_fullDis.size(); j++)
				{
					numclustype the_O = cands_fullDis[j].second;
					if (the_O == the_C)
						continue;
					if (cands_fullDis[j].first > min_dis2)
						break;

					distype DpriOpri = cprime_cprime[the_D][the_O];
					distype CDpri = ccprime[the_C][the_D];

					distype COpri = ccprime[the_C][the_O];
					distype CpriOpri = cprime_cprime[the_C][the_O];
					distype CO = cc[the_C][the_O];

					if (the_D != the_O)
					{
						distype nw_lb = 0;
						distype dlt_o = deltaC[the_O];
						get_popri_specialcases(dlt_c, CO, dlt_o, CpriOpri, COpri, pC, pCpri, nw_lb);
						if (nw_lb * nw_lb > min_dis2)
							continue;
					}

					if (dlt_c > 0 && the_D != the_O)
					{
						MyPoint point3d_dltC_original = point2d_dltC;
						MyPoint O2D_dltC = centers_3d_CCpri[IINDX_O_in_centers3D[the_O]];
						update_yz_of_point(dlt_c, COpri, CpriOpri, CDpri, CpriDpri
							, DpriOpri, pD, O2D_dltC, point2d_dltC);
						distype nw_lb2 = dis_3D_euc.dis2(point2d_dltC.data(), O2D_dltC.data());
						point2d_dltC = point3d_dltC_original;
						if (nw_lb2 > min_dis2)
							continue;
					}

					/*if (the_D != the_O)
					{
					MyPoint point3d_CD_original = point2d_CD;
					MyPoint O2D_CD = centers_3d_CD[IINDX_O_in_centers3D[the_O]];

					update_yz_of_point(CpriDpri, CpriOpri, DpriOpri
					,dlt_c, CDpri, COpri, pC, O2D_CD, point2d_CD);
					distype nw_lb2 = dis_3D_euc.dis2(point2d_CD.data(), O2D_CD.data());
					point2d_CD = point3d_CD_original;
					if (nw_lb2 > min_dis2)
					continue;
					}*/


					distype cur_dis2;
					if (the_O != the_D) cur_dis2 = this->dis_metric->measure_2((*inst)[point], centers[the_O].data());
					else cur_dis2 = pD * pD;

					if (cur_dis2 < min_dis2 || (cur_dis2 == min_dis2 && selected_center > the_O))
					{
						IsChanged = true;
						min_dis2 = cur_dis2;
						selected_center = the_O;

					}
				}

				center_1st_dis[point] = sqrt(min_dis2);
				center_1st[point] = selected_center;

			}
		}
		return IsChanged;
	}
public:
	k_means_gs_withFP()
	{
		seedInitializer = 0;
		the_k = 0;
	}
	~k_means_gs_withFP()
	{
	}
private:
	size_t iter_count = 0;
	virtual const vector<numclustype>& apply()
	{
		set_instance(inst);
		set_num_of_clusters(the_k);

		//Euclidean_Distance defult_dis(3);
		Euclidean_Distance defult_dis(inst->dimension());
		if (!this->dis_metric)
			this->dis_metric = &defult_dis;

		dimtype origdim = this->dim;
		this->dim = this->dis_metric->get_dimension();

		initiate_centers();

		iter_count = 1;
		InitialPhase();
		while (iter_count < max_num_iter)
		{
			iter_count++;
			if (!UpdateClustersID())
				break;
			UpdateCenters();
		}

		return center_1st;
	}

	virtual inline void set_instance(const IInstance_Coordinated *inst)
	{
		this->inst = inst; this->size = inst->size(); this->dim = inst->dimension();
		center_1st.assign(inst->size(), maxnumclus);
		center_1st_dis.assign(inst->size(), dismax);
		sizeOf.assign(inst->size(), 0);

		center_2nd_dis.assign(size, dismax);
		center_2nd.assign(size, maxnumclus);
	}

	virtual inline void set_distance_metric(IDistance* dis_metric) { this->dis_metric = dis_metric; }
	virtual inline void set_max_num_iteration(size_t max_num_iter = SIZE_MAX){ this->max_num_iter = max_num_iter; }
	virtual inline void set_num_of_clusters(numclustype the_k){ this->the_k = the_k; }
	virtual inline void set_seeds_initializer(ISeed_Initializer *initializer){ this->seedInitializer = initializer; }

	virtual const vector<numclustype>& get_results()const { return center_1st; }
	virtual inline numclustype get_num_of_clusters()const{ return the_k; }
	virtual size_t get_consumed_iteration_count() const { return iter_count; }
	virtual const coordtype* get_starting_initial_centers(numclustype ci) const { return starting_initial_centers[ci].data(); }

	virtual string name()const { if (!seedInitializer)return "km-gs-withFP"; return "km-gs-withFP-" + seedInitializer->name(); }
};

#endif