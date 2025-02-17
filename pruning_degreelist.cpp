﻿#include<Eigen/Sparse>
#include<Eigen/Dense>
#include<iostream>
#include<fstream>
#include<ctime>
#include <cstdlib>
using namespace Eigen;
using namespace std;


struct Node {                                
	int next, prev;
};


int main(int argc, char** argv)
{
	string data_in = argv[1];
	int weighted_type = atoi(argv[2]);               
	int graph_type = atoi(argv[3]);                           
	ifstream in("./data/" + data_in);
	cout << "the data name is:" << data_in << endl;
	if (!in) {
		cout << "fail to open the file" << endl;
		return -1;
	}
	else
		cout << "opened the file" << endl;
	string comment;
	getline(in, comment);									
	char s;
	in >> s;
	int edge_num, m, n;									    
	in >> edge_num >> m >> n;
	cout << "edge_num=" << edge_num << "  " << "m=" << m << "  " << "n=" << n << "  " << endl;
	int max_m = 0, max_n = 0;
	int max_size = 0;
	vector<Triplet<int>>triplets;
	if (graph_type == 0) {
		if (weighted_type == 0) {
			int a, b;
			while (in >> a >> b)                  
			{
				if (a == b)
					continue;
				triplets.emplace_back(a - 1, b - 1, 1);
				triplets.emplace_back(b - 1, a - 1, 1);                      
				max_m = max_m > a ? max_m : a;
				max_n = max_n > b ? max_n : b;
			}
		}
		else if (weighted_type == 1) {
			int a, b, c;
			while (in >> a >> b >> c)                 
			{
				if (a == b)
					continue;
				triplets.emplace_back(a - 1, b - 1, c);
				triplets.emplace_back(b - 1, a - 1, c);                      
				max_m = max_m > a ? max_m : a;
				max_n = max_n > b ? max_n : b;
			}
		}
		else if (weighted_type == 2) {          
			int a, b, c;
			while (in >> a >> b >> c)
			{
				if (a == b)
					continue;
				triplets.emplace_back(a - 1, b - 1, 1);
				triplets.emplace_back(b - 1, a - 1, 1);                       
				max_m = max_m > a ? max_m : a;
				max_n = max_n > b ? max_n : b;
			}
		}
		else if (weighted_type == 3) {          
			int a, b, c, d;
			while (in >> a >> b >> c >> d)
			{
				if (a == b)
					continue;
				triplets.emplace_back(a - 1, b - 1, c);
				triplets.emplace_back(b - 1, a - 1, c);                       
				max_m = max_m > a ? max_m : a;
				max_n = max_n > b ? max_n : b;
			}
		}
		else {
			cout << "Unrecognized type" << endl;
			return 0;
		}
		max_size = max_m > max_n ? max_m : max_n;
	}
	else if (graph_type == 1) {
		if (weighted_type == 0) {
			int a, b;
			while (in >> a >> b)                                          
			{
				max_m = max_m > a ? max_m : a;
				max_n = max_n > b ? max_n : b;
				b = m + b;
				triplets.emplace_back(a - 1, b - 1, 1);
				triplets.emplace_back(b - 1, a - 1, 1);                       
			}
		}
		else if (weighted_type == 1) {
			int a, b, c;
			while (in >> a >> b >> c)                 
			{
				max_m = max_m > a ? max_m : a;
				max_n = max_n > b ? max_n : b;
				b = m + b;
				triplets.emplace_back(a - 1, b - 1, c);
				triplets.emplace_back(b - 1, a - 1, c);                      

			}
		}
		else if (weighted_type == 2) {          
			int a, b, c;
			while (in >> a >> b >> c)
			{
				max_m = max_m > a ? max_m : a;
				max_n = max_n > b ? max_n : b;
				b = m + b;
				triplets.emplace_back(a - 1, b - 1, 1);
				triplets.emplace_back(b - 1, a - 1, 1);                       
			}
		}
		else if (weighted_type == 3) {          
			int a, b, c, d;
			while (in >> a >> b >> c >> d)
			{
				max_m = max_m > a ? max_m : a;
				max_n = max_n > b ? max_n : b;
				b = m + b;
				triplets.emplace_back(a - 1, b - 1, c);
				triplets.emplace_back(b - 1, a - 1, c);                       
			}
		}
		else {
			cout << "Unrecognized type" << endl;
			return 0;
		}
		max_size = m + n;
	}
	cout << "max_m is:" << max_m << "  " << "max_n is:" << max_n << endl;
	SparseMatrix<int, RowMajor>A(max_size, max_size);                       
	A.setFromTriplets(triplets.begin(), triplets.end());         
	if (weighted_type == 0 || weighted_type == 2) {
		for (int i = 0; i < A.nonZeros(); i++)
			A.valuePtr()[i] = 1;
	}
	A.makeCompressed();
	int edge_sum = A.sum();
	int node_num = A.rows();
	clock_t start, end;
	start = clock();
	double bound = (double)edge_sum / (2 * node_num);
	int length = A.rows();
	vector<int>A_cut;
	int* cut_index = new int[length]();                    
	int* degree = new int[length]();
	int max_degree=0;
	for (int i = 0; i < length; i++) {
		degree[i] = A.row(i).sum();   
		max_degree = max_degree > degree[i] ? max_degree:degree[i];
		if (degree[i] < bound) {
			A_cut.push_back(i);
			cut_index[i] = 1;
		}
	}
	Node* node = new Node[length];
	int* degreelist = new int[max_degree+1]();
	for (int i = 0; i <= max_degree; i++)
		degreelist[i] = -1;
	for (int i = 0; i <length; i++) {
		node[i].next = degreelist[degree[i]];
		node[i].prev = -1;
		if (degreelist[degree[i]] != -1)
			node[degreelist[degree[i]]].prev= i;
		degreelist[degree[i]] = i;
	}
	int iters = 1;
	int*is_dealed = new int[length]();                         
	while (A_cut.size() != 0) {
		int* degree_decrease = new int[length]();
		int edge_reduction = 0;
		vector<int>neighbour;
		for (int i = 0; i < A_cut.size(); i++) {
			edge_reduction += degree[A_cut[i]];
			for (int j = A.outerIndexPtr()[A_cut[i]]; j < A.outerIndexPtr()[A_cut[i] + 1]; j++) {
				int index = A.innerIndexPtr()[j];
				if (cut_index[index] == 0) {
					degree_decrease[index] += 1;
					if (is_dealed[index] == iters)
						continue;
					is_dealed[index] = iters;
					neighbour.push_back(index);
				}
			}
		}
		for (int i = 0; i < neighbour.size(); i++) {
			degree[neighbour[i]] -= degree_decrease[neighbour[i]];
			edge_reduction += degree_decrease[neighbour[i]];
		}
		int deg = floor(bound);
		degreelist[deg] = -1;
		node_num -= A_cut.size();
		edge_sum -= edge_reduction;
		bound = (double)edge_sum / (2 * node_num);
		A_cut.clear();
		for (int i = 0; i < neighbour.size(); i++) {
			int index = neighbour[i];
			if (node[index].prev != -1)
				node[node[index].prev].next = node[index].next;
			else
				degreelist[degree[index] + degree_decrease[index]] = node[index].next;
			if (node[index].next != -1)
				node[node[index].next].prev = node[index].prev;
			if (degree[index] > bound) {
				node[index].next = degreelist[degree[index]];
				node[index].prev = -1;
				if (degreelist[degree[index]] != -1)
					node[degreelist[degree[index]]].prev = index;
				degreelist[degree[index]] = index;
			}
			else {
				node[index].next = degreelist[deg];
				node[index].prev = -1;
				if (degreelist[deg] != -1)
					node[degreelist[deg]].prev = index;
				degreelist[deg] = index;
			}
		}
		for (int i = deg; i <= floor(bound); i++) {
			int index = degreelist[i];
			while (index != -1) {
				cut_index[index] = 1;
				A_cut.push_back(index);
				index = node[index].next;
			}
		}
		iters++;
	}
	cout << "bound is:" << bound << " node_num is:" << node_num << endl;
	end = clock();
	delete[]cut_index;
	delete[]degree;
	double time = (double)(end - start) / CLOCKS_PER_SEC;
	cout << "weak_boundcut cost time is:" << time << "s" << endl;
	cout << endl;
	return 0;
}
