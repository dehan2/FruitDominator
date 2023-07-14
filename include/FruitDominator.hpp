#pragma once

#include "ConstForFruitDominator.h"

#include <list>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class FruitDominator
{
private:
	list<Mat> m_fruits;
	list<Point3d> m_dominantFruitColors;

	//For K-means clustering
	vector<Point3d> m_centroids;

public:
	FruitDominator() = default;
	~FruitDominator() = default;

	const list<Mat>& get_fruits() const { return m_fruits; }
	const list<Point3d>& get_dominant_fruit_colors() const { return m_dominantFruitColors; }

	void addFruit(Mat fruit) { m_fruits.push_back(fruit); }
	void add_fruit_from_file(const string& fileName);
	void add_fruits_from_files(const list<string>& fileNames);
	void read_fruit_names_from_file(const string& fileName, list<string>& fruitNames);
	void clearFruits() { m_fruits.clear(); }

	Point3d calculate_dominant_color(const Mat& fruit);

	vector<Point3d> calculate_centroids_for_colors_using_kmean(const list<Point3d>& colors, const int& numGroups) const;
	list<int> classify_colors(const list<Point3d>& colorsToClassify, const vector<Point3d>& centroids) const;

private:
	vector<Point3d> guess_initial_centroids(const list<Point3d>& colors, const int& numGroups) const;
	vector<Point3d> calculate_centroids(const list<Point3d>& colors, const list<int>& groups, const int& numGroups) const;
	vector<float> calculate_distance_to_centroids(const Point3d& point, const vector<Point3d>& centroids) const;
	
	list<int> assign_points_to_closest_points(const list<vector<float>>& distanceToCentroids) const;
	int find_min_index(const vector<float>& distancesToCentroids) const;
};

