#include "globalMotionEstimation.hpp"

int main(int argc, char *argv[])
{
	std::string inputDir("E:/ProcessVideo/CSIQ/BasketballDrive_832x480_dst_00");
	globalMotionEstimation result(inputDir);
	result.calculateParameter();
	// parameter:	std::vector<cv::Mat> result.parameter
	// mask:		std::vector<cv::Mat> result.mask
	system("pause");
	return 0;
}