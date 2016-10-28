#include "TSP.h"

Cost TSP::c;
vector<Point> TSP::newEdge;

Node::Node(int numRows, int numCols) {
	this->numRows = numRows;

	this->numCols = numCols;
	nodeCosts = new int[numCols]; // Natural indexing
	constraint = new int*[numRows]; // Natural
	for (int i = 0; i < numRows; i++)
	{
		constraint[i] = new int[numCols];
	}
	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < numRows; j++)
		{
			constraint[i][j] = 0;
		}
	}
	// indexing
	trip = new int[numRows];
}

Node::~Node()
{
	delete constraint;
}

// Commands
void Node::assignConstraint(int value, int row, int col) {
	constraint[abs(row)][abs(col)] = value;
	constraint[abs(col)][abs(row)] = value;
}

int Node::assignPoint(Point p, int edgeIndex) {
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
			assignConstraint(-1, abs(pt.getX()), abs(pt.getY()));
		}
		else 
		{
			assignConstraint(1, pt.getX(), pt.getY());
		}
	}
	return edgeIndex;
}

void Node::setConstraint(int** constraint) {
	this->constraint = constraint;
}

void Node::addDisallowedEdges() {
	for (int row = 0; row < numRows; row++) {
		// Count the number of paths from row.
		// If the count exceeds one then disallow all other paths
		// from row
		int count = 0;
		for (int col = 0; col < numCols; col++) {
			if (row != col && constraint[row][col] == 1) {
				count++;
			}
		}
		if (count >= 2) {

			for (int col = 0; col < numCols; col++) {
				if (row != col && constraint[row][col] == 0) {
					constraint[row][col] = -1;
					constraint[col][row] = -1;
				}
			}
		}
	}
	// Check to see whether the presence of a col causes a premature
	// circuit
	for (int row = 0; row < numRows; row++) {
		for (int col = 0; col < numCols; col++) {
			if (row != col && isCycle(row, col) && numCities(b) < numRows) {
				if (constraint[row][col] == 0) {
					constraint[row][col] = -1;
					constraint[col][row] = -1;
				}
			}
		}
	}
}

void Node::addRequiredEdges() {
	for (int row = 0; row < numRows; row++) {
		// Count the number of paths excluded from row
		// If the count equals numCols - 3, include all remaining
		// paths
		int count = 0;
		for (int col = 0; col < numCols; col++) {
			if (row != col && constraint[row][col] == -1) {
				count++;
			}
		}
		if (count >= numRows - 3) {
			for (int col = 0; col < numCols; col++) {
				if (row != col && constraint[row][col] == 0) {
					constraint[row][col] = 1;
					constraint[col][row] = 1;
				}
			}
		}
	}
}

