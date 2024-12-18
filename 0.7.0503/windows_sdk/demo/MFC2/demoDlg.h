
// demoDlg.h : header
//

#pragma once

#include "afxwin.h"
#include "afxcmn.h"
#include "ASICamera2.h"
#include "opencv/highgui.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "Ctrlpanel.h"
//#include <vld.h> //memory leak detector

// CdemoDlg Dialog
class CdemoDlg : public CDialog
{
// construct
public:
	CdemoDlg(CWnd* pParent = NULL);	// standard dialog

// dialog data
	enum { IDD = IDD_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


protected:
	HICON m_hIcon;

	// message map function
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox CComboCamera;
	int m_iWidth;
	int m_iHeight;
	int m_iStartX;
	int m_iStartY;
	int m_iROIWidth;
	int m_iROIHeight;
	int m_ctrlMin;
	int m_ctrlMax;
	int m_curPos;
	float m_fTemp;
	int m_ST4time;
	BOOL m_bFlipY;
	BOOL m_bFlipX;
	CString m_strCameraType;	
	CComboBox m_ComboImgType;
	CComboBox m_ComboBin;
	CComboBox m_comboCtrl;
	CButton m_butOpen;
	CButton m_butClose;
	CButton m_butStop;
	CButton m_butStart;
	CButton m_btNorth;
	CButton m_btEast;
	CButton m_btSouth;
	CButton m_btWest;
	CButton m_btAutoTest;
	CButton m_btnSetFormat;
	CButton m_btnSetPostion;
	CButton m_btGetOneImage;
	CSliderCtrl m_SliderCtrl;
	CButton m_ButAuto;

	afx_msg void OnBnClickedButtonScan();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonStartVideo();
	afx_msg void OnBnClickedButtonSetformat();
	afx_msg void OnBnClickedButtonstop();
	afx_msg void OnBnClickedButtonStartpos();
	afx_msg void OnBnClickedButtonNorth();
	afx_msg void OnBnClickedButtonSouth();
	afx_msg void OnBnClickedButtonWest();
	afx_msg void OnBnClickedButtonEast();
	afx_msg void OnBnClickedChecksubdark();
	afx_msg void OnClose();
	afx_msg void OnCbnSelchangeComboCameras();
	afx_msg void OnBnClickedButtonCloseall();
	afx_msg void OnBnClickedButtonStopall();
	afx_msg void OnMenuSaveimageas();
	afx_msg void OnMenuSetfilepath();
	afx_msg void OnMenuSave();
	afx_msg void OnBnClickedButtonGetlongimage();
	afx_msg void OnBnClickedButtonAbortsnap();


	enum CamStatus {
		closed = 0,
		opened,
		capturing,
		snaping
	};
	struct ConnectedCam{
		HANDLE Thr_Display, Thr_Snap, Thr_CapVideo;
		CamStatus Status;
		int iCtrlNum;
//		bool bAvail;
		ASI_CONTROL_CAPS* pControlCaps;
		ASI_CAMERA_INFO* pASICameraInfo;
		bool *pRefreshedIndex;
		IplImage *pRgb;
		IplImage *pTempImg;
		IplImage *pTempImgScaled;
		int iSnapTime;
		ASI_IMG_TYPE ImageType;
		int width, height;
		bool bSnap;
		float fOldScale;
		bool bNewImg;
		bool bSnapContinuous;
	} ;
	struct SAVE_IMAGE{
		BYTE* pBuf;
		ASI_IMG_TYPE Type;
		int Width, Height, CamID;
		CString FilePath;
		HWND cvHwnd;
	};

	int iSelectedID;
	int iCamNum;
	ConnectedCam ConnectCamera[ASICAMERA_ID_MAX];
	ASI_IMG_TYPE m_ImgTypeArray[64];
	int m_BinArray[64];
	CToolTipCtrl m_ToolTip;
	bool bThreadCall;
	ASI_ERROR_CODE err;
	SAVE_IMAGE SaveImage;
	CString m_static_msg;
	CString m_static_SnapTime;
	CButton m_btnAbortSnap;
	CButton m_btnStopAll;
	bool bCtrlDrawDone;
	CScrollBar m_scrollV_ctrl;
	int iCtrlYEnd, iCtrlYStart;	
	CRect RC_Ctrl;
	ASI_GUIDE_DIRECTION Dir;

	int* p_CtrlOriyStart;
	CCtrlPanel *pCtrlPanel;
	POINT pt0, pt1;
	bool bDrawRect;
	bool bLBDown;
	


	void SetAuto(int iControlIndex, bool bChecked);
	void EnableControl(int iControlIndex, bool bAuto, bool bWriteable);
	BOOL PreTranslateMessage(MSG* pMsg);
	void EditSetValue(int iControlIndex);
	LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
	void SaveBMP(CString FilePath);
	void SaveFilePathToReg(CString FilePath);
	CString LoadFilePathFromReg();
	CString GetFolderPath(CString Path);
	bool saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height, int biBitCount);
	void CloseCam(int iID);
	void StopCam(int iID);
	void AbortSnap(int iID);
	
	void ChangeButtonAppearence(CamStatus status);
	void RefreshInfoCtrl();
	void DrawControl();
	void ReleaseMem(int iID);
	void MallocMem(int iID);
	CButton m_btn_close_all;
	afx_msg void OnBnClickedButtonAutotest();
	afx_msg void OnBnClickedButtonReadId();
	CString m_edit_id_in_spi;
	afx_msg void OnBnClickedButtonWriteId();
	CButton m_btn_get_id;
	CButton m_btn_set_id;
	afx_msg void OnBnClickedButtonRoiSize();
	afx_msg void OnBnClickedButtonMaxSize();
	BOOL m_check_enableHPC;
	int ScaleArray[4];
	float fScale;
	int iNumofConnectCameras;
	CComboBox combobox_scale;
	afx_msg void OnCbnSelchangeComboScale();
	void MallocImg(int iID);
	void ReleaseImg(int iID);
	afx_msg void OnBnClickedCheck3();
	BOOL m_check_continuous;
};
