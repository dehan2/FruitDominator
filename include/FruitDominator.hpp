#pragma once

#include "ConstForFruitDominator.h"
#include "Point3.h"

#include <list>
#include <vector>
#include <array>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class FruitDominator
{
private:
	list<Mat> m_fruits;
	list<Point3> m_dominantFruitColors;

	vector<Point3> m_centroids;

public:
	FruitDominator() = default;
	~FruitDominator() = default;

	const list<Mat>& get_fruits() const { return m_fruits; }
	const list<Point3>& get_dominant_fruit_colors() const { return m_dominantFruitColors; }
	const vector<Point3>& get_centroids() const { return m_centroids; }
	void set_centroids(const vector<Point3>& centroids) { m_centroids = centroids; }

	void addFruit(Mat fruit) { m_fruits.push_back(fruit); }
	void add_fruit_from_file(const string& fileName);
	void add_fruits_from_files(const list<string>& fileNames);
	void read_fruit_names_from_file(const string& fileName);
	void clearFruits() { m_fruits.clear(); }

	void calculate_dominant_colors_for_fruits();
	Point3 calculate_dominant_color_in_hsv(const Mat& fruit);
	Point3 calculate_dominant_color_in_bgr(const Mat& fruit);

	void calculate_centroids_for_colors_using_kmean(const list<Point3>& colors, const int& numGroups);
	vector<list<int>> classify_colors_by_centroids(const list<Point3>& colors) const;

private:
	void construct_histogram_for_color(const set<const Vec3b*>& colors, vector<set<const Vec3b*>>& histogram, const int& channelIndex) const;
	int pick_most_frequent_class_from_histogram(const vector<set<const Vec3b*>>& histogram) const;
	int calculate_average_for_channel(const set<const Vec3b*>& colors, const int& channelIndex) const;

	vector<Point3> guess_initial_centroids(const list<Point3>& colors, const int& numGroups) const;
	vector<Point3> calculate_centroids(const list<Point3>& colors, const list<int>& groups, const int& numGroups) const;
	vector<float> calculate_distance_to_centroids(const Point3& point, const vector<Point3>& centroids) const;
	
	list<int> assign_points_to_closest_points(const list<vector<float>>& distanceToCentroids) const;
	int find_min_index(const vector<float>& distancesToCentroids) const;
};

