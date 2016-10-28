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

public:
	CCost() {}

	CCost(int numRows, int numCols)
	{

	}

	void SetCost(int numRows, int numCols)
	{
		if (costMatrix != nullptr)
			delete costMatrix;

		costMatrix = new int*[numRows + 1];
		for (int i = 1; i <= numRows; i++)
		{
			costMatrix[i] = new int[numCols + 1];
		}
		for (int i = 1; i <= numRows; i++)
		{
			for (int j = 1; j <= numCols; j++)
			{
				costMatrix[i][j] = 0;
			}
		}
	}

	~CCost()
	{
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
