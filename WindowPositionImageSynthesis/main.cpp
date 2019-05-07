#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include "Utils.h"

int main() {
	// read the parameter file
	std::ifstream in("window_params.txt");
	std::map<std::string, std::vector<float>> params;
	while (!in.eof()) {
		std::string line;
		std::getline(in, line);

		std::vector<std::string> strs;
		boost::split(strs, line, boost::is_any_of(","));

		if (strs.size() != 5) continue;
		if (strs[0] == "") break;

		float x1 = atof(strs[1].c_str());
		float y1 = atof(strs[2].c_str());
		float x2 = atof(strs[3].c_str());
		float y2 = atof(strs[4].c_str());

		// skip if the window does not exist
		if (fabs(x1 - x2) * 800 < 3 && fabs(y1 - y2) * 800 < 3) continue;

		for (int i = 1; i < strs.size(); ++i) {
			params[strs[0]].push_back(atof(strs[i].c_str()));
		}
	}
	in.close();

	int cnt = 0;
	printf("Generating images: ");
	boost::filesystem::path dir_data("../tiles/");
	boost::filesystem::path dir_result("//cuda.cs.purdue.edu/scratch2/window_regression/data/images/01/");
	boost::filesystem::path sub_dir;

	std::ofstream out_param(dir_result.string() + "parameters.txt");

	for (auto it = params.begin(); it != params.end(); ++it) {
		// read an image
		cv::Mat orig_img = cv::imread(dir_data.string() + it->first);

		// convert the image to grayscale
		cv::cvtColor(orig_img, orig_img, cv::COLOR_BGR2GRAY);

		int num_samples = 4;
		if (it->first.substr(0, 3) == "cmp") {
			if (it->second[0] > 0.5 || it->second[1] > 0.5 || it->second[2] < 0.5 || it->second[3] < 0.5) {
				num_samples = 10;
			}
			else if (it->second[0] > 0.4 || it->second[1] > 0.4 || it->second[2] < 0.6 || it->second[3] < 0.6) {
				num_samples = 4;
			}
			else if (it->second[0] < 0.05 || it->second[1] < 0.05 || it->second[2] > 0.95 || it->second[3] > 0.95) {
				num_samples = 4;
			}
			else {
				num_samples = 4;
			}
		}
		else if (it->first.substr(0, 4) == "test") {
			num_samples = 4;
		}
		else {
			if (it->second[0] > 0.5 || it->second[1] > 0.5 || it->second[2] < 0.5 || it->second[3] < 0.5) {
				num_samples = 120;
			}
			else if (it->second[0] > 0.4 || it->second[1] > 0.4 || it->second[2] < 0.6 || it->second[3] < 0.6) {
				num_samples = 100;
			}
			else if (it->second[0] < 0.05 || it->second[1] < 0.05 || it->second[2] > 0.95 || it->second[3] > 0.95) {
				num_samples = 60;
			}
			else {
				num_samples = 20;
			}
		}
		
		for (int iter = 0; iter < num_samples; ++iter) {
			printf("\rGenerating images: %d", cnt + 1);

			cv::Mat img = orig_img.clone();

			// window boundary
			float x1 = it->second[0] * img.cols;
			float y1 = it->second[1] * img.rows;
			float x2 = it->second[2] * img.cols;
			float y2 = it->second[3] * img.rows;

#if 1
			// rotate the image
			cv::Point2f offset(img.cols / 4 + (float)(rand() % img.cols) * 0.5, img.rows / 4 + (float)(rand() % img.rows) * 0.5);
			float angle = utils::genRand(-1, 1);
			cv::Mat rot_mat = cv::getRotationMatrix2D(offset, angle, 1.0);
			cv::warpAffine(img, img, rot_mat, img.size(), cv::INTER_CUBIC, cv::BORDER_REPLICATE);

			// since the rotation is little, we do not update the window boundary
#endif

			// crop the image
			int crop_x1 = utils::genRand(0, 4);
			int crop_y1 = utils::genRand(0, 4);
			int crop_x2 = utils::genRand(img.cols - 5, img.cols - 1);
			int crop_y2 = utils::genRand(img.rows - 5, img.rows - 1);
			//if (crop_x2 >= img.cols - 1) crop_x2 = img.cols - 1;
			//if (crop_y2 >= img.rows - 1) crop_y2 = img.rows - 1;
			img = cv::Mat(img, cv::Rect(crop_x1, crop_y1, crop_x2 - crop_x1 + 1, crop_y2 - crop_y1 + 1)).clone();

			// update the window boundary
			x1 -= crop_x1;
			y1 -= crop_y1;
			x2 -= crop_x1;
			y2 -= crop_y1;

			// change the brightness
			int change = rand() % 20 - 10;
			for (int r = 0; r < img.rows; ++r) {
				for (int c = 0; c < img.cols; ++c) {
					int col = img.at<uchar>(r, c);
					col += change;
					if (col < 0) col = 0;
					if (col > 255) col = 255;
					img.at<uchar>(r, c) = col;
				}
			}


			// blur
			int blur = rand() % 2;
			if (blur == 0) {
				// do nothing
			}
			else if (blur == 1) {
				cv::blur(img, img, cv::Size(3, 3));
			}

			// mirror horizontally
			if (rand() % 2 == 0) {
				cv::flip(img, img, 1);

				// update window boundary
				float x1_bak = x1;
				x1 = img.cols - x2;
				x2 = img.cols - x1_bak;
			}

			// normalize the parameter values to [0, 1]
			x1 = x1 / img.cols;
			y1 = y1 / img.rows;
			x2 = x2 / img.cols;
			y2 = y2 / img.rows;

			// resize
			cv::resize(img, img, cv::Size(227, 227));

			// convert to color image
			cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);


			// create sub folder
			if (cnt % 10000 == 0) {
				char sub_dir_name[256];
				sprintf(sub_dir_name, "%06d/", (int)(cnt / 10000));
				sub_dir = boost::filesystem::path(dir_result.string() + sub_dir_name);
				boost::filesystem::create_directory(sub_dir);
			}


			// save the transformed image
			char filename[256];
			sprintf(filename, "%06d.png", cnt++);
			cv::imwrite((sub_dir.string() + filename).c_str(), img);

			// save the parameter values
			out_param << x1 << "," << y1 << "," << x2 << "," << y2 << "\n";
		}
	}
	printf("\n");

	out_param.close();

	return 0;
}