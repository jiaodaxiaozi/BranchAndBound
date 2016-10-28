#include "TSPv2.h"

using namespace std;

#define MAX_VALUE 32600
#define MAX_ATTEMPT 100

CCost TSP::c;
vector<Point> TSP::newEdge;

//////////////////////////////////////////////////////////////////////////////

Node::Node(int numRows, int numCols) {
	this->numRows = numRows;
	this->numCols = numCols;
	for (int i = 0; i <= numCols; i++)
	{
		b.push_back(0);
	}
	nodeCosts = new int[numCols + 1]; // Natural indexing
	constraint = new int*[numRows + 1]; // Natural
	for (int i = 0; i < numRows + 1; i++)
	{
		constraint[i] = new int[numCols + 1];
	}
	for (int i = 0; i < numRows + 1; i++)
	{
		for (int j = 0; j < numRows + 1; j++)
		{
			constraint[i][j] = 0;
		}
	}
	// indexing
	trip = new int[numRows + 1];
}

//////////////////////////////////////////////////////////////////////////////

Node::~Node()
{
	b.clear();
	delete[] trip;
	for (int i = 0; i <= numCols; i++)
	{
		delete[] constraint[i];
	}
	delete[] constraint;
	delete[] nodeCosts;
}

//////////////////////////////////////////////////////////////////////////////

// Commands
void Node::AssignConstraint(int value, int row, int col) {
	constraint[row][col] = value;
	constraint[col][row] = value;
}

//////////////////////////////////////////////////////////////////////////////

int Node::AssignPoint(Point p, int edgeIndex) {
	// Advance edgeIndex until edge that is unconstrained is found
	Point pt = p;
	while (edgeIndex < TSP::newEdge.size()
		&& constraint[(int)abs(pt.getX())][(int)abs(pt.getY())] != 0) {
		edgeIndex++;
		if (edgeIndex < TSP::newEdge.size()) {
			pt = (Point)TSP::newEdge[edgeIndex];
		}
	}
	if (edgeIndex < TSP::newEdge.size()) {
		if (pt.getX() < 0) {
			AssignConstraint((int)-1, (int)abs(pt.getX()), (int)abs(pt.getY()));
		}
		else {
			AssignConstraint((int)1, (int)pt.getX(), (int)pt.getY());
		}
	}
	return edgeIndex;
}

//////////////////////////////////////////////////////////////////////////////

