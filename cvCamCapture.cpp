#include "stdafx.h"
#include "cvCamCapture.h"
#define Camera_Window "Camera_Window"
#define ROI_Window "ROI_Window"

//////////////////////////////////
/////                        /////
/////     Static Members     /////
/////                        /////
//////////////////////////////////

IplImage*	CCamCapture::m_pImage = NULL;
IplImage*	CCamCapture::m_pROI = NULL;
BOOL		CCamCapture::m_bTargetObj = FALSE;
CvRect		CCamCapture::m_TargetRect = { 0, 0, 0, 0 };
CvPoint		CCamCapture::m_Origin = { 0,0 };

/////////////////////////////////////////////////
/////                                       /////
/////     Constructor and Deconstructor     /////
/////                                       /////
/////////////////////////////////////////////////

CCamCapture::CCamCapture() {
	m_bCamInited = FALSE;
	m_heventThreadDone = NULL;
	m_State = STOP;
	m_pCapture = NULL;
	m_bTargetObj = FALSE;
	m_pImage = NULL;
	m_bSavePause = FALSE;
	m_pfnCustomCB = NULL;
}

CCamCapture::~CCamCapture() {
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

BOOL CCamCapture::CreateCapture() {
	if (!m_bCamInited) {
		if (m_heventThreadDone) {
			CloseHandle(m_heventThreadDone);
			m_heventThreadDone = NULL;
		}

		m_heventThreadDone = CreateThread(NULL, 0, threadGrabImage, this, 0, NULL);
		if (m_heventThreadDone == NULL)
			return FALSE;
		Sleep(2000);
		int i = 0;
		while (i < 10000) {
			if (m_bCamInited)
				break;
			i++;
		}
		return m_bCamInited;
	}
	return TRUE;
}

void CCamCapture::DestroyCapture() {
	if (m_bCamInited) {
		m_State = STOP;
		if (::WaitForSingleObject(m_heventThreadDone, 1000) != WAIT_OBJECT_0) {
			SetEvent(m_heventThreadDone);
			if (::WaitForSingleObject(m_heventThreadDone, 1000) != WAIT_OBJECT_0)
				::MessageBox(NULL, L"Could not close grab routine thread!", L"Message", MB_OK | MB_SYSTEMMODAL);
		}
	}
}

void CCamCapture::StartCapture() {
	m_State = START;
}

void CCamCapture::PauseCapture() {
	m_State = PAUSE;
}

void CCamCapture::StopCapture() {
	m_State = STOP;
}

void CCamCapture::SaveCapture() {
	m_bSavePause = TRUE;
}

CaptureState CCamCapture::GetCaptureState() const {
	return m_State;
}

IplImage* CCamCapture::GetSelectedROI() const {
	return m_pROI;
}

CvRect CCamCapture::GetTargetRect() const {
	return m_TargetRect;
}

void CCamCapture::SetTargetRect(CvRect Rect)
{
	m_TargetRect = Rect;
}

void CCamCapture::SetCaptureCallback(pfnCaptureCallback pfnCaptureCB) {
	m_pfnCustomCB = pfnCaptureCB;
}

//////////////////////////////////////////////
/////                                    /////
/////     Protected Member Functions     /////
/////                                    /////
//////////////////////////////////////////////

DWORD WINAPI CCamCapture::threadGrabImage(LPVOID pparam) {
	CCamCapture* pCapture = ((CCamCapture*)pparam);
	pCapture->doGrabLoop();
	return 0;
}

void CCamCapture::doGrabLoop() {
	IplImage* frame = NULL;

	onGrabLoop_cvInit();

	while (m_bCamInited) {
		if (m_State == START) {
			frame = cvQueryFrame(m_pCapture);

			if (!frame)
				continue;

			if (!m_pImage) {
				// Allocate the image buffer
				m_pImage = cvCreateImage(cvGetSize(frame), 8, 3);
				m_pImage->origin = frame->origin;
			}

			cvCopy(frame, m_pImage, 0);

			// Drawing selected ROI
			onGrabLoop_DrawROI(frame);

			if (!m_bTargetObj && m_pfnCustomCB) {
				m_pfnCustomCB(m_pImage);
				cvShowImage(Camera_Window, m_pImage);
			}
			else
				cvShowImage(Camera_Window, frame);

			cvWaitKey(33);
		}
		else if (m_State == PAUSE) {
			if (m_pImage != NULL && frame != NULL)
				cvCopy(m_pImage, frame, 0);

			if (frame) {
				if (m_bSavePause) {
					cvSaveImage("SavePauseImg.bmp", frame);
					m_bSavePause = FALSE;
				}
				onGrabLoop_DrawROI(frame);
				cvShowImage(Camera_Window, frame);
				cvWaitKey(33);
			}
		}
		else if (m_State == STOP) {
			m_bCamInited = FALSE;

			if (m_pImage != NULL && m_pROI != NULL) {
				cvReleaseImage(&m_pImage);
				cvReleaseImage(&m_pROI);
			}

			cvWaitKey(5);

			onGrabLoop_cvClose();
			Sleep(1000);

			SetEvent(m_heventThreadDone);

			if (!m_bCamInited)
				break;
		}
	}
}

void CCamCapture::onGrabLoop_cvInit() {
	m_pCapture = cvCaptureFromCAM(0);

	if (m_pCapture) {
		m_bCamInited = TRUE;
		m_State = START;
		cvNamedWindow(Camera_Window);
		cvSetMouseCallback(Camera_Window, onMouseCB, NULL);
	}
	else
		m_bCamInited = FALSE;
}

void CCamCapture::onGrabLoop_cvClose() {
	if (m_pCapture != NULL || m_pImage != NULL || m_pROI != NULL) {
		cvReleaseImage(&m_pImage);
		cvReleaseImage(&m_pROI);
		cvReleaseCapture(&m_pCapture);
		cvDestroyWindow(Camera_Window);
		cvDestroyWindow(ROI_Window);
	}
}

////////////////////////////////////////////
/////                                  /////
/////     Private Member Functions     /////
/////                                  /////
////////////////////////////////////////////

void CCamCapture::onMouseCB(int event, int x, int y, int flags, void * param) {
	if (m_pImage->origin != 0)
		m_pImage->origin = 0;

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
			if (m_pROI) {
				cvDestroyWindow(ROI_Window);
				cvReleaseImage(&m_pROI);
				m_pROI = NULL;
			}
			IplImage* ROI = cvCreateImage(cvSize(m_pImage->width, m_pImage->height), 8, 3);
			cvCopy(m_pImage, ROI);
			cvSetImageROI(ROI, m_TargetRect);
			m_pROI = ROI;
			cvShowImage(ROI_Window, m_pROI);
		}
		break;
	default:
		break;
	}
}

void CCamCapture::onGrabLoop_DrawROI(IplImage* frame) {
	if (m_bTargetObj)
		if (m_TargetRect.width > 0 && m_TargetRect.height > 0) {
			cvSetImageROI(frame, m_TargetRect);
			cvXorS(frame, cvScalarAll(255), frame, 0);
			cvRectangle(frame, cvPoint(0, 0), cvPoint(m_TargetRect.width-1, m_TargetRect.height-1), cv::Scalar(0, 255, 0), 2);
			cvResetImageROI(frame);
		}
}
