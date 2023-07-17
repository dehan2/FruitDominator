#include "FruitDominator.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>


using namespace std;

int main(int argc, char** argv)
{
	FruitDominator fruitDominator;
	string fruitNamesFile = "Fruits\\FruitList.txt";
	fruitDominator.read_fruit_names_from_file(fruitNamesFile);

	fruitDominator.calculate_dominant_colors_for_fruits();

	int numGroups = 3;
	fruitDominator.calculate_centroids_for_colors_using_kmean(fruitDominator.get_dominant_fruit_colors(), numGroups);

	FruitDominator candidateDominator;
	string candidateNamesFile = "Candidates\\CandidateList.txt";
	candidateDominator.read_fruit_names_from_file(candidateNamesFile);
	candidateDominator.calculate_dominant_colors_for_fruits();

	candidateDominator.set_centroids(fruitDominator.get_centroids());
	auto classificationResult = candidateDominator.classify_colors_by_centroids(candidateDominator.get_dominant_fruit_colors());
	
	cout << "Classification finish - [ ";
	for(int i=0; i< numGroups; i++)
		cout<< classificationResult[i].size() << " ";
	cout<<"]" << endl;

	for (int i = 0; i < numGroups; i++)
	{
		cout<<"Group ["<< i << "]: ";
		for(auto& index : classificationResult.at(i))
			cout << index+1 << " ";
		cout << endl;
	}

	return 0;
}