#include <time.h>
#include "TSPv2.h"

using namespace std;

class CParser
{
public:

	CParser(string filename)
	{
		try
		{
			ifstream istr(filename);
			smatch match;

			string tmp;
			getline(istr, tmp);
			regex rx("^NAME:\\s+(.*)$");
			regex_search(tmp, match, rx);
			name = match[1];

			getline(istr, tmp);
			rx = "^TYPE:\\s+(.*)$";
			regex_search(tmp, match, rx);
			type = match[1];

			getline(istr, tmp);
			rx = "^COMMENT:\\s+(.*)$";
			regex_search(tmp, match, rx);
			comment = match[1];

			getline(istr, tmp);
			rx = "^DIMENSION:\\s+(.*)$";
			regex_search(tmp, match, rx);
			dimension = atoi(match[1].str().c_str());

			getline(istr, tmp);
			rx = "^EDGE_WEIGHT_TYPE:\\s+(.*)$";
			regex_search(tmp, match, rx);
			edgeWeightType = match[1];

			getline(istr, tmp);
			rx = "^EDGE_WEIGHT_FORMAT:\\s+(.*)$";
			regex_search(tmp, match, rx);
			edgeWeightFormat = match[1];

			int i = 1;
			int j = 1;
			costMatrix = new int*[dimension + 1];
			for (int k = 1; k <= dimension; k++)
			{
				costMatrix[k] = new int[dimension + 1];
			}
			for (int l = 1; l <= dimension; l++)
			{
				for (int y = 1; y <= dimension; y++)
				{
					costMatrix[l][y] = 0;
				}
			}
			getline(istr, tmp);
			getline(istr, tmp);

			while (tmp != "" && tmp != "EOF")
			{
				list<int> list;
				string line(tmp);

				while (line != "")
				{
					int n = line.find_first_of(string(" "));
					if (n != 0 && n < line.length())
					{
						string c = line.substr(0, n);
						list.push_back(atoi(c.c_str()));
						line = string(line.substr(n, line.length()));
					}
					else if (n == -1 && line != "")
					{
						list.push_back(atoi(line.c_str()));
						line = "";
					}
					else
					{
						line = line.substr(1, line.length());
					}
				}

				for (int cost : list)
				{
					if (j > dimension) {
						i++;
						j = 1;
					}
					int edgeWeight = cost;
					if (i == 1 && j == 1) {
						separator = edgeWeight;
					}
//					if (edgeWeight == separator) edgeWeight = 0;
					costMatrix[i][j] = (int)edgeWeight;
					j++;
				}

				tmp.clear();
				getline(istr, tmp);
			}
			istr.close();
		}
		catch (exception e)
		{

		}
	}

	~CParser() {}

	string GetName() 
	{
		return name;
	}

	string GetType() 
	{
		return type;
	}

	string GetComment() 
	{
		return comment;
	}

	int GetDimension() 
	{
		return dimension;
	}

	string GetEdgeWeightType() 
	{
		return edgeWeightType;
	}

	string GetEdgeWeightFormat() 
	{
		return edgeWeightFormat;
	}

	int GetEdgeWeight(int row, int col) 
	{
		return costMatrix[row][col];
	}

	int** GetCostMatrix() 
	{
		return costMatrix;
	}

private:
	string name;
	string type;
	string comment;
	int dimension;
	string edgeWeightType;
	string edgeWeightFormat;
	int** costMatrix;
	int separator;
};

class Euristic
{
	int** costMatrix;
	int dim;
	vector<int> usedIndexs;

public:
	struct Min
	{
		int min;
		int index;
	};

	Euristic(int** costMatrix, int dim)
	{
		this->costMatrix = costMatrix;
		this->dim = dim;
	}

	int getEuristic()
	{
		srand(time(0));
		int ret = 0;
		int currentIndex = rand() % dim + 1;
		usedIndexs.push_back(currentIndex);
		for (int i = 1; i < dim; i++)
		{
			Min min = getMinInLine(currentIndex);
			currentIndex = min.index;
			ret += min.min;
		}
		ret += costMatrix[usedIndexs[usedIndexs.size() - 1]][usedIndexs[0]];

		return ret;
	}

private:

	Min getMinInLine(int i)
	{
		int min = 99999999;
		int lineIndex = 1;
		for (int j = 1; j <= dim; j++)
		{
			if (min > costMatrix[i][j] && i != j)
			{
				bool flag = false;
				for (int k = 0; k < usedIndexs.size(); k++)
				{
					if (j == usedIndexs[k]) flag = true;
				}
				if (!flag)
				{
					min = costMatrix[i][j];
					lineIndex = j;
				}
			}
		}

		Min ret;
		if (min == 99999999) min = 0;
		ret.min = min;
		ret.index = lineIndex;
		usedIndexs.push_back(lineIndex);

		return ret;
	}
};

void main(int argc, char *argv[])
{
	string path(argv[1]);
	int maxTime = atoi(argv[2]);
	CParser parser(path);
	Euristic e(parser.GetCostMatrix(), parser.GetDimension());
	int heuristics = e.getEuristic();

	cout << "Current solution: " << heuristics << " time: " << "0.001 s" << endl;
		TSP tsp(parser.GetCostMatrix(), parser.GetDimension(), heuristics, maxTime);
	tsp.generateSolution();

	cout << tsp.GetBestTour() << endl;
}