void Node::SetConstraint(int** constraint) {
	for (int i = 0; i <= numCols; i++)
	{
		for (int j = 0; j <= numCols; j++)
		{
			this->constraint[i][j] = constraint[i][j];
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

void Node::AddDisallowedEdges() {
	for (int row = 1; row <= numRows; row++) {
		// Count the number of paths from row.
		// If the count exceeds one then disallow all other paths
		// from row
		int count = 0;
		for (int col = 1; col <= numCols; col++) {
			if (row != col && constraint[row][col] == 1) {
				count++;
			}
		}
		if (count >= 2) {

			for (int col = 1; col <= numCols; col++) {
				if (row != col && constraint[row][col] == 0) {
					constraint[row][col] = -1;
					constraint[col][row] = -1;
				}
			}
		}
	}
	// Check to see whether the presence of a col causes a premature
	// circuit
	for (int row = 1; row <= numRows; row++) {
		for (int col = 1; col <= numCols; col++) {
			if (row != col && IsCycle(row, col) && NumCities(b) < numRows) {
				if (constraint[row][col] == 0) {
					constraint[row][col] = -1;
					constraint[col][row] = -1;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

void Node::AddRequiredEdges() {
	for (int row = 1; row <= numRows; row++) {
		// Count the number of paths excluded from row
		// If the count equals numCols - 3, include all remaining
		// paths
		int count = 0;
		for (int col = 1; col <= numCols; col++) {
			if (row != col && constraint[row][col] == -1) {
				count++;
			}
		}
		if (count >= numRows - 3) {
			for (int col = 1; col <= numCols; col++) {
				if (row != col && constraint[row][col] == 0) {
					constraint[row][col] = 1;
					constraint[col][row] = 1;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

void Node::ComputeLowerBound() {
	int lowB = 0;
	for (int row = 1; row <= numRows; row++) {
		for (int col = 1; col <= numCols; col++) {
			nodeCosts[col] = TSP::c.cost(row, col);
		}
		nodeCosts[row] = MAX_VALUE;

		// Eliminate edges that are not allowed
		for (int col = 1; col <= numCols; col++) {
			if (constraint[row][col] == -1) {
				nodeCosts[col] = MAX_VALUE; // Taken out of
				// contention
			}
		}
		int* required = new int[numCols - 1]; // Natural indexing
		int numRequired = 0;
		// Determine whether an edge is required
		for (int col = 1; col <= numCols; col++) {
			if (constraint[row][col] == 1) {
				numRequired++;
				required[numRequired] = nodeCosts[col];
				nodeCosts[col] = MAX_VALUE; // Taken out of
				// contention
			}
		}
		int smallest = 0, nextSmallest = 0;
		if (numRequired == 0) {
			smallest = Smallest();
			nextSmallest = NextSmallest();
		}
		else if (numRequired == 1) {
			smallest = required[1];
			nextSmallest = Smallest();
		}
		else if (numRequired == 2) {
			smallest = required[1];
			nextSmallest = required[2];
		}
		if (smallest == MAX_VALUE) {
			smallest = 0;
		}
		if (nextSmallest == MAX_VALUE) {
			nextSmallest = 0;
		}
		lowB += smallest + nextSmallest;

		delete[] required;
	}
	lowerBound = lowB; // This is twice the actual lower bound
}

//////////////////////////////////////////////////////////////////////////////

void Node::SetTour() {
	int path = 0;
	for (int col = 2; col <= numCols; col++) {
		if (constraint[1][col] == 1) {
			path = (int)col;
			break;
		}
	}
	tourCost = TSP::c.cost(1, path);
	trip[1] = path;
	int row = 1;
	int col = path;
	int from = row;

	int pos = path;
	while (pos != row) {
		for (int column = 1; column <= numCols; column++) {
			if (column != from && constraint[pos][column] == 1) {
				from = pos;
				pos = column;
				nodeAsString += " " + pos;
				tourCost += TSP::c.cost(from, pos);
				trip[from] = pos;
				break;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

// Queries
int Node::TourCost() {
	return tourCost;
}

//////////////////////////////////////////////////////////////////////////////

int* Node::Trip() {
	return trip;
}

//////////////////////////////////////////////////////////////////////////////

int Node::Constraint(int row, int col) {
	return constraint[row][col];
}

//////////////////////////////////////////////////////////////////////////////

int** Node::Constraint() {
	return constraint;
}

//////////////////////////////////////////////////////////////////////////////

int Node::LowerBound() {
	return lowerBound;
}

//////////////////////////////////////////////////////////////////////////////

boolean Node::IsTour() {
	// Determine path from 1
	int path = 0;
	for (int col = 2; col <= numCols; col++) {
		if (constraint[1][col] == 1) {
			path = col;
			break;
		}
	}
	if (path > 0) {
		boolean cycle = IsCycle(1, path);
		return cycle && NumCities(b) == numRows;
	}
	else {
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////

boolean Node::IsCycle(int row, int col) 
{
	for (int i = 0; i <= numCols; i++)
	{
		b[i] = 0;
	}

	b[row] = 1;
	b[col] = 1;

	int from = row;
	int pos = col;
	int edges = 1;
	boolean quit = false;
	while (pos != row && edges <= numCols && !quit) {
		quit = true;
		for (int column = 1; column <= numCols; column++) {
			if (column != from && constraint[pos][column] == 1) {
				edges++;
				from = pos;
				pos = column;
				b[pos] = 1;
				quit = false;
				break;
			}
		}
	}
	return pos == row || edges >= numCols;
}

//////////////////////////////////////////////////////////////////////////////

CString Node::Tour() {
	return nodeAsString;
}

//////////////////////////////////////////////////////////////////////////////

// Internal methods
int Node::Smallest() {
	int s = nodeCosts[1];
	int index = 1;
	for (int i = 2; i <= numCols; i++) {
		if (nodeCosts[i] < s) {
			s = nodeCosts[i];
			index = i;

		}
	}
	int temp = nodeCosts[1];
	nodeCosts[1] = nodeCosts[index];
	nodeCosts[index] = temp;
	return nodeCosts[1];
}

//////////////////////////////////////////////////////////////////////////////

int Node::NextSmallest() {
	int ns = nodeCosts[2];
	int index = 2;
	for (int i = 2; i <= numCols; i++) {
		if (nodeCosts[i] < ns) {
			ns = nodeCosts[i];
			index = i;
		}
	}
	int temp = nodeCosts[2];
	nodeCosts[2] = nodeCosts[index];
	nodeCosts[index] = temp;
	return nodeCosts[2];
}

//////////////////////////////////////////////////////////////////////////////

int Node::NumCities(vector<int> b) {
	int num = 0;
	for (int i = 1; i <= numRows; i++) {
		if (b.at(i)) {
			num++;
		}
	}
	return num;
}

//////////////////////////////////////////////////////////////////////////////

/**
* TSP Branch and Bound
*/
TSP::TSP(int** costMatrix, int size, int bestTour, int maxTime) {
	this->maxTime = maxTime;
	this->bestTour = bestTour;
	numRows = numCols = size;
	bestNode = NULL;
	c.SetCost(numRows, numCols);
	for (int row = 1; row <= size; row++) {
		for (int col = 1; col <= size; col++) {
			c.assignCost(costMatrix[row][col], row, col);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

TSP::TSP(int** costMatrix, int size, int maxTime) {
	this->maxTime = maxTime;
	numRows = numCols = size;
	c.SetCost(numRows, numCols);
	bestNode = NULL;
	for (int row = 1; row <= size; row++) {
		for (int col = 1; col <= size; col++) {
			c.assignCost(costMatrix[row][col], row, col);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

void TSP::generateSolution() {
	Point* pt;
	// Load newEdge Vector of edge points
	for (int row = 1; row <= numRows; row++) {
		for (int col = row + 1; col <= numCols; col++) {
			pt = new Point(row, col);
			newEdge.push_back(*pt);
			pt = new Point(-row, -col);
			newEdge.push_back(*pt);
		}
	}
	// Create root node
	Node* root = new Node(numRows, numCols);
	newNodeCount++;
	root->ComputeLowerBound();

	start = clock();
	BranchAndBound(root, -1);
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	delete root;
}

//////////////////////////////////////////////////////////////////////////////

// Queries
int TSP::nodesCreated() {
	return newNodeCount;
}

//////////////////////////////////////////////////////////////////////////////

int TSP::nodesPruned() {
	return numberPrunedNodes;
}

//////////////////////////////////////////////////////////////////////////////

CString TSP::tour() {
	if (bestNode != NULL) {
		return bestNode->Tour();
	}
	else {
		return "";
	}
}

//////////////////////////////////////////////////////////////////////////////

int TSP::tourCost() {
	return bestTour;
}

//////////////////////////////////////////////////////////////////////////////

int* TSP::trip() {
	if (bestNode != NULL) {
		return bestNode->Trip();
	}
	else {
		return NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////

void TSP::BranchAndBound(Node* node, int edgeIndex) {
	counter++;
	if (counter >= MAX_ATTEMPT)
	{
		isFound = true;
	}
	if (((std::clock() - start) / (double)CLOCKS_PER_SEC) > maxTime) {
		if (!isFound)
		{
			bestTour = -1;
		}

		return;
	}
	if (node != NULL && edgeIndex < (int)newEdge.size())
	{
		int leftEdgeIndex = 0, rightEdgeIndex = 0;
		if (node->IsTour()) 
		{
			node->SetTour();
			if (node->TourCost() < bestTour)
			{
				bestTour = node->TourCost();
				if (bestNode != NULL)
				{
					//delete bestNode;
				}
				bestNode = node;
				cout << "Current solution: " << bestTour << endl;
				counter = 0;
			}
		}
		else 
		{
			if (node->LowerBound() < 2 * bestTour) 
			{
				// Create left child node
				Node* leftChild = new Node(numRows, numCols);
				newNodeCount++;
				leftChild->SetConstraint(node->Constraint());
				if (edgeIndex != -1 && ((Point)newEdge[edgeIndex]).getX() > 0) {
					edgeIndex += 2;
				}
				else {
					edgeIndex++;
				}
				if (edgeIndex >= newEdge.size()) {
					delete leftChild;
					leftChild = NULL;
					return;
				}
				Point p = (Point)newEdge[edgeIndex];
				leftEdgeIndex = leftChild->AssignPoint(p, edgeIndex);

				leftChild->AddDisallowedEdges();
				leftChild->AddRequiredEdges();
				leftChild->ComputeLowerBound();
				if (leftChild->LowerBound() >= 2 * bestTour) {
					delete leftChild;
					leftChild = NULL;
					numberPrunedNodes++;
				}
				// Create right child node
				Node* rightChild = new Node(numRows, numCols);
				newNodeCount++;
				rightChild->SetConstraint(node->Constraint());
				if (leftEdgeIndex >= newEdge.size()) {
					delete rightChild;
					rightChild = NULL;
					delete leftChild;
					leftChild = NULL;
					return;
				}
				p = (Point)newEdge[leftEdgeIndex + 1];
				rightEdgeIndex = rightChild->AssignPoint(p, leftEdgeIndex + 1);
				rightChild->AddDisallowedEdges();
				rightChild->AddRequiredEdges();
				rightChild->ComputeLowerBound();
				if (rightChild->LowerBound() > 2 * bestTour) {
					delete rightChild;
					rightChild = NULL;
					numberPrunedNodes++;
				}
				if (leftChild != NULL && rightChild == NULL) {
					BranchAndBound(leftChild, leftEdgeIndex);
				}
				else if (leftChild == NULL && rightChild != NULL) {
					BranchAndBound(rightChild, rightEdgeIndex);
				}
				else if (leftChild != NULL && rightChild != NULL
					&& leftChild->LowerBound() <= rightChild->LowerBound()) 
				{
					if (leftChild->LowerBound() < 2 * bestTour) {
						BranchAndBound(leftChild, leftEdgeIndex);
					}
					else 
					{
						delete leftChild;
						leftChild = NULL;
						numberPrunedNodes++;
					}

					if (rightChild->LowerBound() < 2 * bestTour) {
						BranchAndBound(rightChild, rightEdgeIndex);
					}
					else 
					{
						delete rightChild;
						rightChild = NULL;
						numberPrunedNodes++;
					}
				}
				else if (rightChild != NULL)
				{
					if (rightChild->LowerBound() < 2 * bestTour) {
						BranchAndBound(rightChild, rightEdgeIndex);
					}
					else 
					{
						delete rightChild;
						rightChild = NULL;
						numberPrunedNodes++;
					}
					if (leftChild->LowerBound() < 2 * bestTour) 
					{
						BranchAndBound(leftChild, leftEdgeIndex);
					}
					else 
					{
						delete leftChild;
						leftChild = NULL;
						numberPrunedNodes++;
					}
				}

				if (leftChild != NULL)
				{
					delete leftChild;
					leftChild = NULL;
				}
				if (rightChild != NULL)
				{
					delete rightChild;
					rightChild = NULL;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

/*int** TSP::copy(int** constraint) {
	int** toReturn = new int*[numRows + 1];
	for (int row = 0; row <= numRows; row++) {
		toReturn[row] = new int[numCols + 1];
	}
	for (int row = 0; row <= numRows; row++) {
		for (int col = 0; col <= numCols; col++) {
			toReturn[row][col] = constraint[row][col];
		}
	}
	return toReturn;
}*/

//////////////////////////////////////////////////////////////////////////////

int TSP::GetBestTour()
{
	return bestTour;
}

//////////////////////////////////////////////////////////////////////////////
