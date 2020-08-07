#pragma once
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

typedef enum {
	STOP,
	PAUSE,
	START,
} CaptureState;

typedef void(__cdecl *pfnCaptureCallback)(IplImage* pFrame);	// OpenCV 1.0

class CCamCapture {
public:
	// Member Function
	CCamCapture();
	~CCamCapture();
	BOOL CreateCapture();
	void DestroyCapture();
	void StartCapture();
	void PauseCapture();
	void StopCapture();
	void SaveCapture();
	CaptureState GetCaptureState() const;
	IplImage* GetSelectedROI() const;
	CvRect GetTargetRect() const;
	void SetTargetRect(CvRect Rect);
	void SetCaptureCallback(pfnCaptureCallback pfnCaptureCB);

protected:
	// Member Function
	static DWORD WINAPI threadGrabImage(LPVOID pparam);
	virtual void doGrabLoop();
	virtual void onGrabLoop_cvInit();
	virtual void onGrabLoop_cvClose();

	// Member Variable
	bool m_bCamInited;
	HANDLE m_heventThreadDone;
	CaptureState m_State;
	bool m_bSavePause;

	// Static Member Variable
	static BOOL m_bTargetObj;

private:
	// Member Function
	static void onMouseCB(int event, int x, int y, int flags, void* param);
	void onGrabLoop_DrawROI(IplImage* frame);

	// Static Member Variable
	static IplImage* m_pImage;
	static IplImage* m_pROI;
	static CvRect m_TargetRect;
	static CvPoint m_Origin;

	// Member Variable
	CvCapture* m_pCapture;
	pfnCaptureCallback m_pfnCustomCB;
};