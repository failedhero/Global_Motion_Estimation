#include "GMEParameter.hpp"

GMEParameter::GMEParameter(std::shared_ptr<globalMotionEstimation> gme) : GME(gme)
{
	mask = *(GME->mask.end() - 1);
	height = mask.rows;
	width = mask.cols;

	GME->flow.uFlow.copyTo(xCurPos);
	GME->flow.vFlow.copyTo(yCurPos);
	xCurPos += GME->xPos;
	yCurPos += GME->yPos;

	sigma1 = 0.01;
	sigma2 = 0.000001;
}
/*GMEParameter::getParameter()
	find the GME parameters via interations*/
void GMEParameter::getParameter()
{
	if (initial())
	{
		std::cerr << "GMEParameter Initial Failed." << std::endl;
		exit(1);
	}

	while (thi.checkIfStop())
	{
		thi.calculateParameter();
		thi.updateMask();
		parameter.copyTo(preParameter);
		++reGetCnt;
	}
}
/*GMEParameter::initial()
	initial elements before each iteration*/
int GMEParameter::initial()
{
	reGetCnt = 1;
	parDiff1 = 1;
	parDiff2 = 1;	
	breakFlag = false;

	diffVar.erase(diffVar.begin(), diffVar.end());
	diffVar.push_back(100.0);
	preParameter = cv::Mat::zeros(6,1,CV_32F);

	parameter.at<float>(0) = 1.0;
	parameter.at<float>(1) = 0.0;
	parameter.at<float>(2) = cv::mean(GME->flow.uFlow).val[0];
	parameter.at<float>(3) = 0.0;
	parameter.at<float>(4) = 1.0;	
	parameter.at<float>(5) = cv::mean(GME->flow.vFlow).val[0];

	cv::Mat tmp1,tmp2,motionVector,ltFlag,gtFlag;
	cv::pow(GME->flow.uFlow, 2, tmp1);
	cv::pow(GME->flow.vFlow, 2, tmp2);
	cv::add(tmp1,tmp2,motionVector);
	cv::pow(motionVector, 0.5, motionVector);

	cv::meanStdDev(motionVector, tmp1, tmp2);
	cv::compare(motionVector, tmp1.at<double>(0) + tmp2.at<double>(0), ltFlag, cv::CMP_LT);
	cv::compare(motionVector, tmp1.at<double>(0) - tmp2.at<double>(0), gtFlag, cv::CMP_GT);
	ltFlag.convertTo(ltFlag, CV_32F, 1.0/255.0, 0);
	gtFlag.convertTo(gtFlag, CV_32F, 1.0/255.0, 0);
	cv::multiply(ltFlag, mask, mask);
	cv::multiply(gtFlag, mask, mask);
	return 0;
}

/*GMEParameter::updateMask()
	refresh the mask after calculating parameters*/
void GMEParameter::updateMask()
{
	cv:: Mat diffPos, tmp1, tmp2;
	cv::absdiff(xCalPos, xCurPos, tmp1);
	cv::absdiff(yCalPos, yCurPos, tmp2);
	cv::add(tmp1, tmp2, diffPos);
	cv::meanStdDev(diffPos, tmp1, tmp2);

	if (*(diffVar.end() - 1) <= tmp2.at<double>(0) && tmp2.at<double>(0) < 1)
		breakFlag = true;
	else{
		breakFlag = false;
		diffVar.push_back(tmp2.at<double>(0));
		errorHist(diffPos);
	}
}

/*GMEParameter::errorHist()
	refresh mask via calculate histogram of diffPos*/
