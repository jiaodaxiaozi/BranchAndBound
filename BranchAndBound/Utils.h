class Point
{
public:
	int x, y;

	Point() : x(0), y(0) {}
	Point(int x, int y) : x(x), y(y) {}
	int getX() { return x; }
	int getY() { return y; }
};

class CCost 
{
private:
	int** costMatrix;
	int dim = 0;

public:
	CCost() {}

	CCost(int numRows, int numCols)
	{
		dim = numCols;
	}

	void SetCost(int numRows, int numCols)
	{
		dim = numCols;
		if (costMatrix != nullptr)
		{
			for (int i = 0; i <= numRows; i++)
			{
				delete[] costMatrix[i];
			}
			delete[] costMatrix;
		}

		costMatrix = new int*[numRows + 1];
		for (int i = 0; i <= numRows; i++)
		{
			costMatrix[i] = new int[numCols + 1];
		}
		for (int i = 0; i <= numRows; i++)
		{
			for (int j = 0; j <= numCols; j++)
			{
				costMatrix[i][j] = 0;
			}
		}
	}

	~CCost()
	{
		for (int i = 0; i <= dim; i++)
		{
			delete[] costMatrix[i];
		}
		delete[] costMatrix;
	}

	void assignCost(int s, int row, int col)
	{
		costMatrix[row][col] = s;
	}

	int cost(int row, int col)
	{
		return costMatrix[row][col];
	}
};
