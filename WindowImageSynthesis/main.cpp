#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <iomanip>

int main() {
	// read the parameter file
	std::ifstream in2("window_params.txt");
	std::map<std::string, int> existences;

	while (!in2.eof()) {
		std::string line;
		std::getline(in2, line);

		std::vector<std::string> strs;
		boost::split(strs, line, boost::is_any_of(","));

		if (strs.size() != 5) continue;
		if (strs[0] == "") break;

		float x1 = atof(strs[1].c_str());
		float y1 = atof(strs[2].c_str());
		float x2 = atof(strs[3].c_str());
		float y2 = atof(strs[4].c_str());

		// if the window does not exist
		if (fabs(x1 - x2) * 800 < 3 && fabs(y1 - y2) * 800 < 3) {
			existences[strs[0]] = 0;
		}
		else {
			existences[strs[0]] = 1;
		}
	}
	in2.close();


	// read the parameter file
	std::ifstream in("window_shape.txt");
	std::map<std::string, int> params;
	std::map<int, int> type_distribution;
	while (!in.eof()) {
		std::string filename;
		int type;
		in >> filename >> type;
		type = type - 1;

		if (existences[filename] == 0) continue;
		if (type < 0) continue;

		params[filename] = type;

		if (type_distribution.find(type) == type_distribution.end()) {
			type_distribution[type] = 1;
		}
		else {
			type_distribution[type]++;
		}

		if (filename == "") break;
	}
	in.close();

	for (auto it = type_distribution.begin(); it != type_distribution.end(); ++it) {
		std::cout << "type: " << it->first << ", cnt=" << it->second << std::endl;
	}

	// num of variation for each type
	std::map<int, int> num_variations;
	for (auto it = type_distribution.begin(); it != type_distribution.end(); ++it) {
		int num = it->second;
		num_variations[it->first] = 5000 / num;
	}

	printf("Generating images: ");
	boost::filesystem::path dir_data("../tiles/");
	boost::filesystem::path dir_result("//cuda.cs.purdue.edu/scratch2/window/data/images/");
	std::vector<boost::filesystem::path> sub_dir(num_variations.size());

	int cnt = 0;
	std::vector<int> num_generated(num_variations.size(), 0);
	for (auto it = boost::filesystem::directory_iterator(dir_data); it != boost::filesystem::directory_iterator(); ++it) {
		if (boost::filesystem::is_directory(it->path())) continue;

		// skip if the parameter values are not provided
		if (params.find(it->path().filename().string()) == params.end()) continue;

		// read an image
		cv::Mat img = cv::imread(dir_data.string() + it->path().filename().string());

		// get parameter values
		int type = params[it->path().filename().string()];

		// class directory
		std::stringstream ss;
		ss << std::setw(2) << std::setfill('0') << type + 1;

		// create class directory
		boost::filesystem::path class_dir(dir_result.string() + ss.str() + "/");
		if (!boost::filesystem::exists(class_dir)) {
			boost::filesystem::create_directory(class_dir);
		}

		for (int i = 0; i < num_variations[type]; ++i) {
			printf("\rGenerating images: %d", cnt + 1);
			cnt++;

			// resize
			cv::resize(img, img, cv::Size(227, 227));

			// rotate the image
			cv::Mat rot_img;
			cv::Point2f offset(img.cols / 2 + rand() % 20 - 10, img.rows / 2 + rand() % 20 - 10);
			float angle = rand() % 7 - 3;
			cv::Mat rot_mat = cv::getRotationMatrix2D(offset, angle, 1.0);
			cv::warpAffine(img, rot_img, rot_mat, img.size(), cv::INTER_CUBIC, cv::BORDER_REPLICATE);

			// change the color
			cv::Mat hsv;
			cv::cvtColor(rot_img, hsv, cv::COLOR_BGR2HSV);
			int change_h = rand() % 180 - 90;
			int change_s = rand() % 80 - 40;
			int change_v = rand() % 80 - 40;
			for (int r = 0; r < hsv.rows; ++r) {
				for (int c = 0; c < hsv.cols; ++c) {
					cv::Vec3b col = hsv.at<cv::Vec3b>(r, c);
					col[0] = (col[0] + change_h) % 180;
					col[1] = std::max(0, std::min(255, col[1] + change_s));
					col[2] = std::max(0, std::min(255, col[2] + change_v));
					hsv.at<cv::Vec3b>(r, c) = col;
				}
			}
			cv::cvtColor(hsv, rot_img, cv::COLOR_HSV2BGR);

			// blur
			cv::Mat blurred_img;
			int blur = rand() % 2;
			if (blur == 0) {
				blurred_img = rot_img;
			}
			else if (blur == 1) {
				cv::blur(rot_img, blurred_img, cv::Size(3, 3));
			}

			// mirror
			if (rand() % 2 == 0) {
				cv::flip(blurred_img, blurred_img, 1);
			}

			// create sub folder
			if (num_generated[type] % 10000 == 0) {
				char sub_dir_name[256];
				sprintf(sub_dir_name, "%06d/", (int)(num_generated[type] / 10000));
				sub_dir[type] = boost::filesystem::path(class_dir.string() + sub_dir_name);
				boost::filesystem::create_directory(sub_dir[type]);
			}

			char filename[256];
			sprintf(filename, "%06d.png", num_generated[type]);
			cv::imwrite((sub_dir[type].string() + filename).c_str(), blurred_img);


			num_generated[type]++;
		}
	}
	printf("\n");

	return 0;
}