void GMEParameter::errorHist(const cv::Mat &diffPos)
{
	// configure the histogram parameters
	const int channels[1] = {0};
	const int histSize[1] = {10};

	double minv,maxv;
	cv::minMaxIdx(diffPos, &minv, &maxv);
	float hRanges[2] = {minv, maxv};
	const float *ranges[1] = {hRanges};

	cv::Mat hist;
	cv::calcHist(&diffPos, 1, channels, cv::Mat(), hist, 1, histSize, ranges);

	// process the result of histogram
	const int N = height * width;
	const int cnt = hist.rows;
	
	cv::Mat binVal(cnt+1, 1, CV_32F);	
	for (int h=0;h<=cnt;h++)
		binVal.at<float>(h) = minv + (maxv - minv) * h / cnt;

	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(hist, &minv, &maxv, &minLoc, &maxLoc);
	float errMax = binVal.at<float>(maxLoc);	

	cv::Mat errTmp = diffPos;
	cv::Mat &w = hist;
	errTmp -= errMax;
	w -= N;

	cv::divide(w, cv::mean(w).val[0], w);

	cv::Mat err = cv::Mat::zeros(errTmp.size(),errTmp.depth());
	cv::Mat tmp1,tmp2;
	for (int i=0; i!=cnt; i++)
	{
		cv::Mat errFlag;
		if (i == 0){
			cv::compare(diffPos, binVal.at<float>(i), tmp1, cv::CMP_GE);
			cv::compare(diffPos, binVal.at<float>(i+1), tmp2, cv::CMP_LE);
			
			tmp1.convertTo(tmp1,CV_32F,1.0/255.0,0);
			tmp2.convertTo(tmp2,CV_32F,1.0/255.0,0);
			cv::multiply(tmp1, tmp2, errFlag);
			
			cv::multiply(errFlag, errTmp, errFlag);
			cv::scaleAdd(errFlag, w.at<float>(i), err, err);
		}else{
			cv::compare(diffPos, binVal.at<float>(i), tmp1, cv::CMP_GT);
			cv::compare(diffPos, binVal.at<float>(i+1), tmp2, cv::CMP_LE);

			tmp1.convertTo(tmp1, CV_32F, 1.0/255.0, 0);
			tmp2.convertTo(tmp2, CV_32F, 1.0/255.0, 0);
			cv::multiply(tmp1, tmp2, errFlag);
			cv::multiply(errFlag, errTmp, errFlag);
			cv::scaleAdd(errFlag, w.at<float>(i), err, err);
		}
	}
	cv::pow(err, 2, mask);
	err = cv::abs(err);
	cv::compare(err, 1, err, cv::CMP_LT);
	err.convertTo(err, CV_32F, 1.0/255.0, 0);
	cv::pow(mask, 2, mask);
	cv::multiply(1 - mask, err, mask);	
}

/*GMEParameter2::checkIfStop()
	check if stop the interation via justice parDiff1*/
bool GMEParameter2::checkIfStop()
{
	cv::Mat tmp1;
	cv::pow(parameter - preParameter, 2, tmp1);
	parDiff1 = (tmp1.at<float>(2) + tmp1.at<float>(5)) * 0.5;
	if (parDiff1 >= sigma1 && reGetCnt <= 32 && !breakFlag)
		return true;
	else
		return false;
}

/*GMEParameter6::checkIfStop()
	check if stop the interation via justice parDiff1 and parDiff2*/
bool GMEParameter6::checkIfStop()
{
	cv::Mat tmp1;
	cv::pow(parameter - preParameter, 2, tmp1);
	parDiff1 = (tmp1.at<float>(2) + tmp1.at<float>(5)) * 0.5;
	parDiff2 = (tmp1.at<float>(0) + tmp1.at<float>(1) + tmp1.at<float>(3) + tmp1.at<float>(4)) * 0.25;
	if (parDiff1 >= sigma1 && parDiff2 >= sigma2 && reGetCnt <= 32 && !breakFlag)
		return true;
	else
		return false;
}

/*GMEParameter2::calculateParameter()
	calculate the GME parameters with 2P method*/
void GMEParameter2::calculateParameter()
{
	GME->xPos.copyTo(xCalPos);
	GME->yPos.copyTo(yCalPos);

	xCalPos += parameter.at<float>(2);
	yCalPos += parameter.at<float>(5);

	cv::Mat alpha, beta;
	cv::multiply((xCalPos - xCurPos), mask, alpha);
	cv::multiply((yCalPos - yCurPos), mask, beta);

	parameter.at<float>(2) += cv::sum(alpha).val[0] / cv::sum(mask).val[0];
	parameter.at<float>(5) += cv::sum(beta).val[0] / cv::sum(mask).val[0];
}

