#include<iostream>
#include<vector>
#include<tuple>
#include<fstream>
#include <limits>
#include <cmath>

struct Node
{
	float x, y;
};


float TSPgreedy(std::vector<Node> nodes, std::vector<Node>& visited) {
	std::vector<Node> notVisited;
	notVisited = nodes;

	Node currentNode = notVisited[0];
	visited.push_back(currentNode);
	notVisited.erase(notVisited.begin());

	float distanceTraveled = 0;

	while (notVisited.size() > 0) {
		float shortestDistance = std::numeric_limits<float>::infinity();
		int nearestNodeIndex;

		for (int i = 0; i < notVisited.size(); i++) {
			Node node = notVisited[i];
			float distance = std::sqrt(std::pow(currentNode.x - node.x, 2) + std::pow(currentNode.y - node.y, 2));
			if (distance < shortestDistance) {
				shortestDistance = distance;
				nearestNodeIndex = i;
			}
		}

		currentNode = notVisited[nearestNodeIndex];
		notVisited.erase(notVisited.begin() + nearestNodeIndex);
		visited.push_back(currentNode);
		distanceTraveled += shortestDistance;
	}

	Node firstNode = visited[0];
	float distance = std::sqrt(std::pow(currentNode.x - firstNode.x, 2) + std::pow(currentNode.y - firstNode.y, 2));
	distanceTraveled += distance;
	visited.push_back(firstNode);

	return distanceTraveled;
}

std::vector<Node> LoadNodes(std::string testPath) {
	std::ifstream testFile;
	testFile.open(testPath);
	if (testFile.is_open() == false) {
		std::cout << "Coulnd't open the file\n";
		exit(1);
	}
	int vertexCount;
	float vx, vy;
	testFile >> vertexCount;

	std::vector<Node> nodes;
	while (testFile >> vx >> vy) {
		Node n = { vx, vy };
		nodes.push_back(n);
	}
	return nodes;
}

int main() {
	std::string testPath= "F:\\Programowanie\\Studia\\OptymalizacjaKombinatoryczna\\tests\\instances\\kroa100.txt";
	// = "C:\\Users\\student\\Documents\\Mateusz Oleszek\\OptymalizacjaKombinatoryczna-master\\tests\\test_10.txt";
	// "F:\\Programowanie\\Studia\\OptymalizacjaKombinatoryczna\\tests\\test_10.txt";
	
	//std::cin >> testPath;

	std::vector<Node> nodes = LoadNodes(testPath);

	std::vector<Node> visited;
	float distanceTraveled = TSPgreedy(nodes, visited);

	std::cout << distanceTraveled << "\n";
	for (int i = 0; i < visited.size(); i++) {
		std::cout << visited[i].x << ";" << visited[i].y << "\n";
	}

}