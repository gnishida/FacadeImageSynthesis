#include "Utils.h"
#include <regex>
#include <sstream>
#include <iostream>

#ifndef SQR
#define SQR(x)	((x) * (x))
#endif

namespace utils {

	const double M_PI = 3.1415926535897932384626433832795;
	const float FLOAT_TOL = 1e-6f;

	bool isNumber(const std::string& str) {
		std::regex e("^-?\\d*\\.?\\d+");
		if (std::regex_match(str, e)) return true;
		else return false;
	}

	void computeMean(const std::vector<std::vector<float>>& values, std::vector<float>& mean) {
		mean.clear();

		if (values.size() == 0) return;

		// 総和を計算する
		mean.resize(values[0].size(), 0);
		for (int i = 0; i < values.size(); ++i) {
			for (int j = 0; j < values[i].size(); ++j) {
				mean[j] += values[i][j];
			}
		}

		// 平均を計算する
		for (int i = 0; i < mean.size(); ++i) {
			mean[i] /= (float)values.size();
		}
	}

	void computeVariance(const std::vector<std::vector<float>>& values, const std::vector<float>& mean, std::vector<float>& var) {
		var.clear();

		if (values.size() == 0) return;

		// 二乗差の総和を計算する
		var.resize(mean.size(), 0);
		for (int i = 0; i < values.size(); ++i) {
			for (int j = 0; j < values[i].size(); ++j) {
				var[j] += SQR(values[i][j] - mean[j]);
			}
		}

		// 二乗差の平均を計算する
		for (int i = 0; i < var.size(); ++i) {
			var[i] /= (float)values.size();
		}
	}

	float median(std::vector<float> list) {
		size_t size = list.size();
		std::sort(list.begin(), list.end());
		if (size % 2 == 0) {
			return (list[size / 2 - 1] + list[size / 2]) / 2;
		}
		else {
			return list[list.size() / 2];
		}
	}

	float stddev(std::vector<float> list) {
		float sum = 0.0f;
		for (int i = 0; i < list.size(); ++i) {
			sum += list[i];
		}
		float avg = sum / list.size();

		float total = 0.0f;
		for (int i = 0; i < list.size(); ++i) {
			total += (list[i] - avg) * (list[i] - avg);
		}

		return sqrt(total / (list.size() - 1));
	}

	float mean(std::vector<float> list) {
		float sum = 0.0f;
		for (int i = 0; i < list.size(); ++i) {
			sum += list[i];
		}
		return sum / list.size();
	}

	/**
	 * Generate a random number in [0, 1).
	 * Note: 1 is exluded!
	 */
	double genRand() {
		//return (float)(rand() % 1000) / 1000.0f;
		return ((double)rand() + 1.0) / ((double)RAND_MAX + 2.0);
	}

	/**
	 * Generate a random number in [0, b).
	 * Note: b is excluded!
	 */
	double genRand(double v) {
		return genRand() * v;
	}

	/**
	 * Generate a random number in [a, b).
	 * Note: b is excluded!
	 */
	double genRand(double a, double b) {
		return genRand(b - a) + a;
	}

	/**
	* Generate a random integer number in [0, a].
	* Note: a is included!
	*/
	int genIntRand(int a) {
		return genRand(a + 1);
	}

	/**
	* Generate a random integer number in [a, b].
	* Note: b is included!
	*/
	int genIntRand(int a, int b) {
		return genRand(b + 1 - a) + a;
	}

	double genNormal(double mu, double sigma) {
		double z = sqrt(-2.0 * log(genRand())) * sin(2.0 * M_PI * genRand());
		return mu + sigma * z;
	}

	double gause(double u, double sigma) {
		return 1.0 / 2.0 / M_PI / sigma / sigma * exp(-u * u / 2.0 / sigma / sigma);
	}

	void output_vector(const std::vector<float>& values) {
		for (int i = 0; i < values.size(); ++i) {
			if (i > 0) std::cout << ", ";
			std::cout << values[i];
		}
		std::cout << std::endl;
	}
}