#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>

int main() {
	// read the parameter file
	std::ifstream in("floors_columns.txt");
	std::map<std::string, std::vector<float>> params;
	while (!in.eof()) {
		std::string filename;
		int num_floors;
		int num_columns;
		in >> filename >> num_floors >> num_columns;
		params[filename].push_back(num_floors);
		params[filename].push_back(num_columns);

		if (filename == "") break;
	}

	int cnt = 0;
	printf("Generating images: ");
	boost::filesystem::path dir_data("facade/");
	boost::filesystem::path dir_result("//cuda.cs.purdue.edu/scratch2/facade_floors/data/images/01/");
	//boost::filesystem::path dir_result("C:/Anaconda/caffe/facade_floors/data/images/01/");
	boost::filesystem::path sub_dir;

	std::ofstream out_param(dir_result.string() + "parameters.txt");

	for (auto it = boost::filesystem::directory_iterator(dir_data); it != boost::filesystem::directory_iterator(); ++it) {
		if (boost::filesystem::is_directory(it->path())) continue;

		// read an image
		cv::Mat img = cv::imread(dir_data.string() + it->path().filename().string());

		// get parameter values
		int num_floors = params[it->path().filename().string()][0];
		int num_columns = params[it->path().filename().string()][1];

		float tile_height = (float)img.rows / num_floors;
		float tile_width = (float)img.cols / num_columns;

		int num_samples = 150;

		for (int iter = 0; iter < num_samples; ++iter) {
			printf("\rGenerating images: %d", cnt + 1);

#if 1
			// rotate the image
			cv::Mat rot_img;
			cv::Point2f offset(img.cols / 2 + rand() % (int)tile_width * 2 - tile_width, img.rows / 2 + rand() % (int)tile_height * 2 - tile_height);
			float angle = (float)(rand() % 200 - 100) / 100.0f;
			cv::Mat rot_mat = cv::getRotationMatrix2D(offset, angle, 1.0);
			cv::warpAffine(img, rot_img, rot_mat, img.size(), cv::INTER_CUBIC, cv::BORDER_REPLICATE);

			// crop the image
			int x1 = rand() % (int)(tile_width * 0.1);
			int y1 = rand() % (int)(tile_height * 0.1);
			int x2 = rot_img.cols - 1 - rand() % (int)(tile_width * 0.1);
			int y2 = rot_img.rows - 1 - rand() % (int)(tile_height * 0.1);
			cv::Mat crop_img(rot_img, cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1));

			// change the color
			cv::cvtColor(crop_img, crop_img, cv::COLOR_BGR2GRAY);
			float change_brightness = (float)(rand() % 60 + 70) / 100;
			for (int r = 0; r < crop_img.rows; ++r) {
				for (int c = 0; c < crop_img.cols; ++c) {
					crop_img.at<uchar>(r, c) = std::min(255, (int)(crop_img.at<uchar>(r, c) * change_brightness));
				}
			}
			cv::cvtColor(crop_img, crop_img, cv::COLOR_GRAY2BGR);

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


			for (int k = 0; k < params[it->path().filename().string()].size(); ++k) {
				if (k > 0) out_param << ",";
				out_param << params[it->path().filename().string()][k];
			}
			out_param << "\n";
#endif
		}
	}
	printf("\n");

	out_param.close();

	return 0;
}