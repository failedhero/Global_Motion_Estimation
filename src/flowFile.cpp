#include "FlowFile.hpp"

int FlowFile::initial()
{
	if (inputPath.empty() && !_access(inputPath.c_str(), 4))
	{
		std::cerr << "Initial Failed, Please check the input file path: \"" << inputPath << "\"." << std::endl;
		return 1;
	}
	std::ifstream file(inputPath, std::fstream::binary);
	if (!file.is_open())
	{
		std::cerr << "File Read Failed, Accessed Denified: \"" << inputPath << "\"." << std::endl;
		return 1;
	}
	file.read((char*)&tag, sizeof(float));
	file.read((char*)&width, sizeof(float));
	file.read((char*)&height, sizeof(float));

	if (width <= 0 || height <= 0 || width >= 9999 || height >= 9999)
	{
		std::cerr << "Height: " << height << ", Width: " << width << ", Exclude the range(1-9999)." << std::endl;
		return 1;
	}

	uFlow = cv::Mat(height, width, CV_32F);
	vFlow = cv::Mat(height, width, CV_32F);

	int idx = 0;
	bool flag = true;
	float tmp = 0.0;

	while (file.read((char*)&tmp, sizeof(float)))
	{
		if (flag)
		{
			uFlow.at<float>(idx) = tmp;
			flag = false;
		}else{
			vFlow.at<float>(idx) = tmp;
			flag = true;
			++idx;
		}
	}
	file.close();

	if (!flag)
	{
		std::cerr << "Flow File is not complete." << std::endl;
		return 1;
	}
}

void FlowFile::changeSize(int ratio)
{
	if (ratio > 1)
	{
		ratio = ratio / 2 * 2;
		cv::pyrDown(uFlow, uFlow, cv::Size(width/ratio, height/ratio));
		cv::pyrDown(vFlow, vFlow, cv::Size(width/ratio, height/ratio));
		width = uFlow.cols;
		height = vFlow.rows;

		cv::medianBlur(uFlow, uFlow, 3);
		cv::medianBlur(vFlow, vFlow, 3);
	}else{
		std::cerr << "Please Re-Confirm the ratio, it should be larger than one." << std::endl;
		std::cout << "Continue without nothing changed or exit. Y(y) or N(n)." << std::endl;
		std::string ans;
		std::cin >> ans;
		if (ans != "Y" && ans != "y")
			exit(1);
	}
}