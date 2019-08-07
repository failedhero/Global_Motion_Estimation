#include "GlobalMotionEstimation.hpp"

int main(int argc, char *argv[]) {
  std::string inputDir("input_dir");
  GlobalMotionEstimation result(inputDir);
  result.calculateParameter();
  // parameter:	std::vector<cv::Mat> result.parameter
  // mask:	std::vector<cv::Mat> result.mask
  system("pause");
  return 0;
}