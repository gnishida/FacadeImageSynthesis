#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>

int main() {
	// read the parameter file
	std::ifstream in("facade_params.txt");
	std::map<std::string, std::vector<float>> params;
	while (!in.eof()) {
		std::string line;
		std::getline(in, line);

		std::vector<std::string> strs;
		boost::split(strs, line, boost::is_any_of(","));

		if (strs.size() == 0) break;
		if (strs[0] == "") break;

		for (int i = 1; i < strs.size(); ++i) {
			params[strs[0]].push_back(atof(strs[i].c_str()));
		}
	}
	in.close();

	int cnt = 0;
	printf("Generating images: ");
	boost::filesystem::path dir_data("../cmp/");
	boost::filesystem::path dir_result("//cuda.cs.purdue.edu/scratch2/facade_direct_regression/data/images/01/");
	boost::filesystem::path sub_dir;

	std::ofstream out_param(dir_result.string() + "parameters.txt");

	for (auto it = params.begin(); it != params.end(); ++it) {
		// read an image
		cv::Mat img = cv::imread(dir_data.string() + it->first);

		// convert the image to grayscale
		cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

		for (int iter = 0; iter < 400; ++iter) {
			printf("\rGenerating images: %d", cnt + 1);

			// rotate the image
			cv::Mat rot_img;
			cv::Point2f offset(img.cols / 2 + rand() % 30 - 15, img.rows / 2 + rand() % 30 - 15);
			float angle = rand() % 5 - 2;
			cv::Mat rot_mat = cv::getRotationMatrix2D(offset, angle, 1.0);
			cv::warpAffine(img, rot_img, rot_mat, img.size(), cv::INTER_CUBIC, cv::BORDER_REPLICATE);

			// crop the image
			int x1 = rand() % (int)5;
			int y1 = rand() % (int)5;
			int x2 = rot_img.cols - 1 - rand() % 5;
			int y2 = rot_img.rows - 1 - rand() % 5;
			cv::Mat crop_img = cv::Mat(rot_img, cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1)).clone();

			// change the brightness
			int change = rand() % 100 - 50;
			for (int r = 0; r < crop_img.rows; ++r) {
				for (int c = 0; c < crop_img.cols; ++c) {
					int col = crop_img.at<uchar>(r, c);
					col += change;
					if (col < 0) col = 0;
					if (col > 255) col = 255;
					crop_img.at<uchar>(r, c) = col;
				}
			}

			// blur
			int blur = rand() % 2;
			if (blur == 0) {
				// do nothing
			}
			else if (blur == 1) {
				cv::blur(crop_img, crop_img, cv::Size(3, 3));
			}

			// mirror
			if (rand() % 2 == 0) {
				cv::flip(crop_img, crop_img, 1);
			}

			// resize
			cv::resize(crop_img, crop_img, cv::Size(227, 227));

			// convert to color image
			cv::cvtColor(crop_img, crop_img, cv::COLOR_GRAY2BGR);


			// create sub folder
			if (cnt % 10000 == 0) {
				char sub_dir_name[256];
				sprintf(sub_dir_name, "%06d/", (int)(cnt / 10000));
				sub_dir = boost::filesystem::path(dir_result.string() + sub_dir_name);
				boost::filesystem::create_directory(sub_dir);
			}



			char filename[256];
			sprintf(filename, "%06d.png", cnt++);
			cv::imwrite((sub_dir.string() + filename).c_str(), crop_img);


			for (int k = 0; k < it->second.size(); ++k) {
				if (k > 0) out_param << ",";
				out_param << it->second[k];
			}
			out_param << "\n";
		}
	}
	printf("\n");

	out_param.close();

	return 0;
}