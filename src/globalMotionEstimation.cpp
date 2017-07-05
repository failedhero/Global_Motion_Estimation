#include "globalMotionEstimation.hpp"

/*globalMotionEstimation::globalMotionEstimation()
s: contains flow Directory and frame images
	<rootPath> --- frame images
			=>flow Directory <rootPath>/flow/
			 --- flow files
ratio: default ratio = 1*/
globalMotionEstimation::globalMotionEstimation(const std::string s, const int r) : rootPath(s), ratio(r)
{
	std::string rootPath(s);
	checkInPutDir(rootPath);
	inputIMG = inputFilePath(rootPath, "png");
	inputFlow = inputFilePath(rootPath.append("flow/"), "flo");

	parameter = std::make_shared<std::vector<cv::Mat>>();
	mask = std::make_shared<std::vector<cv::Mat>>();
}

/*globalMotionEstimation::initial()
Check the files in rootPath and get file lists of image and flow*/
int globalMotionEstimation::initial()
{
	// initial file info
	// members:
	// std::map<std::string, std::string> filePath
	// std::vector<std::string> fileName
	if (!inputIMG.initial(false))
		return 1;
	if (!inputFlow.initial(false))
		return 1;
	if (inputIMG.fileName.size() != inputFlow.fileName.size() + 1)
	{
		std::cerr << "Check the NUM of the files, match failed: IMAGE - " << inputIMG.fileName.size() << " FLOW - " << inputFlow.fileName.size() << std::endl;
		return 1;
	}else{
		nbframes = inputIMG.fileName.size();
		return 0;	
	}
}

/*globalMotionEstimation::calculateParameter()
	get GME parameters of motionvector of each frame with GMEParameter*/
void globalMotionEstimation::calculateParameter()
{
	if (initial())
		exit(1);

	for (int i = 0; i < nbframes -1; ++i)
	{
		calculateMotionVector(i);
		GMEModel->getParameter();
		mask.push_back(GMEModel->mask);
		parameter.push_back(GMEModel->parameter);
		showMask(GMEModel->mask);
	}		
}

/*globalMotionEstimation::calculateMotionVector()
	read the flowfile and refresh the motionvector, refresh gp 
	with a pointer of gmeParameters for calculating of GME parameters
cnt: input,the index of frames for processing */
void globalMotionEstimation::calculateMotionVector(const int cnt)
{
	std::string flowPath(inputFlow.filePath.at(inputFlow.fileName[cnt]));
	flow = std::make_shared<flowFile>(inputFlow);
	flow->initial();

	if (cnt == 0)
	{
		mask->push_back(cv::Mat::ones(flow->height, flow->width, CV_32F));
		geneuratePosition(xPos, flow->height, flow->width, true);
		geneuratePosition(yPos, flow->height, flow->width, false);
	}
	GMEModel = checkRatio(cnt);
}

/*globalMotionEstimation::checkRatio()
	check which method should be used and return with a pointer to 
	gmeParameters2 or gmeParameters6
cnt: input,the index of frames for processing*/
std::shared_ptr<GMEParameter> globalMotionEstimation::checkRatio(const int cnt)
{
	if (cnt == 0)
		return std::make_shared<GMEParameter6>(this);

	float hRatio, vRatio;
	calculateRatio(inputFlow.filePath.at(inputFlow.fileName[cnt-1]), inputFlow.filePath.at(inputFlow.fileName[cnt]), hRatio, vRatio);

	if (hRatio >= 0/5 || vRatio >= 0.5)
		return std::make_shared<GMEParameter2>(this);
	else
		return std::make_shared<GMEParameter6>(this);
}

