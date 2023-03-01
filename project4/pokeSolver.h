#include <iostream>
#include <getopt.h>
#include <vector>
#include <limits>
#include <queue>
#include <iomanip>
#include <cmath>
#include <string>
#include <set>
#include <algorithm>
#include <climits>
#include <numeric>

using std::string; 
using std::vector; 
using std::cout; 
using std::cin; 
using std::priority_queue;
using std::set;
using std::deque; 

class pokeSolver {
	enum class Mode {
		MST = 'M', 
		FTSP = 'F',
		OTSP = 'O'
	};

	struct v {
		int x;
		int y; 
		bool used = false; 
		double distance = std::numeric_limits<double>::infinity(); 
		int closest = -1; 
	};

	struct vb {
		int x; 
		int y; 
	};

	public: 
		int getOptions(int argc, char** argv);
		int runMode();


	private:

		//
		// GENERAL INPUTS 
		//
		void readVerticies();
		int validateOptionInput();


		/*  
		//  MST FUNCTIONS AND HELPERS
		*/
		int runMST();
		void findMST();
		int getNextVert();
		void outputMST();
		// helpers 
		bool checkValidMSTSet();
		bool MSTcanReach(int i, int index);
		bool onWater(int i);
		bool onLand(int i);
		// vars
		int landVerts = 0;
		int waterVerts = 0;
		int edgeVerts = 0;


		/*
		//  FASTTSP FUNCTIONS AND HELPERS
		*/
		int runFTSP();
		//void findMSTUtil(); 
		//void createAdjList();
		void readVerticiesB(vector<vb> & v);
		int findNext(int cur); 
		double findNextValue(int cur);
		void arbitraryInsert(vector<int> & tour, vector<bool> & used);
		void outputTSP(vector<int> & v);
		double distanceB(int v1, int v2);
		//void findEulerianCycle(vector<int> & v); 
		//void findHamiltonianCycle(vector<int> & v);
		//void twoOptSmall(vector<int> & v);
		//void twoOptLarge(vector<int> & v); 
		//double tourLength(vector<int> & v); 
		double sum = 0; 
		vector<vb> bvec;
		vector<double> distances; 
		
		/*
		//  OPTTSP FUNCTIONS AND HELPERS
		*/
		void setZero(); 
		int runOTSP();

		vector<vector<double>> dmat; 
		void fillDMAT(); 

		void genPerms(int permLength);
		bool promising(int permLength); 
		double tourLength(int permLength);
		double mstSize(int permLength);
		double groupSeparation(int permLength);

		vector<bool> contained; 

		vector<int> path; 
		vector<int> pathBest; 
		double lenBest; 
		double len; 
		int permcnt = 0;

		/*
		//  SHARED FUNCTIONS AND VARIABLES
		*/
		vector<vector<int>> adjList;
		void printAdjList(); 

		int spanSize = 0;
		int edges = 0;
		double estimate; 
		double tempdbl; 
		int TSPSTART = 0; 
		int verts = 0;
		char mode = '\0';
		bool help = false;

		static double distanceCalc(const v & a, const v & b); 
		double distanceSum = 0; 
		

		vector<v> verticies;

		//struct distFunctor { 
		//	int inx;
		//	vector<v> & vec;
		//	bool operator()(const int a, const int b) {
		//		return distanceCalc(vec[a], vec[inx]) < distanceCalc(vec[b], vec[inx]);
		//	}
		//};
};

