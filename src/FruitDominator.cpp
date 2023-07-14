#include "FruitDominator.hpp"
#include <fstream>

void FruitDominator::add_fruit_from_file(const string& fileName)
{
	m_fruits.emplace_back(imread(fileName));
}



void FruitDominator::add_fruits_from_files(const list<string>& fileNames)
{
for (auto fileName : fileNames)
	{
		add_fruit_from_file(fileName);
	}
}



void FruitDominator::read_fruit_names_from_file(const string& fileName, list<string>& fruitNames)
{
	ifstream file(fileName);
	string line;
	while (getline(file, line))
	{
		fruitNames.push_back(line);
	}
}



Point3d FruitDominator::calculate_dominant_color(const Mat& fruit)
{



	return Point3d();
}



vector<Point3d> FruitDominator::calculate_centroids_for_colors_using_kmean(const list<Point3d>& colors, const int& numGroups) const
{
	vector<Point3d> centroids = guess_initial_centroids(colors, numGroups);

	list<int> groups;

	int numIterations = 0;
	bool isChanged = true;
	while (isChanged == true && numIterations++ < KMEANS_MAX_ITERATION)
	{
		//Step 1. Evaluate distance to centroids and assign to groups
		list<vector<float>> distanceToCentroids;
		for (auto& color : colors)
			distanceToCentroids.emplace_back(calculate_distance_to_centroids(color, centroids));
	
		list<int> newGroups = assign_points_to_closest_points(distanceToCentroids);


		//Step 2. Check whether groups are changed; if not, stop.
		if(newGroups == groups)
		{
			isChanged = false;
			break;
		}

		//Step 3. If groups are changed, evaluate new centroids
		centroids = calculate_centroids(colors, newGroups, numGroups);
		groups = newGroups;	
	}

	return centroids;
}



vector<Point3d> FruitDominator::guess_initial_centroids(const list<Point3d>& colors, const int& numGroups) const
{
	vector<Point3d> centroids;

	// Pick farthest points as initial centroids

	for (int i = 0; i < numGroups; i++)
	{
		const Point3d* farthestPoint = nullptr;
		float maxDistance = 0;

		for (auto& color : colors)
		{
			float distance = sqrt(color.x*color.x + color.y*color.y + color.z*color.z);
			for (auto& centroid : centroids)
			{
				distance += norm(Mat(color), Mat(centroid));
			}

			if (distance > maxDistance)
			{
				maxDistance = distance;
				farthestPoint = &color;
			}
		}

		centroids.emplace_back(*farthestPoint);
	}

	return centroids;
}



vector<Point3d> FruitDominator::calculate_centroids(const list<Point3d>& colors, const list<int>& groups, const int& numGroups) const
{
	vector<Point3d> centroids(numGroups);
	vector<int> counts(numGroups);
	
	auto itForGroup = groups.begin();
	for (auto& color : colors)
	{
		int group = *itForGroup++;
		centroids[group] += color;
		counts[group]++;
	}

for (int i = 0; i < numGroups; i++)
	{
		centroids[i] /= counts[i];
	}

	return centroids;
}



vector<float> FruitDominator::calculate_distance_to_centroids(const Point3d& point, const vector<Point3d>& centroids) const
{
	vector<float> distances(centroids.size());
for (int i = 0; i < centroids.size(); i++)
	{
		distances.at(i) = norm(Mat(point), Mat(centroids.at(i)));
	}

	return distances;
}



list<int> FruitDominator::assign_points_to_closest_points(const list<vector<float>>& distanceToCentroids) const
{
	list<int> newGroups;
	for (auto& distance : distanceToCentroids)
	{
		newGroups.emplace_back(find_min_index(distance));
	}
	return newGroups;
}



int FruitDominator::find_min_index(const vector<float>& distances) const
{
	int minIndex = 0;
	float minDistance = distances[0];
	for (int i = 1; i < distances.size(); i++)
	{
		if (distances[i] < minDistance)
		{
			minDistance = distances[i];
			minIndex = i;
		}
	}
	return minIndex;
}