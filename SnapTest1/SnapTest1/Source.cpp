#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include "timers.h"
//#include <omp.h> //Can only use openMP on Mac and Linux also have to define USE_OPENMP

//example how to make a graph
void populateWeightedGraph(PNEANet Graph, TFltV& attrForSSSP, const TStr& attrForPageRank)
{
	//Nodes must be added to graph before edges
	for (int i = 0; i < 9; i++)
	{
		Graph->AddNode();
	}

	//For weighted dataset depending on method being used two different ways to input edge weight
	//AddFltAttrDate() adds label to edgeId of type attrForPageRank
	//attrForSSSP is a vector which holds edge weights
	Graph->AddEdge(0, 1);
	Graph->AddFltAttrDatE(0, 15, attrForPageRank);
	attrForSSSP.Add(15);

	Graph->AddEdge(1, 0);
	Graph->AddFltAttrDatE(1, 15, attrForPageRank);
	attrForSSSP.Add(15);

	Graph->AddEdge(0, 2);
	Graph->AddFltAttrDatE(2, 25, attrForPageRank);
	attrForSSSP.Add(25);

	Graph->AddEdge(2, 0);
	Graph->AddFltAttrDatE(3, 25, attrForPageRank);
	attrForSSSP.Add(25);

	Graph->AddEdge(1, 3);
	Graph->AddFltAttrDatE(4, 5, attrForPageRank);
	attrForSSSP.Add(5);

	Graph->AddEdge(3, 1);
	Graph->AddFltAttrDatE(5, 5, attrForPageRank);
	attrForSSSP.Add(5);

	Graph->AddEdge(1, 4);
	Graph->AddFltAttrDatE(6, 25, attrForPageRank);
	attrForSSSP.Add(25);

	Graph->AddEdge(4, 1);
	Graph->AddFltAttrDatE(7, 25, attrForPageRank);
	attrForSSSP.Add(25);

	Graph->AddEdge(1, 5);
	Graph->AddFltAttrDatE(8, 10, attrForPageRank);
	attrForSSSP.Add(10);

	Graph->AddEdge(5, 1);
	Graph->AddFltAttrDatE(9, 10, attrForPageRank);
	attrForSSSP.Add(10);

	Graph->AddEdge(2, 6);
	Graph->AddFltAttrDatE(10, 10, attrForPageRank);
	attrForSSSP.Add(10);

	Graph->AddEdge(6, 2);
	Graph->AddFltAttrDatE(11, 10, attrForPageRank);
	attrForSSSP.Add(10);

	Graph->AddEdge(2, 5);
	Graph->AddFltAttrDatE(12, 20, attrForPageRank);
	attrForSSSP.Add(20);

	Graph->AddEdge(5, 2);
	Graph->AddFltAttrDatE(13, 20, attrForPageRank);
	attrForSSSP.Add(20);

	Graph->AddEdge(4, 3);
	Graph->AddFltAttrDatE(14, 15, attrForPageRank);
	attrForSSSP.Add(15);

	Graph->AddEdge(3, 4);
	Graph->AddFltAttrDatE(15, 15, attrForPageRank);
	attrForSSSP.Add(15);

	Graph->AddEdge(7, 5);
	Graph->AddFltAttrDatE(16, 10, attrForPageRank);
	attrForSSSP.Add(10);

	Graph->AddEdge(5, 7);
	Graph->AddFltAttrDatE(17, 10, attrForPageRank);
	attrForSSSP.Add(10);

	Graph->AddEdge(5, 8);
	Graph->AddFltAttrDatE(18, 5, attrForPageRank);
	attrForSSSP.Add(5);

	Graph->AddEdge(8, 5);
	Graph->AddFltAttrDatE(19, 5, attrForPageRank);
	attrForSSSP.Add(5);
}

//Using a small dataset to test against nvGraph results
void weightedGraphTests()
{
	//Variables for weighted graph
	PNEANet Graph = PNEANet::New(); //Smart pointer for Graph type TNEANet which is multidirectional directed graph with labels
	TStr attr = "float";
	TFltV weightVector; //Vector holds edge weights
	TIntFltH NIdDistH; //THash<TInt, TFloat> holds results returned from Shortest Path
	TIntFltH PRankH; //holds results for Page Rank

	//Use already defined dataset
	populateWeightedGraph(Graph, weightVector, attr);

	printf("====================== Graph Information =======================\n");
	//Displays information about graph in console
	Graph->Dump();
	printf("====================== End Graph Information =======================\n\n\n");


	//Can only be used with graphs where node order is defined in dfs order
	//Otherwise it will cause an out of bounds in the Thash
	//This is only the case for a weighted graph
	TSnap::GetWeightedShortestPath(Graph, 0, NIdDistH, weightVector);

	printf("====================== Single Source Shortest Path =======================\n");
	for (int i = 0; i < NIdDistH.Len(); i++)
	{
		printf("Distance for node %d, %f\n", i, NIdDistH.GetDat(i));
	}
	printf("======================  End Single Source Shortest Path ==================\n\n\n");

	//This does not have same restriction as shortestPath, attr defines label type on edge
	TSnap::GetWeightedPageRank(Graph, PRankH, attr);

	printf("====================== Page Rank =======================\n");
	for (int i = 0; i < NIdDistH.Len(); i++)
	{
		printf("Page rank of node %d, %f\n", i, PRankH.GetDat(i));
	}
	printf("====================== End Page Rank =======================\n\n\n");
}

//Large unweighted dataset for timing tests to compare against nvGraph
void unweightedGraphTests()
{
	//Load data file into type PNGraph. Note PNGraph is directed without labels
	PNGraph Graph = TSnap::LoadEdgeList<PNGraph>("web-Google.txt", 0, 1);
	TIntV shortDists; //Vector to hold distances
	TIntH NidToDistH; // THash to hold dinstance results
	TIntFltH PageRank;// THash to hold PageRank results

	printf("Data Loaded\n");

	//sssp implmenation 1
	StartCounter();
	TSnap::GetShortestDistances(Graph, 1, true, false, shortDists);
	printf("The time for non parellel sssp was %f\n", GetCounter());

	//sssp implementation 2
	StartCounter();
	int length = TSnap::GetShortPath(Graph, 2, NidToDistH, true);
	printf("The time for non parallel second implemenation sssp was %f\n", GetCounter());

	//OpenMP implementation but only works on gcc linux and mac OS
	//TSnap::GetShortestDistancesMP(Graph, 1, true, false, shortDists);

	//Page rank implemenation 1
	StartCounter();
	TSnap::GetPageRank(Graph, PageRank);
	printf("The time for non parellel page rank was %f\n", GetCounter());

	//Page rank implenation 2 this one is not recommended because it is unoptimized
	StartCounter();
	TSnap::GetPageRank_v1(Graph, PageRank);
	printf("The time for non parellel v1 page rank was %f\n", GetCounter());

	//OpenMP implementation but only works on gcc linux and mac OS
	//TSnap::GetPageRankMP(Graph, PageRank);
}


int main(int argc, char* argv[])
{
	//Tests a small weighted dataset and prints results to compare against nvGraph
	weightedGraphTests();

	//Large Unweighted Graph Test used for timing results
	unweightedGraphTests();

	getchar();
	return 0;
}

