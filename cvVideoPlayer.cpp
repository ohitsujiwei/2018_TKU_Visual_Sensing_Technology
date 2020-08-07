#include "stdafx.h"
#include "cvVideoPlayer.h"
#define Video_Window "Video_Window"
#define Image_Window "Image_Window"
#define ROI_Window "ROI_Window"


//////////////////////////////////
/////                        /////
/////     Static Members     /////
/////                        /////
//////////////////////////////////

cv::Mat		CVideoPlayer::m_Image = cv::Mat();
cv::Mat		CVideoPlayer::m_ROI = cv::Mat();
cv::Rect	CVideoPlayer::m_TargetRect = { 0, 0, 0, 0 };
cv::Point	CVideoPlayer::m_Origin = { 0,0 };

/////////////////////////////////////////////////
/////                                       /////
/////     Constructor and Deconstructor     /////
/////                                       /////
/////////////////////////////////////////////////

CVideoPlayer::CVideoPlayer() : CCamCapture2(){
}

CVideoPlayer::~CVideoPlayer() {
	DestroyCapture();
	if (m_heventThreadDone) {
		CloseHandle(m_heventThreadDone);
		m_heventThreadDone = NULL;
	}
}

void CVideoPlayer::ConvertPath(CString path) {

	// Convert CString to std::string
	CT2CA pszConvertedAnsiString(path); // Convert a TCHAR string to a LPCSTR
	std::string dst_path(pszConvertedAnsiString); // construct a std::string using the LPCSTR input

	m_FilePath = dst_path;
}

//////////////////////////////////////////////
/////                                    /////
/////     Protected Member Functions     /////
/////                                    /////
//////////////////////////////////////////////

void CVideoPlayer::doGrabLoop() {
	cv::Mat frame;

	onGrabLoop_cvInit();

	while (m_bCamInited) {
		if (m_State == START) {
			m_cap >> frame;

			if (frame.rows == 0 || frame.cols == 0) {
				PauseCapture();
				continue;
			}

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
				imshow(Video_Window, frame);
			}
			else
				imshow(Video_Window, frame);
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
				imshow(Video_Window, frame);
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

void CVideoPlayer::onGrabLoop_cvInit() {
	if (!m_cap.isOpened())
		m_cap.open(m_FilePath);

	if (m_cap.isOpened()) {
		m_bCamInited = TRUE;
		//m_State = PAUSE;
		m_State = START;
		cvNamedWindow(Video_Window);
		//cvNamedWindow(ROI_Window);
		cvSetMouseCallback(Video_Window, onMouseCB2, NULL);
	}
	else
		m_bCamInited = FALSE;
}

////////////////////////////////////////////
/////                                  /////
/////     Private Member Functions     /////
/////                                  /////
////////////////////////////////////////////

void CVideoPlayer::onMouseCB2(int event, int x, int y, int flags, void * param) {
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

void CVideoPlayer::onGrabLoop_DrawROI(cv::Mat& frame) {
	if (m_bTargetObj)
		if (m_TargetRect.width > 0 && m_TargetRect.height > 0) {
			cv::bitwise_not(frame(m_TargetRect), frame(m_TargetRect));
			cv::rectangle(frame, m_TargetRect, cv::Scalar(0, 255, 0), 2);
		}
}

