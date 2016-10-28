#include <iostream>
#include <fstream>
#include <string>
#include <atlstr.h>  
#include <regex>
#include <list>
#include <bitset>
#include <memory>

#include "Utils.h"

using namespace std;

#define MAX_VALUE 32600

typedef std::tr1::shared_ptr<vector<int>> BitsetPtr;

class Node {
	// Fields
private:
	int lowerBound;
	int numRows, numCols;
	int** constraint;
	// -1 indicates no edge from row to column allowed,
	// 1 indicates that edge from row to column required,
	// 0 indicates that edge from row to column allowed
	int* nodeCosts; // Used to compute smallest and
	// nexSmallest
	int edges; // Used by isTour query
	int tourCost; // Used by isTour query
	int* trip;
	CString nodeAsString; // Used by isTour query
	boolean isLoop = false;
	BitsetPtr b; // Used by isCycle and initialized in TSPUI
	// Constructor

public:
	Node(int numRows, int numCols);

	~Node();

	// Commands
	void AssignConstraint(int value, int row, int col);

	int AssignPoint(Point p, int edgeIndex);

	void SetConstraint(int** constraint);

	void AddDisallowedEdges();

	void AddRequiredEdges();

	void ComputeLowerBound();

	void SetTour();

	// Queries
	int TourCost();

	int* Trip();

	int Constraint(int row, int col);

	int** Constraint();

	int LowerBound();

	boolean IsTour();

	boolean IsCycle(int row, int col);

	CString Tour();

	CString ToString();

	// Internal methods
	int Smallest();

	int NextSmallest();

	int NumCities(const BitsetPtr& b);
};

typedef std::shared_ptr<Node> NodePtr;

/**
* TSP Branch and Bound
*/

class TSP {
	// Fields
private: 

	int numRows;
	int numCols;
	int bestTour = MAX_VALUE / 4;
	NodePtr bestNode;
	// Contains objects of type Point
	int newNodeCount = 0;
	int numberPrunedNodes = 0;
	int counter = 0;
	clock_t start;
	double duration;
	int maxTime = -1;
	bool isFound = false;

public:

	static CCost c;
	static vector<Point> newEdge;

	TSP(int** costMatrix, int size, int bestTour, int maxTime);

	TSP(int** costMatrix, int size, int maxTime);

	void generateSolution();

	// Queries
	int nodesCreated();

	int nodesPruned();

	CString tour();

	int tourCost();

	int* trip();

	void BranchAndBound(const NodePtr& node, int edgeIndex);

	int** copy(int** constraint);

	int GetBestTour();
};