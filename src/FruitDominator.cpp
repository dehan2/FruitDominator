#include "FruitDominator.hpp"
#include <fstream>
#include <set>

void FruitDominator::add_fruit_from_file(const string& fileName)
{
	Mat img = imread(fileName, IMREAD_COLOR);
	Mat resizedImg;
	resize(img, resizedImg, Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));
	m_fruits.emplace_back(resizedImg);
}



void FruitDominator::add_fruits_from_files(const list<string>& fileNames)
{
for (auto fileName : fileNames)
	{
		add_fruit_from_file(fileName);
	}
}



void FruitDominator::read_fruit_names_from_file(const string& fileName)
{
	ifstream file(fileName);
	string line;
	while (getline(file, line))
	{
		add_fruit_from_file(line);
	}
}



void FruitDominator::calculate_dominant_colors_for_fruits()
{
	int index = 1;
	for (auto fruit : m_fruits)
	{
		cout << "Fruit[" << index++ << "]: ";
		m_dominantFruitColors.push_back(calculate_dominant_color_in_bgr(fruit));
	}
}



Point3 FruitDominator::calculate_dominant_color_in_hsv(const Mat& fruit)
{
	// Step 1. Convert to HSV
	Mat hsvFruit;
	cvtColor(fruit, hsvFruit, COLOR_BGR2HSV);

	// Step 2. Filter background - remove pixels with too low saturation and too high value
	set<Vec3b*> filteredColors;
	for (int i = 0; i < hsvFruit.rows; i++)
	{
		for (int j = 0; j < hsvFruit.cols; j++)
		{
			Vec3b color = hsvFruit.at<Vec3b>(i, j);
			Vec3b originalColor = fruit.at<Vec3b>(i, j);

			if (static_cast<int>(color[1]) > MIN_SATURATION && static_cast<int>(color[2]) < MAX_VALUE)
			{
				filteredColors.insert(&hsvFruit.at<Vec3b>(i, j));
			}
		}
	}

	// Step 3. Calculate histogram
	vector<set<Vec3b*>> hueHistogram(NUM_CLASS_IN_HISTOGRAM);
	const float step = 180.0f / hueHistogram.size();
	for (auto color : filteredColors)
	{
		double hue = static_cast<int>((*color)[0]);
		int index = ceil(hue / step + 0.5) - 1;

		if (index == hueHistogram.size())
			index = 0;
		
		hueHistogram[index].insert(color);
	}


	// Step 4. Find the most frequent color
	int maxCount = 0;
	int maxIndex = 0;
	for (int i = 0; i < hueHistogram.size(); i++)
	{
		if (hueHistogram.at(i).size() > maxCount)
		{
			maxCount = hueHistogram.at(i).size();
			maxIndex = i;
		}
	}


	// Step 5. Calculate average color

	double hueSum = 0;
	double saturationSum = 0;
	double valueSum = 0;
	size_t numColors = hueHistogram.at(maxIndex).size();

	for (auto color : hueHistogram.at(maxIndex))
	{
		hueSum += (*color)[0];
		saturationSum += (*color)[1];
		valueSum += (*color)[2];
	}
	
	Vec3b averageColor(hueSum/numColors, saturationSum/numColors, valueSum/numColors);


	// Step 6. Convert to BGR
	Mat bgrMat(1, 1, CV_8UC3);
	cvtColor(Mat(1, 1, CV_8UC3, averageColor), bgrMat, COLOR_HSV2BGR);
	Vec3b bgrColor = bgrMat.at<Vec3b>(0, 0);

	cout<<"Dominant color: ["<<static_cast<int>(averageColor[0])<<", "<<static_cast<int>(averageColor[1])<<", "<<static_cast<int>(averageColor[2])<<"] -> [";
	cout <<static_cast<int>(bgrColor[0])<<", "<<static_cast<int>(bgrColor[1])<<", "<<static_cast<int>(bgrColor[2])<<"] "<<endl;

	return Point3((double)bgrColor[0]/255, (double)bgrColor[1] / 255, (double)bgrColor[2] / 255);
}



