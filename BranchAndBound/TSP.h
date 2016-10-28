#include <iostream>
#include <fstream>
#include <string>
#include <atlstr.h>  
#include <regex>
#include <list>
#include <bitset>

#include "Utils.h"

using namespace std;

class Node
{
// Fields
public:
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
	string nodeAsString; // Used by isTour query
	bool isLoop = false;
	bitset<10000>* b; // Used by isCycle and initialized in TSPUI
	// Constructor

public:
	Node(int numRows, int numCols);
	~Node();

	// Commands
	void assignConstraint(int value, int row, int col);

	int assignPoint(Point p, int edgeIndex);

	void setConstraint(int** constraint);

	void addDisallowedEdges();

	void addRequiredEdges();

	void computeLowerBound();

	void setTour();

	// Queries
	int TourCost();

	int* Trip();

	int Constraint(int row, int col);

	int** Constraint();

	int LowerBound();

	bool isTour();

	bool isCycle(int row, int col);

	string tour();

	string toString();

	// Internal methods
	int Smallest();

	int NextSmallest();

	int numCities(bitset<10000>* b);
};

class TSP
{
public:

	TSP(int** costMatrix, int size, int bestTour, int maxTime);

	TSP(int** costMatrix, int size, int maxTime);

	void GenerateSolution();

	// Queries
	int NodesCreated();

	int NodesPruned();

	string Tour();

	int TourCost();

	int* Trip();

	void BranchAndBound(Node* node, int edgeIndex);

	int** copy(int** constraint);

public:
	static Cost c;
	static vector<Point> newEdge;
private:
	int numRows;

	int numCols;
	std::clock_t start;
	double duration = 0;
	int bestTour = 999999999 / 4;
	Node* bestNode;
	// Contains objects of type Point
	int newNodeCount = 0;
	int numberPrunedNodes = 0;
	int maxTime = 999999;
};