#pragma once
#include "cvCamCapture2.h"

class CVideoPlayer : public CCamCapture2 {
public:
	CVideoPlayer();
	~CVideoPlayer();
	void ConvertPath(CString path);

protected:
	// Member Function
	virtual void doGrabLoop();
	virtual void onGrabLoop_cvInit();

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
	std::string m_FilePath;
	cv::VideoCapture m_cap;
	pfnCaptureCallback2 m_pfnCustomCB;
};

