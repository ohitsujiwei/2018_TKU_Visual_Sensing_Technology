
// MFC_VSTDlg.h: 標頭檔
//

#pragma once


// CMFCVSTDlg 對話方塊
class CMFCVSTDlg : public CDialogEx
{
// 建構
public:
	CMFCVSTDlg(CWnd* pParent = nullptr);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_VST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton m_Button_Capture;
	CButton m_Button_Pause;
	CButton m_Button_Save;
	CButton m_Button_File;
	CButton m_Button_Recognition;
	CComboBox m_Combo_Method;
	CStatic m_Text_Param1;
	CStatic m_Text_Param2;
	CStatic m_Text_Param3;
	CStatic m_Text_Value1;
	CStatic m_Text_Value2;
	CStatic m_Text_Value3;
	CSliderCtrl m_Slider_Param1;
	CSliderCtrl m_Slider_Param2;
	CSliderCtrl m_Slider_Param3;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonCapture();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonFile();
	afx_msg void OnBnClickedButtonRecognition();
	afx_msg void OnCbnSelchangeComboMethod();
	afx_msg void OnNMCustomdrawSliderParam1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderParam2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderParam3(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_Text_File;
};
