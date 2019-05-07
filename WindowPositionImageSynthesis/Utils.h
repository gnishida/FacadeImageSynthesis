#pragma once

#include <string>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

namespace utils {
	
	bool isNumber(const std::string& str);
	void computeMean(const std::vector<std::vector<float>>& values, std::vector<float>& mean);
	void computeVariance(const std::vector<std::vector<float>>& values, const std::vector<float>& mean, std::vector<float>& var);
	float median(std::vector<float> list);
	float stddev(std::vector<float> list);
	float mean(std::vector<float> list);

	double genRand();
	double genRand(double v);
	double genRand(double a, double b);
	int genIntRand(int v);
	int genIntRand(int a, int b);
	double genNormal(double mu, double sigma);
	double gause(double u, double sigma);


	void output_vector(const std::vector<float>& values);
}
