#pragma once
#include <opencv2/opencv.hpp>
#include "cvCamCapture.h"

typedef void(__cdecl *pfnCaptureCallback2)(cv::Mat& Frame);	// OpenCV 2.0

class CCamCapture2 : public CCamCapture {
public:
	// Member Function
	CCamCapture2();
	~CCamCapture2();
	cv::Mat* GetSelectedROI() const;
	cv::Rect GetTargetRect() const;
	void SetTargetRect(cv::Rect Rect);
	void SetCaptureCallback(pfnCaptureCallback2 pfnCaptureCB);

protected:
	// Member Function
	virtual void doGrabLoop();
	virtual void onGrabLoop_cvInit();
	virtual void onGrabLoop_cvClose();

private:
	// Member Function
	static void onMouseCB2(int event, int x, int y, int flags, void* param);
	void onGrabLoop_DrawROI(cv::Mat& frame);

	// Static Member Variable
	static cv::Mat m_Image;
	static cv::Mat m_ROI;
	static cv::Rect m_TargetRect;
	static cv::Point m_Origin;

	// Member Variable
	cv::VideoCapture m_cap;
	pfnCaptureCallback2 m_pfnCustomCB;
};