Point3 FruitDominator::calculate_dominant_color_in_bgr(const Mat& fruit)
{
	// Step 1. Convert to HSV
	Mat hsvFruit;
	cvtColor(fruit, hsvFruit, COLOR_BGR2HSV);

	// Step 2. Filter background - remove pixels with too low saturation and too high value
	set<const Vec3b*> filteredColors;
	for (int i = 0; i < hsvFruit.rows; i++)
	{
		for (int j = 0; j < hsvFruit.cols; j++)
		{
			Vec3b color = hsvFruit.at<Vec3b>(i, j);
			Vec3b originalColor = fruit.at<Vec3b>(i, j);

			if (static_cast<int>(color[1]) > MIN_SATURATION && static_cast<int>(color[2]) < MAX_VALUE)
			{
				filteredColors.insert(&fruit.at<Vec3b>(i, j));
			}
		}
	}

	// Step 3. Calculate histogram
	vector<set<const Vec3b*>> blueHistogram(NUM_CLASS_IN_HISTOGRAM);
	vector<set<const Vec3b*>> greenHistogram(NUM_CLASS_IN_HISTOGRAM);
	vector<set<const Vec3b*>> redHistogram(NUM_CLASS_IN_HISTOGRAM);

	construct_histogram_for_color(filteredColors, blueHistogram, 0);
	construct_histogram_for_color(filteredColors, greenHistogram, 1);
	construct_histogram_for_color(filteredColors, redHistogram, 2);

	// Step 4. Find the most frequent color
	int blueIndex = pick_most_frequent_class_from_histogram(blueHistogram);
	int greenIndex = pick_most_frequent_class_from_histogram(greenHistogram);
	int redIndex = pick_most_frequent_class_from_histogram(redHistogram);

	// Step 5. Calculate average color
	int average_blue = calculate_average_for_channel(blueHistogram.at(blueIndex), 0);
	int average_green = calculate_average_for_channel(greenHistogram.at(greenIndex), 1);
	int average_red = calculate_average_for_channel(redHistogram.at(redIndex), 2);
	
	cout << "Dominant color: [ " << static_cast<int>(average_blue) << ", " << static_cast<int>(average_green) << ", " << static_cast<int>(average_red) << "] " << endl;

	int maxColor = max(average_blue, max(average_green, average_red));

	return Point3((double)average_blue / maxColor, (double)average_green / maxColor, (double)average_red / maxColor);
}



void FruitDominator::calculate_centroids_for_colors_using_kmean(const list<Point3>& colors, const int& numGroups)
{
	vector<Point3> centroids = guess_initial_centroids(colors, numGroups);

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
		if (newGroups == groups)
		{
			cout << "Static result found" << endl;
			isChanged = false;
			break;
		}

		//Step 3. If groups are changed, evaluate new centroids
		centroids = calculate_centroids(colors, newGroups, numGroups);
		groups = newGroups;
	}

	if (isChanged == true)
		cout << "Max iteration reached" << endl;

	m_centroids = centroids;
}



vector<list<int>> FruitDominator::classify_colors_by_centroids(const list<Point3>& colors) const
{
	vector<list<int>> classifiedColors(m_centroids.size());

	list<vector<float>> distanceToCentroids;
	for (auto& color : colors)
		distanceToCentroids.emplace_back(calculate_distance_to_centroids(color, m_centroids));

	list<int> groups = assign_points_to_closest_points(distanceToCentroids);

	// Save the classification result
	int imageIndex = 0;
	for (auto& groupIndex : groups)
	{
		classifiedColors.at(groupIndex).push_back(imageIndex++);
	}

	return classifiedColors;
}



void FruitDominator::construct_histogram_for_color(const set<const Vec3b*>& colors, vector<set<const Vec3b*>>& histogram, const int& channelIndex) const
{
	const float step = 255 / NUM_CLASS_IN_HISTOGRAM;
	for (auto color : colors)
	{
		double value = static_cast<int>((*color)[channelIndex]);
		int classIndex = static_cast<int>(value / step);
		histogram.at(classIndex).insert(color);
	}
}



int FruitDominator::pick_most_frequent_class_from_histogram(const vector<set<const Vec3b*>>& histogram) const
{
	int maxCount = 0;
	int maxIndex = 0;
	for (int i = 0; i < histogram.size(); i++)
		if (histogram.at(i).size() > maxCount)
		{
			maxCount = histogram.at(i).size();
			maxIndex = i;
		}

	return maxIndex;
}



int FruitDominator::calculate_average_for_channel(const set<const Vec3b*>& colors, const int& channelIndex) const
{
	double sum = 0;
	size_t numColors = colors.size();

	for (auto color : colors)
	{
		sum += (*color)[channelIndex];
	}

	return static_cast<int>(sum / numColors);
}



vector<Point3> FruitDominator::guess_initial_centroids(const list<Point3>& colors, const int& numGroups) const
{
	vector<Point3> centroids;

	// Pick farthest points as initial centroids

	cout << " Initial centroids: ";
	for (int i = 0; i < numGroups; i++)
	{
		const Point3* farthestPoint = nullptr;
		float maxDistance = 0;

		for (auto& color : colors)
		{
			float distance = color.distance_to(Point3());
			for (auto& centroid : centroids)
			{
				distance += color.distance_to(centroid);
			}

			if (distance > maxDistance)
			{
				maxDistance = distance;
				farthestPoint = &color;
			}
		}

		centroids.push_back(*farthestPoint);
	}
	cout << endl;

	return centroids;
}



vector<Point3> FruitDominator::calculate_centroids(const list<Point3>& colors, const list<int>& groups, const int& numGroups) const
{
	vector<Point3> centroids(numGroups);
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



vector<float> FruitDominator::calculate_distance_to_centroids(const Point3& point, const vector<Point3>& centroids) const
{
	vector<float> distances(centroids.size());
for (int i = 0; i < centroids.size(); i++)
	{
		distances.at(i) = point.distance_to(centroids.at(i));
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