/*GMEParameter6::calculateParameter()
	calculate the GME parameters with 6P method*/
void GMEParameter6::calculateParameter()
{
	cv::Mat H(3,3,CV_32F);
	cv::Mat B(3,2,CV_32F);

	cv::Mat tmp1,tmp2,tmp3,alpha,beta;
	cv::multiply(GME.xPos, GME.xPos, tmp1);
	cv::multiply(tmp1, mask, tmp1);
	H.at<float>(0,0) = cv::sum(tmp1).val[0];
	cv::multiply(GME.xPos, GME.yPos, tmp1);
	cv::multiply(tmp1, mask, tmp1);
	H.at<float>(0,1) = cv::sum(tmp1).val[0];
	cv::multiply(GME.xPos, mask, tmp1);
	H.at<float>(0,2) = cv::sum(tmp1).val[0];

	H.at<float>(1,0) = H.at<float>(0,1);
	cv::multiply(GME.yPos, GME.yPos, tmp1);
	cv::multiply(tmp1, mask, tmp1);
	H.at<float>(1,1) = cv::sum(tmp1).val[0];
	cv::multiply(GME.yPos, mask, tmp1);
	H.at<float>(1,2) = cv::sum(tmp1).val[0];

	H.at<float>(2,0) = H.at<float>(0,2);
	H.at<float>(2,1) = H.at<float>(1,2);
	H.at<float>(2,2) = cv::sum(mask).val[0];

	cv::addWeighted(GME.xPos, parameter.at<float>(0), GME.yPos, parameter.at<float>(1), parameter.at<float>(2), alpha);
	cv::addWeighted(GME.xPos, parameter.at<float>(3), GME.yPos, parameter.at<float>(4), parameter.at<float>(5), beta);

	cv::multiply(alpha, GME.xPos, tmp1);
	cv::multiply(*xCurPos, GME.xPos, tmp2);
	cv::multiply(tmp1 - tmp2, mask, tmp3);
	B.at<float>(0,0) = -cv::sum(tmp3).val[0];
	cv::multiply(alpha, GME.yPos, tmp1);
	cv::multiply(*xCurPos, GME.yPos, tmp2);
	cv::multiply(tmp1 - tmp2, mask, tmp3);
	B.at<float>(1,0) = -cv::sum(tmp3).val[0];
	cv::multiply(alpha - *xCurPos, mask, tmp3);
	B.at<float>(2,0) = -cv::sum(tmp3).val[0];

	cv::multiply(beta, GME.xPos, tmp1);
	cv::multiply(*yCurPos, GME.xPos, tmp2);
	cv::multiply(tmp1 - tmp2, mask, tmp3);
	B.at<float>(0,1) = -cv::sum(tmp3).val[0];
	cv::multiply(beta, GME.yPos, tmp1);
	cv::multiply(*yCurPos, GME.yPos, tmp2);
	cv::multiply(tmp1 - tmp2, mask, tmp3);
	B.at<float>(1,1) = -cv::sum(tmp3).val[0];
	cv::multiply(beta - *yCurPos, mask, tmp3);
	B.at<float>(2,1) = -cv::sum(tmp3).val[0];

	cv::invert(H, tmp1);
	tmp2 = tmp1 * B.col(0);
	tmp3 = tmp1 * B.col(1);

	parameter.at<float>(0) += tmp2.at<float>(0);
	parameter.at<float>(1) += tmp2.at<float>(1);
	parameter.at<float>(2) += tmp2.at<float>(2);
	parameter.at<float>(3) += tmp3.at<float>(0);
	parameter.at<float>(4) += tmp3.at<float>(1);
	parameter.at<float>(5) += tmp3.at<float>(2);

	cv::addWeighted(GME.xPos, parameter.at<float>(0), GME.yPos, parameter.at<float>(1), parameter.at<float>(2), xCalPos);
	cv::addWeighted(GME.xPos, parameter.at<float>(3), GME.yPos, parameter.at<float>(4), parameter.at<float>(5), yCalPos);	
}