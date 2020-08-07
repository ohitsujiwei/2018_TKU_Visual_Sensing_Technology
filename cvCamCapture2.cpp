#include "stdafx.h"
#include "cvCamCapture2.h"
#define Camera_Window "Camera_Window"
#define ROI_Window "ROI_Window"

//////////////////////////////////
/////                        /////
/////     Static Members     /////
/////                        /////
//////////////////////////////////

cv::Mat		CCamCapture2::m_Image = cv::Mat();
cv::Mat		CCamCapture2::m_ROI = cv::Mat();
cv::Rect	CCamCapture2::m_TargetRect = { 0, 0, 0, 0 };
cv::Point	CCamCapture2::m_Origin = { 0,0 };

/////////////////////////////////////////////////
/////                                       /////
/////     Constructor and Deconstructor     /////
/////                                       /////
/////////////////////////////////////////////////

CCamCapture2::CCamCapture2() {
	m_bCamInited = FALSE;
	m_heventThreadDone = NULL;
	m_State = STOP;
	m_bSavePause = FALSE;
	m_pfnCustomCB = NULL;
	m_cap = cv::VideoCapture();
}

CCamCapture2::~CCamCapture2() {
	DestroyCapture();
	if (m_heventThreadDone) {
		CloseHandle(m_heventThreadDone);
		m_heventThreadDone = NULL;
	}
}

///////////////////////////////////////////
/////                                 /////
/////     Public Member Functions     /////
/////                                 /////
///////////////////////////////////////////

cv::Mat* CCamCapture2::GetSelectedROI() const {
	return &m_ROI;
}

cv::Rect CCamCapture2::GetTargetRect() const {
	return m_TargetRect;
}

void CCamCapture2::SetTargetRect(cv::Rect Rect) {
	m_TargetRect = Rect;
}

void CCamCapture2::SetCaptureCallback(pfnCaptureCallback2 pfnCaptureCB) {
	m_pfnCustomCB = pfnCaptureCB;
}

//////////////////////////////////////////////
/////                                    /////
/////     Protected Member Functions     /////
/////                                    /////
//////////////////////////////////////////////

void CCamCapture2::doGrabLoop() {
	cv::Mat frame;

	onGrabLoop_cvInit();

	while (m_bCamInited) {
		if (m_State == START) {
			m_cap >> frame;

			if (!frame.empty()) {
				if (m_bSavePause) {
					imwrite("SavePauseImg.bmp", frame);
					m_bSavePause = FALSE;
				}
				frame.copyTo(m_Image);
			}

			// Drawing selected ROI
			onGrabLoop_DrawROI(frame);

			if (!m_bTargetObj && m_pfnCustomCB) {
				m_pfnCustomCB(m_Image);
				imshow(Camera_Window, frame);
			}
			else
				imshow(Camera_Window, frame);
			cv::waitKey(33);
		}
		else if (m_State == PAUSE) {
			if (!m_Image.empty())
				m_Image.copyTo(frame);

			if (!frame.empty()) {
				if (m_bSavePause) {
					imwrite("SavePauseImg.bmp", frame);
					m_bSavePause = FALSE;
				}
				onGrabLoop_DrawROI(frame);
				imshow(Camera_Window, frame);
				cv::waitKey(33);
			}
		}
		else if (m_State == STOP) {
			m_bCamInited = FALSE;

			cvWaitKey(5);

			onGrabLoop_cvClose();
			Sleep(1000);

			SetEvent(m_heventThreadDone);

			if (!m_bCamInited)
				break;
		}
	}
}

void CCamCapture2::onGrabLoop_cvInit() {
	if (!m_cap.isOpened())
		m_cap.open(0);

	if (m_cap.isOpened()) {
		m_bCamInited = TRUE;
		//m_State = PAUSE;
		m_State = START;
		cvNamedWindow(Camera_Window);
		//cvNamedWindow(ROI_Window);
		cvSetMouseCallback(Camera_Window, onMouseCB2, NULL);
	}
	else
		m_bCamInited = FALSE;
}

void CCamCapture2::onGrabLoop_cvClose() {
	if (m_cap.isOpened()) {
		m_cap.release();

		cvDestroyWindow(Camera_Window);
		cvDestroyWindow(ROI_Window);
	}
}

////////////////////////////////////////////
/////                                  /////
/////     Private Member Functions     /////
/////                                  /////
////////////////////////////////////////////

void CCamCapture2::onMouseCB2(int event, int x, int y, int flags, void * param) {
	if (flags == CV_EVENT_FLAG_LBUTTON) { // «ö¤U¥ªÁä¨Ã©ì¦²
		if (x > m_TargetRect.x) {
			m_TargetRect.x = m_Origin.x;
			m_TargetRect.width = x - m_Origin.x;
		}
		if (y > m_TargetRect.y) {
			m_TargetRect.y = m_Origin.y;
			m_TargetRect.height = y - m_Origin.y;
		}
		if (x < m_TargetRect.x) {
			m_TargetRect.x = x;
			m_TargetRect.width = m_Origin.x - x;
		}
		if (y < m_TargetRect.y) {
			m_TargetRect.y = y;
			m_TargetRect.height = m_Origin.y - y;
		}
	}
	switch (event) {
	case CV_EVENT_LBUTTONDOWN:
		m_Origin = cvPoint(x, y);
		m_TargetRect = cvRect(x, y, 0, 0);
		m_bTargetObj = 1;
		break;
	case CV_EVENT_LBUTTONUP:
		m_bTargetObj = 0;
		if (m_TargetRect.width > 0 && m_TargetRect.height > 0) {
			m_Image(m_TargetRect).copyTo(m_ROI);
			imshow(ROI_Window, m_ROI);
		}
		break;
	default:
		break;
	}
}

void CCamCapture2::onGrabLoop_DrawROI(cv::Mat& frame) {
	if (m_bTargetObj)
		if (m_TargetRect.width > 0 && m_TargetRect.height > 0) {
			cv::bitwise_not(frame(m_TargetRect), frame(m_TargetRect));
			cv::rectangle(frame, m_TargetRect, cv::Scalar(0, 255, 0), 2);
		}
}
