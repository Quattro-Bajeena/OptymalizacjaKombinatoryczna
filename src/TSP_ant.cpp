#include <iostream>
#include <vector>
#include <tuple>
#include <fstream>
#include <limits>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>
#include <ctime>
#include <cstdlib>
#include <string>
#include <chrono>

struct Node
{
	int x, y;
};

struct Ant{
    int currentNode;
    float cumulativeDistance;
    std::vector<int> visitedNodes;
};

float Distance(const Node& a, const Node b){
    return std::sqrt(std::pow(a.x-b.x, 2) + std::pow(a.y-b.y, 2) );
}

float Distance(int a, int b, const std::vector<Node>& nodes){
    if(a == b)
        return 0;
    else
        return Distance(nodes[a], nodes[b]);
}

float AllPossiblePathsPower(int start, 
                    float visibilityFactor, float pheromoneFactor, 
                    const std::vector<std::vector<float>>& edgeCosts, const std::vector<std::vector<float>>& edgePheromones, 
                    const std::vector<int>& visitedNodes){

    float allPosiblePathsPower = 0;
    for(int i = 0; i < edgePheromones.size(); i++){
        if(i != start && (std::find(visitedNodes.begin(), visitedNodes.end(), i) == visitedNodes.end()) ){
            allPosiblePathsPower += std::pow(edgePheromones[start][i], pheromoneFactor) * std::pow(edgeCosts[start][i], visibilityFactor);
        }
    }
    return allPosiblePathsPower;

}

float PathPropability(
                    int start, int destination, 
                    float visibilityFactor, float pheromoneFactor, 
                    const std::vector<std::vector<float>>& edgeCosts, const std::vector<std::vector<float>>& edgePheromones, 
                    float allPossiblePathsPower){

    float pathPower = std::pow(edgePheromones[start][destination], pheromoneFactor) * std::pow(edgeCosts[start][destination], visibilityFactor);
    return pathPower / allPossiblePathsPower;
}

int PickNode(std::vector<float> propabilities){
    
    float random = (float)rand() / float(RAND_MAX); // between 0 and 1
    float cumulativePropability = 0;
    for(int i = 0; i < propabilities.size(); i++){
        cumulativePropability += propabilities[i];
        if(random <= cumulativePropability){
            return i;
        }
    }

    // unnecesary I think
    return propabilities.size()-1;
}

float NewPheromones(int i, int j, std::vector<Ant> ants, float pheromoneAddingRate){
    float newPheromones = 0;
    for(Ant& ant : ants){
        for(int visited = 0; visited < ant.visitedNodes.size()-1; visited++){
            int from = ant.visitedNodes[visited];
            int to = ant.visitedNodes[visited + 1];
            if( (from == i && to == j) || (from == j && to == i) ){
                newPheromones += pheromoneAddingRate / ant.cumulativeDistance;
            }

        }
    }
    return newPheromones;
}