void Node::computeLowerBound()
{
	int lowB = 0;
	for (int row = 0; row < numRows; row++) 
	{
		for (int col = 0; col < numCols; col++) {
			nodeCosts[col] = TSP::c.cost(row, col);
		}
		nodeCosts[row] = 32000;

		// Eliminate edges that are not allowed
		for (int col = 0; col < numCols; col++) {
			if (constraint[row][col] == -1) {
				nodeCosts[col] = 32000; // Taken out of
				// contention
			}
		}
		int* required = new int[numCols - 1]; // Natural indexing
		int numRequired = 0;
		// Determine whether an edge is required
		for (int col = 0; col < numCols; col++) {
			if (constraint[row][col] == 1) {
				numRequired++;
				required[numRequired] = nodeCosts[col];
				nodeCosts[col] = 32000; // Taken out of
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
		if (smallest == 32000) {
			smallest = 0;
		}
		if (nextSmallest == 32000) {
			nextSmallest = 0;
		}
		lowB += smallest + nextSmallest;
	}
	lowerBound = lowB; // This is twice the actual lower bound
}

void Node::setTour()
{
	int path = 0;
	for (int col = 0; col < numCols; col++) {
		if (constraint[0][col] == 1) {
			path = col;
			break;
		}
	}
	tourCost = TSP::c.cost(0, path);
	trip[0] = path;
	int row = 0;
	int col = path;
	int from = row;

	int pos = path;

	nodeAsString = row + " " + col;

	while (pos != row) {
		for (int column = 0; column < numCols; column++) {
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

// Queries
int Node::TourCost()
{
	return tourCost;
}

int* Node::Trip()
{
	return trip;
}

int Node::Constraint(int row, int col)
{
	return constraint[row][col];
}

int** Node::Constraint()
{
	int ** ret = new int*[numCols];
	for (int i = 0; i < numCols; i++)
	{
		ret[i] = new int[numRows];
	}
	for (int i = 0; i < numCols; i++)
	{
		for (int j = 0; j < numRows; j++)
		{
			ret[i][j] = constraint[i][j];
		}
	}

	return ret;
}

int Node::LowerBound()
{
	return lowerBound;
}

bool Node::isTour() {
	// Determine path from 1
	int path = 0;
	for (int col = 1; col < numCols; col++) {
		if (constraint[0][col] == 1) {
			path = col;
			break;
		}
	}
	if (path > 0) {
		bool cycle = isCycle(0, path);
		return cycle && numCities(b) == numRows;
	}
	else {
		return false;
	}
}

bool Node::isCycle(int row, int col) {
	b = new bitset<10000>(numRows + 1);

	for (int i = 0; i < numRows; i++) {
		b->reset(i);
	}
	b->set(row);
	b->set(col);
	int from = row;
	int pos = col;
	int edges = 1;
	bool quit = false;
	while (pos != row && edges < numCols && !quit) 
	{
		quit = true;
		for (int column = 0; column < numCols; column++) {
			if (column != from && constraint[pos][column] == 1) {
				edges++;
				from = pos;
				pos = column;
				b->set(pos);
				quit = false;
				break;
			}
		}
	}
	return pos == row || edges >= numCols;
}

string Node::tour()
{
	return nodeAsString;
}

string Node::toString() {
	// String representation of constraint matrix
	string returnString = "";
	for (int row = 0; row < numRows; row++) {
		for (int col = row + 1; col < numCols; col++) {
			if (constraint[row][col] == 1) {
				//returnString += "" + row + col + " ";
			}
			else if (constraint[row][col] == -1) {
				//returnString += "*" + row + col + " ";
			}
		}
	}
	return returnString;
}

// Internal methods
int Node::Smallest() {
	int s = nodeCosts[0];
	int index = 0;
	for (int i = 1; i < numCols; i++) {
		if (nodeCosts[i] < s) {
			s = nodeCosts[i];
			index = i;
		}
	}
	int temp = nodeCosts[0];
	nodeCosts[0] = nodeCosts[index];
	nodeCosts[index] = temp;
	return nodeCosts[0];
}

int Node::NextSmallest() {
	int ns = nodeCosts[1];
	int index = 1;
	for (int i = 1; i < numCols; i++) {
		if (nodeCosts[i] < ns) {
			ns = nodeCosts[i];
			index = i;
		}
	}
	int temp = nodeCosts[1];
	nodeCosts[1] = nodeCosts[index];
	nodeCosts[index] = temp;
	return nodeCosts[1];
}

int Node::numCities(bitset<10000>* b) {
	int num = 0;
	for (int i = 0; i < numRows; i++) {
		if (b->at(i)) {
			num++;
		}
	}
	return num;
}

TSP::TSP(int** costMatrix, int size, int bestTour, int maxTime)
{
	this->bestTour = bestTour;
	numRows = numCols = size;
	this->maxTime = maxTime;
	c.SetCost(numRows, numCols);
	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++) {
			c.assignCost(costMatrix[row][col], row, col);
		}
	}
}

TSP::TSP(int** costMatrix, int size, int maxTime) {
	numRows = numCols = size;
	this->maxTime = maxTime;
	c = Cost(numRows, numCols);
	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++) {
			c.assignCost(costMatrix[row][col], row, col);
		}
	}
}

void TSP::GenerateSolution() {
	Point* pt;
	// Load newEdge Vector of edge points
	for (int row = 0; row < numRows; row++) {
		for (int col = row + 1; col < numCols; col++) {
			pt = new Point(row, col);
			newEdge.push_back(*pt);
			pt = new Point(-row, -col);
			newEdge.push_back(*pt);
		}
	}
	// Create root node
	Node* root = new Node(numRows, numCols);
	newNodeCount++;
	root->computeLowerBound();

	cout << "Twice the lower bound for root node (no constraints): " << root->LowerBound() << endl;
	// Apply the branch and bound algorithm
	start = clock();
	BranchAndBound(root, -1);
	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
	if (bestNode != NULL) {
		/*cout << "\n\nCost of optimum tour: " << bestTour << "\nOptimum tour: " << bestNode->tour()
			<< "\nTotal of nodes generated: " << newNodeCount << "\nTotal number of nodes pruned: "
			<< numberPrunedNodes << endl;*/
	}
	else {
		cout << "Tour obtained heuristically is the best tour." << endl;
	}
	cout << "Elapsed time for entire algorithm: " << duration << " seconds." << endl;
}

// Queries
int TSP::NodesCreated()
{
	return newNodeCount;
}

int TSP::NodesPruned()
{
	return numberPrunedNodes;
}

string TSP::Tour()
{
	if (bestNode != NULL) {
		return bestNode->tour();
	}
	else {
		return "";
	}
}

int TSP::TourCost()
{
	return bestTour;
}

int* TSP::Trip()
{
	if (bestNode != NULL) {
		return bestNode->trip;
	}
	else {
		return NULL;
	}
}

void TSP::BranchAndBound(Node* node, int edgeIndex)
{
	try
	{
		if (((std::clock() - start) / (double)CLOCKS_PER_SEC / 10000) >= this->maxTime)
		{
			//cout << "\n\nBest final tour cost: " << bestTour << "\nBest final tour: " << bestNode->tour() << endl;
			//return;
		}
		if (node != NULL && edgeIndex < (int)newEdge.size())
		{

			Node *leftChild, *rightChild;
			int leftEdgeIndex = 0, rightEdgeIndex = 0;
			if (node->isTour())
			{
				node->setTour();
				if (node->tourCost < bestTour) {
					bestTour = node->tourCost;
					bestNode = node;
					cout << "\n\nBest tour cost so far: " << bestTour << "\nBest tour so far: "
						<< bestNode->tour() << "\nNumber of nodes generated so far: " << newNodeCount
						<< "\nTotal number of nodes pruned so far: " << numberPrunedNodes
						<< "\nElapsed time to date for branch and bound: " << ((std::clock() - start) / (double)CLOCKS_PER_SEC) << " seconds.\n" << endl;
				}
			}
			else
			{
				if (node->LowerBound() < 2 * bestTour)
				{
					// Create left child node
					leftChild = new Node(numRows, numCols);
					newNodeCount++;
					if (newNodeCount % 1000 == 0)
					{
						Point p = newEdge[edgeIndex];
						duration = ((std::clock() - start) / (double)CLOCKS_PER_SEC);
						cout << "\nTotal number of nodes created so far: " << newNodeCount
							<< "\nTotal number of nodes pruned so far: " << numberPrunedNodes
							<< "\nElapsed time to date for branch and bound: " << duration << " seconds." << endl;
					}
					else if (newNodeCount % 25 == 0) {
						cout << "." << endl;
					}
					if (newNodeCount % 10000 == 0 && bestNode != NULL) {
						cout << "\n\nBest tour cost so far: " << bestTour << "\nBest tour so far: " << bestNode->tour() << endl;
					}
					leftChild->setConstraint(node->Constraint());
					if (edgeIndex != -1 && ((Point)newEdge[edgeIndex]).getX() > 0) {
						edgeIndex += 2;
					}
					else {
						edgeIndex++;
					}
					if (edgeIndex >= newEdge.size()) {
						return;
					}
					Point p = (Point)newEdge[edgeIndex];
					leftEdgeIndex = leftChild->assignPoint(p, edgeIndex);

					leftChild->addDisallowedEdges();
					leftChild->addRequiredEdges();
					leftChild->addDisallowedEdges();
					leftChild->addRequiredEdges();
					leftChild->computeLowerBound();
					if (leftChild->LowerBound() >= 2 * bestTour) {
						leftChild = NULL;
						numberPrunedNodes++;
					}
					// Create right child node
					rightChild = new Node(numRows, numCols);
					newNodeCount++;
					if (newNodeCount % 1000 == 0) {
						cout << "\nTotal number of nodes created so far: " << newNodeCount
							<< "\nTotal number of nodes pruned so far: " << numberPrunedNodes << endl;
					}
					else if (newNodeCount % 25 == 0) {
						cout << "." << endl;
					}
					rightChild->setConstraint(node->Constraint());
					if (leftEdgeIndex >= newEdge.size()) {
						return;
					}
					p = (Point)newEdge[leftEdgeIndex + 1];
					rightEdgeIndex = rightChild->assignPoint(p, leftEdgeIndex + 1);
					rightChild->addDisallowedEdges();
					rightChild->addRequiredEdges();
					rightChild->addDisallowedEdges();
					rightChild->addRequiredEdges();
					rightChild->computeLowerBound();
					if (rightChild->LowerBound() > 2 * bestTour) {
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
						&& leftChild->LowerBound() <= rightChild->LowerBound()) {
						if (leftChild->LowerBound() < 2 * bestTour) {
							BranchAndBound(leftChild, leftEdgeIndex);
						}
						else {
							leftChild = NULL;
							numberPrunedNodes++;
						}

						if (rightChild->LowerBound() < 2 * bestTour) {
							BranchAndBound(rightChild, rightEdgeIndex);
						}
						else {
							rightChild = NULL;
							numberPrunedNodes++;
						}
					}
					else if (rightChild != NULL) {
						if (rightChild->LowerBound() < 2 * bestTour) {
							BranchAndBound(rightChild, rightEdgeIndex);
						}
						else {
							rightChild = NULL;
							numberPrunedNodes++;
						}
						if (leftChild->LowerBound() < 2 * bestTour) {
							BranchAndBound(leftChild, leftEdgeIndex);
						}
						else {
							leftChild = NULL;
							numberPrunedNodes++;
						}
					}
				}
			}
		}
	}
	catch (exception e)
	{
	}
}

int** TSP::copy(int** constraint)
{
	int** toReturn = new int*[numRows];
	for (int i = 0; i < numRows; i++)
	{
		toReturn[i] = new int[numCols];
	}
	for (int row = 0; row < numRows; row++) {
		for (int col = 0; col < numCols; col++) {
			toReturn[row][col] = constraint[row][col];
		}
	}
	return toReturn;
}