#include "flowFile.hpp"

int flowFile::initial()
{
	if (inputPath.empty() && !_access(inputPath.c_str(), 4))
	{
		std::cerr << "Initial Failed, Please check the input file path: \"" << inputPath << "\"." << std::endl;
		return 1;
	}
	FILE *fid;
	fopen_s(&fid, inputPath.c_str(), "rb");
	if (fid == 0)
	{
		std::cerr << "File Read Failed, Accessed Denified: \"" << inputPath << "\"." << std::endl;
		return 1;
	}
	fread(&tag, sizeof(float), 1, fid);
	fread(&width, sizeof(int), 1, fid);
	fread(&height, sizeof(int), 1, fid);

	if (width <= 0 || height <= 0 || width >= 9999 || height >= 9999)
	{
		std::cerr << "Height: " << height << ", Width: " << width << ", Exclude the range(1-9999)." << std::endl;
		return 1;
	}

	uFlow = cv::Mat(height, width, CV_32F);
	vFlow = cv::Mat(height, width, CV_32F);

	int idx = 0;
	bool flag = true;
	float tmp;

	while (fread(&tmp, sizeof(float), 1, fid))
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
	fclose(fid);

	if (!flag)
	{
		std::cerr << "Flow File is not complete." << std::endl;
		return 1;
	}
}

void flowFile::changeSize(int ratio)
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