std::pair<float, std::vector<int>> TSPAnt(std::vector<Node> nodes) {
	float shortestDistance = std::numeric_limits<float>::max();
	std::vector<int> shortestPath;

    int nodeNum = nodes.size();
    const float pheromoneAddingRate = 100; // Q - quantity of trail laid
    const float evaporationRate = 0.5; // p - trail persitance
    const float pheromoneFactor = 1; // alpha - relative importance of trail
    const float visibilityFactor = 5; // veta - relative importance of visibility
    const int antsCount = 20;
    const int turnCount = 200;

    std::vector<Ant> ants(antsCount);

    for(int i = 0; i < antsCount; i++){
		int node = rand() % nodeNum;
        ants[i].currentNode = node;
		ants[i].visitedNodes = std::vector<int>(nodeNum+1, -1);
    }

    std::vector<std::vector<float>> edgeDistances(nodeNum, std::vector<float>(nodeNum, 0));
	std::vector<std::vector<float>> edgeCosts(nodeNum, std::vector<float>(nodeNum, 0));
    std::vector<std::vector<float>> edgePheromones(nodeNum, std::vector<float>(nodeNum, 1));

    for(int i = 0; i < nodeNum; i++){
        for(int j = 0; j < nodeNum; j++){
            if(i == j)
                break;
			float distance = Distance(i, j, nodes);
            float cost = 1. / distance;

			edgeDistances[i][j] = distance;
			edgeDistances[j][i] = distance;

            edgeCosts[i][j] = cost;
            edgeCosts[j][i] = cost;
        }
    }

    for(int turn = 0; turn < turnCount; turn++){
        // Ant movements
        for(Ant& ant : ants){
			ant.cumulativeDistance = 0;
			std::fill(ant.visitedNodes.begin(), ant.visitedNodes.end(), -1);

			int tourStartNode = ant.currentNode;
			ant.visitedNodes[0] = ant.currentNode;

            for(int i = 1; i < nodeNum; i++){

				int from = ant.currentNode;
				const float allPossiblePathsPower = AllPossiblePathsPower(from, visibilityFactor, pheromoneFactor, edgeCosts, edgePheromones, ant.visitedNodes);

				std::vector<float> propabilities(nodeNum);

				for(int possibleDestination = 0; possibleDestination < nodeNum; possibleDestination++){

					if(from == possibleDestination){
						propabilities[possibleDestination] = 0;
					}
					else if(std::find(ant.visitedNodes.begin(), ant.visitedNodes.end(), possibleDestination) != ant.visitedNodes.end()){
						propabilities[possibleDestination] = 0;
					}
					else{
						propabilities[possibleDestination] = PathPropability(
														from, possibleDestination, 
														visibilityFactor, pheromoneFactor,
														edgeCosts, edgePheromones, allPossiblePathsPower);
					}
				}
				int nextNode = PickNode(propabilities);
				float distance = edgeDistances[from][nextNode];
				ant.currentNode = nextNode;
				ant.visitedNodes[i] = nextNode;
				ant.cumulativeDistance += distance;
            }
			// Coming back
			float distance = edgeDistances[ant.currentNode][tourStartNode];
			ant.currentNode = tourStartNode;
			ant.visitedNodes[ant.visitedNodes.size()-1] = tourStartNode;
			ant.cumulativeDistance += distance;

			if(ant.cumulativeDistance < shortestDistance){
				shortestDistance = ant.cumulativeDistance;
				shortestPath = ant.visitedNodes;
			}
        }
        
        // Updating pheromone levels
        for(int i = 0; i < nodeNum; i++){
            for(int j = 0; j < nodeNum; j++){
                if(i == j)
                    break;

                // for every edge (i,j)
                float newPheromones = NewPheromones(i, j, ants, pheromoneAddingRate);
                float currentPheronomeLevel = (evaporationRate * edgePheromones[i][j]) + newPheromones;
                edgePheromones[i][j] = currentPheronomeLevel;
                edgePheromones[j][i] = currentPheronomeLevel;
            }
        }
        
    }

	return std::make_pair(shortestDistance, shortestPath);
}

std::vector<Node> LoadNodes(std::string testPath) {
	std::ifstream testFile;
	testFile.open(testPath);
	if (testFile.is_open() == false) {
		std::cout << "Coulnd't open the file\n";
		exit(1);
	}
	int vertexCount, vertexIndex;
	int vx, vy;
	testFile >> vertexCount;

	std::vector<Node> nodes;
	while (testFile >> vertexIndex >> vx >> vy) {
		Node n = { vx, vy };
		nodes.push_back(n);
	}
	return nodes;
}

int main() {
    srand(time(NULL));

	std::string testPath; // = "F:\\Programowanie\\Studia\\OptymalizacjaKombinatoryczna\\tests\\random\\test_10.txt";
    std::getline(std::cin, testPath);
	// = "C:\\Users\\student\\Documents\\Mateusz Oleszek\\OptymalizacjaKombinatoryczna-master\\tests\\test_10.txt";
	// = "F:\\Programowanie\\Studia\\OptymalizacjaKombinatoryczna\\tests\\random\\test_10.txt";
    

	std::vector<Node> nodes = LoadNodes(testPath);

	auto begin = std::chrono::steady_clock::now();
	std::pair<float, std::vector<int>> results = TSPAnt(nodes);
    auto end = std::chrono::steady_clock::now();

	float distanceTraveled = results.first;
	std::vector<int> visited = results.second;

    std::cout<<nodes.size()<<"\n";
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() <<"\n";
	std::cout << distanceTraveled << "\n";
	for (int node : visited) {
		std::cout << nodes[node].x << ";" << nodes[node].y << "\n";
	}

}