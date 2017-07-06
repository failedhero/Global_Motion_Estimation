#ifndef _GMEPARAMETER_HPP
#define _GMEPARAMETER_HPP

#include "globalMotionEstimation.hpp"

/*class gmeParameters:
	calculate GME parameters and as an sub-process of globalMotionEstimation
	Initiated with a reference of globalMotionEstimation*/
class GMEParameter
{
protected:
	friend class globalMotionEstimation;
	GMEParameter(globalMotionEstimation *gme);
	~GMEParameter() = default;
	
	virtual void calculateParameter() = 0;
	virtual bool checkIfStop() = 0;

	void getParameter();
	int initial();	
	void updateMask();
	void errorHist(const cv::Mat &diffPos);

	globalMotionEstimation *GME;

	cv::Mat xCurPos, yCurPos, xCalPos, yCalPos, preParameter, parameter, mask;
	std::vector<float> diffVar;

	int reGetCnt, height, width;
	float parDiff1, sigma1, parDiff2, sigma2;
	bool breakFlag;
};

class GMEParameter2 : public GMEParameter
{
public:
	GMEParameter2(globalMotionEstimation *gme) : GMEParameter(gme) { }
	bool checkIfStop();
	void calculateParameter();
};

class GMEParameter6 : public GMEParameter
{
public:
	GMEParameter6(globalMotionEstimation *gme) : GMEParameter(gme) { }
	bool checkIfStop();
	void calculateParameter();
};

#endif