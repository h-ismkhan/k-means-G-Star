//In the name of Allah
//Author: Hassan Ismkhan (esmkhan@gmail.com)

#include<iostream>
using namespace std;
#include<numeric>

#include "headers/Instance/Inst-CoordpLine.h"
#include "headers/Distance/Euclidean-Distance.h"
#include "headers/Clustering/k-means-gs-withFP.h"
#include "headers/Clustering/k-means-gs.h"
#include "headers/Clustering/k-means-initializers/Seeds-Random-Initializer.h"
#include "headers/Distance/Euclidean-Distance.h"
#include "headers/Clustering/evaluation/SSEDM.h"
#include "headers/Utils/myTimer.h"

int main()
{
	cout << "In the name of God!" << endl;
	cout << "Welcome to test KM-G*!" << endl << endl;

	size_t max_itr = 10;
	size_t the_k = 10;
	Inst_CoordpLine inst("iris.cl");
	
	Seeds_Random_Init randInit;
	randInit.set_instance(&inst);
	randInit.set_the_k(the_k);

	Euclidean_Distance dis_m(inst.dimension());

	k_means_gs km;
	km.set_instance(&inst);
	km.set_num_of_clusters(the_k);
	km.set_distance_metric(&dis_m);
	km.set_seeds_initializer(&randInit);
	km.set_max_num_iteration(max_itr);

	MyChronometer mytimer;
	mytimer.start();
	const vector<numclustype> result = km.apply();
	mytimer.end();

	SSEDM_Measure ssedm_m;
	ssedm_m.set_instance(&inst);
	ssedm_m.set_clusterIds(result.data());
	ssedm_m.set_distance_metric(&dis_m);
	double ssedm_val = ssedm_m.measure();

	cout << "Instance name:	" << inst.name() << endl;
	cout << "Instance dimension:	" << inst.dimension() << endl;
	cout << "Instance #points:	" << inst.size() << endl;
	cout << "Instance the k:	" << the_k << endl;
	cout << endl;

	cout << "After " << km.get_consumed_iteration_count();
	cout << "number of iterations, the exact k-means obtains ssedm value: " << fixed << ssedm_val;
	cout << ", while the runtime is " << (size_t)round(mytimer.duration_recent() * 1000);
	cout << "(ms), and number of distance computation is " << dis_m.counter() << endl;

	getch();

	return 0;
}