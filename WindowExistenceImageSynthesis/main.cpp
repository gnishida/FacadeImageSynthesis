#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <iomanip>

int main() {
	// read the parameter file
	std::ifstream in("window_params.txt");
	std::map<std::string, int> params;
	std::map<int, int> existence_distribution;

	existence_distribution[0] = 0;	// not exist
	existence_distribution[1] = 0;	// exist

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

		// if the window does not exist
		if (fabs(x1 - x2) * 800 < 3 && fabs(y1 - y2) * 800 < 3) {
			params[strs[0]] = 0;
			existence_distribution[0]++;
		}
		else {
			params[strs[0]] = 1;
			existence_distribution[1]++;
		}

#if 0
		std::string filename;
		float x1, y1, x2, y2;
		in >> filename >> x1 >> y1 >> x2 >> y2;

		if (abs(x1 - x2) * 800 < 3 && abs(y1 - y2) * 800 < 3) {
			params[filename] = 0;
			existence_distribution[0]++;
		}
		else {
			params[filename] = 1;
			existence_distribution[1]++;
		}

		if (filename == "") break;
#endif
	}


	for (auto it = existence_distribution.begin(); it != existence_distribution.end(); ++it) {
		std::cout << it->first << ", cnt=" << it->second << std::endl;
	}

	// num of variation for each type
	std::map<int, int> num_variations;
	for (auto it = existence_distribution.begin(); it != existence_distribution.end(); ++it) {
		int num = it->second;
		num_variations[it->first] = 100000 / num;
	}

	printf("Generating images: ");
	boost::filesystem::path dir_data("../tiles/");
	boost::filesystem::path dir_result("//cuda.cs.purdue.edu/scratch2/window_existence/data/images/");
	std::vector<boost::filesystem::path> sub_dir(num_variations.size());

	int cnt = 0;
	std::vector<int> num_generated(num_variations.size(), 0);
	for (auto it = boost::filesystem::directory_iterator(dir_data); it != boost::filesystem::directory_iterator(); ++it) {
		if (boost::filesystem::is_directory(it->path())) continue;

		// read an image
		cv::Mat img = cv::imread(dir_data.string() + it->path().filename().string());

		// convert the image to grayscale
		cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

		// get parameter values
		if (params.find(it->path().filename().string()) == params.end()) continue;
		int exist = params[it->path().filename().string()];

		// class directory
		std::stringstream ss;
		ss << std::setw(2) << std::setfill('0') << exist + 1;

		// create class directory
		boost::filesystem::path class_dir(dir_result.string() + ss.str() + "/");
		if (!boost::filesystem::exists(class_dir)) {
			boost::filesystem::create_directory(class_dir);
		}

		for (int i = 0; i < num_variations[exist]; ++i) {
			printf("\rGenerating images: %d", cnt + 1);
			cnt++;

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
			int change = rand() % 50 - 25;
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
			if (num_generated[exist] % 10000 == 0) {
				char sub_dir_name[256];
				sprintf(sub_dir_name, "%06d/", (int)(num_generated[exist] / 10000));
				sub_dir[exist] = boost::filesystem::path(class_dir.string() + sub_dir_name);
				boost::filesystem::create_directory(sub_dir[exist]);
			}

			char filename[256];
			sprintf(filename, "%06d.png", num_generated[exist]);
			cv::imwrite((sub_dir[exist].string() + filename).c_str(), crop_img);


			num_generated[exist]++;
		}
	}
	printf("\n");

	return 0;
}