/*calculateRatio: choose the method of GME calculation with 2-parameters
				or 6-parameters via calculating ratio
preImagePath: input,path of pre-frame image
curImagePath: input,path of current frame image
hRatio: output
vRatio: output*/
void calculateRatio(const std::string preIMGPath, const std::string curIMGPath, float &hRatio, float &wRatio)
{
	cv::Mat preIMG, curIMG, diffIMG, xGRDIMG, yGRDIMG;
	preIMG = cv::imread(preImagePath, CV_LOAD_IMAGE_GRAYSCALE);
	curIMG = cv::imread(curImagePath, CV_LOAD_IMAGE_GRAYSCALE);

	cv::GaussianBlur(preIMG, preIMG, cv::Size(5,5), 3, 3);
	cv::GaussianBlur(curIMG, curIMG, cv::Size(5,5), 3, 3);

	cv::subtract(curIMG, preIMG, diffIMG);
	diffIMG.converTo(diffIMG, CV_32F);
	cv::Sobel(preIMG, xGRDIMG, CV_32F, 1,0,1,1,0, cv::BORDER_DEFAULT);
	cv::Sobel(preIMG, yGRDIMG, CV_32F, 0,1,1,1,0, cv::BORDER_DEFAULT);

	cv::Mat hSTGS = cv::Mat(preImage.size(), CV_32F);
	cv::Mat vSTGS = cv::Mat(preImage.size(), CV_32F);
	cv::divide(diffImage, xGrdImage, hSTGS);
	cv::divide(diffImage, yGrdImage, vSTGS);

	hRatio = defineMinority(hSTGS);
	vRatio = defineMinority(vSTGS);
}

/*defineMinority: calculate ratio of image difference and 
				image gradient
STGS: input,the result of calculateRatio*/
float defineMinority(cv::Mat &STGS)
{
	int h = STGS.rows, w = STGS.cols;

	cv::Mat negativeMap, positiveMap, mulMap, powxMap, powyMap, xPos, yPos;
	cv::compare(STGS, 0, negativeMap, cv::CMP_LT);
	cv::compare(STGS, 0, positiveMap, cv::CMP_GT);

	unsigned int negNum = cv::sum(negativeMap).val[0];
	unsigned int posNum = cv::sum(positiveMap).val[0];

	cv::Mat &minMap(negNum > posNum ? positiveMap : negativeMap);

	geneuratePosition(xPos, h, w, true);
	geneuratePosition(yPos, h, w, true);

	minMap.convertTo(minMap,CV_32F,1.0/255.0,0);
	unsigned int minNum = cv::sum(minMap).val[0];
	cv::multiply(xPos, minMap, mulMap);
	int xCenter = cv::sum(mulMap).val[0] / minNum;
	cv::multiply(yPos, minMap, mulMap);
	int yCenter = cv::sum(mulMap).val[0] / minNum;

	cv::pow((xPos - xCenter), 2, powxMap);
	cv::pow((yPos - yCenter), 2, powyMap);

	cv::multiply(powxMap, minMap, mulMap);
	float varience = cv::sum(mulMap).val[0];
	cv::multiply(powyMap, minMap, mulMap);
	varience += cv::sum(mulMap).val[0];
	return varience / (minNum * minNum);
}

void showmask(cv::Mat data)
{
	// show the data in each interation
	cv::Mat test;
	double testminv, testmaxv;
	cv::namedWindow("GlobalMotionEstimation");
	cv::normalize(data, test, 1, 0, cv::NORM_MINMAX);
	test.convertTo(test, CV_8U, 255, 0);
	cv::minMaxIdx(data, &testminv, &testmaxv);
	std::cout << testminv << " " << testmaxv << std::endl;
	std::cout << cv::sum(test).val[0] << std::endl;

	cv::imshow("GlobalMotionEstimation", test);
	cv::waitKey(0);
	cv::destroyAllWindows();
}

/*geneuratePosition: generate a defined metrix with positions values
	of each one
pos: the generated position matrix
h: input,height of metrix
w: input,width of metrix
rowflag: generate x-axis positions or y-axis positions
		 true for x-axis and false for y-axis*/
void geneuratePosition(cv::Mat &pos, int h, int w, bool rowflag)
{
	pos = cv::Mat::ones(h, w, CV_16U);
	if (rowflag)
	{
		for (int i = 0; i != w; i++)
			pos.col(i) *= i;
		pos -= w/2;
	}
	else{
		for (int i = 0; i != h; i++)
			pos.row(i) *= i;
		pos = h/2 - pos;
	}
	pos.convertTo(pos, CV_32F);
}