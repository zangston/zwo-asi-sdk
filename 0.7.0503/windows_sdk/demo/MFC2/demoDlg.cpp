
// demoDlg.cpp :

#include "stdafx.h"
#include "demo.h"
#include "demoDlg.h"
#include <process.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  UPDATE_MESSAGE 2010 


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CdemoDlg 




CdemoDlg::CdemoDlg(CWnd* pParent /*=NULL*/)
: CDialog(CdemoDlg::IDD, pParent)
, m_iWidth(0)
, m_iHeight(0)
, m_iStartX(0)
, m_iStartY(0)
, m_iROIWidth(0)
, m_iROIHeight(0)
, m_ctrlMin(0)
, m_ctrlMax(0)
, m_ST4time(0)
, m_strCameraType(_T(""))
, m_static_msg(_T(""))
, m_static_SnapTime(_T(""))
, m_edit_id_in_spi(_T(""))
, m_check_enableHPC(FALSE)
, m_check_continuous(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	iSelectedID = -1;
	for(int i = 0; i < sizeof(ConnectCamera)/sizeof(ConnectedCam); i++)
	{
		ConnectCamera[i].Status = closed;
		ConnectCamera[i].pControlCaps = NULL;
		ConnectCamera[i].pASICameraInfo = NULL;
		ConnectCamera[i].pRefreshedIndex = NULL;
		ConnectCamera[i].bSnap = true;
		ConnectCamera[i].pRgb = NULL;
		ConnectCamera[i].pTempImg = NULL;
		ConnectCamera[i].pTempImgScaled = NULL;
		ConnectCamera[i].fOldScale = -1;
		ConnectCamera[i].bNewImg = false;
		ConnectCamera[i].bSnapContinuous = false;
	}

	m_ToolTip.Create(this);
	bThreadCall = false;
	iNumofConnectCameras = 0;
	bCtrlDrawDone = false;
	iCtrlYStart = iCtrlYEnd = 0;
	p_CtrlOriyStart = 0;
	bDrawRect = false;
	bLBDown = false;
}

void CdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_CAMERAS, CComboCamera);
	DDX_Text(pDX, IDC_STATIC_WIDTH, m_iWidth);
	DDX_Text(pDX, IDC_STATIC_HEIGHT, m_iHeight);
	DDX_Text(pDX, IDC_EDIT_STARTX, m_iStartX);
	DDX_Text(pDX, IDC_EDIT_STARTY, m_iStartY);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_iROIWidth);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_iROIHeight);
	DDX_Control(pDX, IDC_COMBO_IMGTYPE, m_ComboImgType);
	DDX_Control(pDX, IDC_COMBO_BIN, m_ComboBin);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_butClose);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_butOpen);
	DDX_Control(pDX, IDC_BUTTON_stop, m_butStop);
	DDX_Control(pDX, IDC_BUTTON_Start, m_butStart);
	DDX_Control(pDX, IDC_BUTTON_SETFORMAT, m_btnSetFormat);
	DDX_Control(pDX, IDC_BUTTON_STARTPOS, m_btnSetPostion);
	DDX_Text(pDX, IDC_EDIT_DELAY, m_ST4time);
	DDX_Text(pDX, IDC_STATIC_CAMERATYPE, m_strCameraType);
	DDX_Control(pDX, IDC_BUTTON_NORTH, m_btNorth);
	DDX_Control(pDX, IDC_BUTTON_EAST, m_btEast);
	DDX_Control(pDX, IDC_BUTTON_SOUTH, m_btSouth);
	DDX_Control(pDX, IDC_BUTTON_West, m_btWest);
	DDX_Control(pDX, IDC_BUTTON_AUTOTEST, m_btAutoTest);
	DDX_Control(pDX, IDC_BUTTON_GETLONGIMAGE, m_btGetOneImage);
	DDX_Text(pDX, IDC_STATIC_FPS, m_static_msg);
	DDX_Text(pDX, IDC_STATIC_SNAPTIME, m_static_SnapTime);
	DDX_Control(pDX, IDC_BUTTON_ABORTSNAP, m_btnAbortSnap);
	DDX_Control(pDX, IDC_BUTTON_STOPALL, m_btnStopAll);
	DDX_Control(pDX, IDC_BUTTON_CLOSEALL, m_btn_close_all);
	DDX_Text(pDX, IDC_EDIT_ID_IN_SPI, m_edit_id_in_spi);
	DDX_Control(pDX, IDC_BUTTON_READ_ID, m_btn_get_id);
	DDX_Control(pDX, IDC_BUTTON_WRITE_ID, m_btn_set_id);
	DDX_Check(pDX, IDC_CHECK_LOADHPC, m_check_enableHPC);
	DDX_Control(pDX, IDC_COMBO_SCALE, combobox_scale);
	DDX_Check(pDX, IDC_CHECK3, m_check_continuous);
}

BEGIN_MESSAGE_MAP(CdemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SCAN, &CdemoDlg::OnBnClickedButtonScan)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CdemoDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CdemoDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_Start, &CdemoDlg::OnBnClickedButtonStartVideo)
	ON_BN_CLICKED(IDC_BUTTON_SETFORMAT, &CdemoDlg::OnBnClickedButtonSetformat)
	ON_BN_CLICKED(IDC_BUTTON_stop, &CdemoDlg::OnBnClickedButtonstop)
	ON_BN_CLICKED(IDC_BUTTON_STARTPOS, &CdemoDlg::OnBnClickedButtonStartpos)
	ON_BN_CLICKED(IDC_BUTTON_NORTH, &CdemoDlg::OnBnClickedButtonNorth)
	ON_BN_CLICKED(IDC_BUTTON_SOUTH, &CdemoDlg::OnBnClickedButtonSouth)
	ON_BN_CLICKED(IDC_BUTTON_West, &CdemoDlg::OnBnClickedButtonWest)
	ON_BN_CLICKED(IDC_BUTTON_EAST, &CdemoDlg::OnBnClickedButtonEast)
	ON_BN_CLICKED(IDC_CHECK_SUBDARK, &CdemoDlg::OnBnClickedChecksubdark)

	ON_MESSAGE(WM_MY_MSG, OnUpdateData)

	ON_WM_CLOSE()

	ON_CBN_SELCHANGE(IDC_COMBO_CAMERAS, &CdemoDlg::OnCbnSelchangeComboCameras)
	ON_WM_CREATE()
	ON_WM_TIMER()

	ON_BN_CLICKED(IDC_BUTTON_CLOSEALL, &CdemoDlg::OnBnClickedButtonCloseall)
	ON_BN_CLICKED(IDC_BUTTON_STOPALL, &CdemoDlg::OnBnClickedButtonStopall)

	ON_WM_DEVICECHANGE()

	ON_BN_CLICKED(IDC_BUTTON_GETLONGIMAGE, &CdemoDlg::OnBnClickedButtonGetlongimage)

	ON_COMMAND(ID_MENU_SAVEIMAGEAS, &CdemoDlg::OnMenuSaveimageas)
	ON_COMMAND(ID_MENU_SETFILEPATH, &CdemoDlg::OnMenuSetfilepath)
	ON_COMMAND(ID_MENU_SAVE, &CdemoDlg::OnMenuSave)
	ON_BN_CLICKED(IDC_BUTTON_ABORTSNAP, &CdemoDlg::OnBnClickedButtonAbortsnap)
	ON_BN_CLICKED(IDC_BUTTON_GETLONGIMAGE, &CdemoDlg::OnBnClickedButtonGetlongimage)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_AUTOTEST, &CdemoDlg::OnBnClickedButtonAutotest)
	ON_BN_CLICKED(IDC_BUTTON_READ_ID, &CdemoDlg::OnBnClickedButtonReadId)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_ID, &CdemoDlg::OnBnClickedButtonWriteId)
	ON_BN_CLICKED(IDC_BUTTON_ROI_SIZE, &CdemoDlg::OnBnClickedButtonRoiSize)
	ON_BN_CLICKED(IDC_BUTTON_MAX_SIZE, &CdemoDlg::OnBnClickedButtonMaxSize)
//	ON_BN_CLICKED(IDC_CHECK_LOADHPC, &CdemoDlg::OnBnClickedCheckLoadhpc)
ON_CBN_SELCHANGE(IDC_COMBO_SCALE, &CdemoDlg::OnCbnSelchangeComboScale)
ON_BN_CLICKED(IDC_CHECK3, &CdemoDlg::OnBnClickedCheck3)
END_MESSAGE_MAP()




BOOL CdemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}


	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		


	CRect CtrlRc;
	GetDlgItem(IDC_STATIC_CTRLPANEL)->GetWindowRect(&CtrlRc);
	ScreenToClient(&CtrlRc);
	pCtrlPanel = new CCtrlPanel;
	pCtrlPanel->Create("",WS_BORDER|WS_CHILD|WS_VISIBLE,CtrlRc, this, 4270);
	OnBnClickedButtonScan();

	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_GETLONGIMAGE),"snap one image");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_Start),"start video capture");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_stop),"stop video capture or snap");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_STOPALL),"stop video capture or snap of all cameras");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_CLOSE),"close selected camera");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_CLOSEALL),"close all cameras");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_OPEN),"open selected cameras");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_SETFORMAT),"set the width && height && image type && bin of image");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_SCAN),"before scanning the connected cameras, click \"Close All\"");

	SaveImage.pBuf = NULL;
	SaveImage.FilePath = LoadFilePathFromReg();
	bool bSet = false;
//	ASIChangeHPCStatus(iCamIndex, &bSet, ASI_FALSE);
	ScaleArray[0] = 100;
	ScaleArray[1] = 75;
	ScaleArray[2] = 50;
	ScaleArray[3] = 25;

	CString temp;
	for(int i = 0; i < sizeof(ScaleArray)/sizeof(int); i++)
	{
		temp.Format("%d%%", ScaleArray[i]);
		combobox_scale.AddString(temp);
	}
	combobox_scale.SetCurSel(0);
	OnCbnSelchangeComboScale();

	CString title, tmp;
	GetWindowText(title);

	char* pszTitle = ASIGetSDKVersion();
	tmp.Format(" (SDK version %s)", pszTitle);
	title += tmp;

	SetWindowText(title);

	return TRUE;  
}

void CdemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}



void CdemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);


		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;


		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


HCURSOR CdemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CdemoDlg::OnBnClickedButtonScan()
{
	iCamNum = ASIGetNumOfConnectedCameras();

	CComboCamera.ResetContent();
	
	int iSelectedIndex = -1, i;

	ASI_CAMERA_INFO CamInfoTemp;
	bool bDevOpened[ASICAMERA_ID_MAX] = {false};

	char buf[64];
	for(i = 0; i < iCamNum; i++)
	{
		ASIGetCameraProperty(&CamInfoTemp, i);
		
		if(iSelectedID == CamInfoTemp.CameraID)
			iSelectedIndex = i;
		if(ASIGetNumOfControls(CamInfoTemp.CameraID, &ConnectCamera[CamInfoTemp.CameraID].iCtrlNum) != ASI_ERROR_CAMERA_CLOSED)//the camera maybe have be closed, say be unplugged and plugged
			bDevOpened[CamInfoTemp.CameraID] = true;
		if(bDevOpened[CamInfoTemp.CameraID])
			sprintf(buf, "%s(ID %d opened)", CamInfoTemp.Name, CamInfoTemp.CameraID);
		else
			sprintf(buf, "%s(ID %d closed)", CamInfoTemp.Name, CamInfoTemp.CameraID);
		CComboCamera.AddString(buf);

	}
	for(i = 0; i < ASICAMERA_ID_MAX; i++)
	{
		if(!bDevOpened[i])
			CloseCam(i);
	}
	if(iSelectedIndex == -1)//
	{
		if(iCamNum > 0)
			iSelectedIndex = 0;
		else
			iSelectedIndex = -1;
		iSelectedID = -1;
		
	}

	CComboCamera.SetCurSel(iSelectedIndex);
	OnCbnSelchangeComboCameras();
}


void CdemoDlg::OnBnClickedButtonOpen()
{
	int iCamIndex = CComboCamera.GetCurSel();
	if(iCamIndex == -1)
		return;

	if(iCamIndex >= ASICAMERA_ID_MAX)
	{
		CString cs;
		cs.Format("Can't open cameras more than %d", sizeof(ConnectCamera)/sizeof(ConnectedCam));
		MessageBox(cs);
		return;
	}
	int i = 0;

//	ASIGetCameraID(iCamIndex, &iSelectedID);
	ASI_CAMERA_INFO CamInfoTemp;
	ASIGetCameraProperty(&CamInfoTemp, iCamIndex);
	iSelectedID = CamInfoTemp.CameraID;

	if(ASIOpenCamera(iSelectedID)== ASI_SUCCESS)
	{
		ASIInitCamera(iSelectedID);
		ConnectCamera[iSelectedID].Status = opened;
		ConnectCamera[iSelectedID].iSnapTime = 0;


		ASIGetNumOfControls(iSelectedID, &ConnectCamera[iSelectedID].iCtrlNum);

		ReleaseMem(iSelectedID);
		MallocMem(iSelectedID);	

	}
	else
		MessageBox("reopen or open fail");	

	OnBnClickedButtonScan();

}
void CdemoDlg::MallocMem(int iID)
{
	int i;
	if(ConnectCamera[iID].pControlCaps == NULL)
	{
		ConnectCamera[iID].pControlCaps = new ASI_CONTROL_CAPS[ConnectCamera[iID].iCtrlNum];
		for(i = 0; i<ConnectCamera[iID].iCtrlNum; i++) 
			ASIGetControlCaps(iSelectedID, i, &ConnectCamera[iID].pControlCaps[i]);
	}
	if(ConnectCamera[iID].pRefreshedIndex == NULL)
		ConnectCamera[iID].pRefreshedIndex = new bool[ConnectCamera[iID].iCtrlNum];
	if(ConnectCamera[iID].pASICameraInfo == NULL)
		ConnectCamera[iID].pASICameraInfo = new ASI_CAMERA_INFO;
}
void CdemoDlg::ReleaseMem(int iID)
{
	if(iID < 0)
		return ;
	if(ConnectCamera[iID].pControlCaps)
	{		
		delete[] ConnectCamera[iID].pControlCaps;
		ConnectCamera[iID].pControlCaps = NULL;
	}
	if(ConnectCamera[iID].pRefreshedIndex)
	{
		delete ConnectCamera[iID].pRefreshedIndex;
		ConnectCamera[iID].pRefreshedIndex = NULL;
	}
	if(ConnectCamera[iID].pASICameraInfo)
	{
		delete ConnectCamera[iID].pASICameraInfo;
		ConnectCamera[iID].pASICameraInfo = NULL;
	}
}
inline void OutputDbgPrint(const char* strOutPutString, ...)
{
#ifdef _DEBUG
	char strBuf[128] = {0};
	sprintf_s(strBuf, "<%s> ", "demo2");
	va_list vlArgs;
	va_start(vlArgs, strOutPutString);
	_vsnprintf((char*)(strBuf+strlen(strBuf)), sizeof(strBuf)-strlen(strBuf), strOutPutString, vlArgs);
	va_end(vlArgs);

	OutputDebugStringA(strBuf);
#endif
}

void CdemoDlg::CloseCam(int iID)
{
	
	if(iID < 0)
		return;

	if(ConnectCamera[iID].Status != closed)
	{
		StopCam(iID);
		ASICloseCamera(iID);
		ConnectCamera[iID].Status = closed;
		ReleaseImg(iID);
	}	
	ReleaseMem(iID);

}
void CdemoDlg::OnBnClickedButtonClose()
{
	m_butClose.EnableWindow(FALSE);
	CloseCam(iSelectedID);
	OnBnClickedButtonScan();
}

void cvText(IplImage* img, const char* text, int x, int y)
{
	CvFont font;

	double hscale = 0.6;
	double vscale = 0.6;
	int linewidth = 2;
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC,hscale,vscale,0,linewidth);

	CvScalar textColor =cvScalar(0,255,255);
	CvPoint textPos =cvPoint(x, y);

	cvPutText(img, text, textPos, &font,textColor);
}


void CdemoDlg::OnCbnSelchangeComboCameras()
{
 	int iCamIndex = CComboCamera.GetCurSel();	

//	ASIGetCameraID(iCamIndex, &iSelectedID);
	ASI_CAMERA_INFO CamInfoTemp;	

	if(ASIGetCameraProperty(&CamInfoTemp, iCamIndex) == ASI_SUCCESS && CamInfoTemp.CameraID > -1 && CamInfoTemp.CameraID < ASICAMERA_ID_MAX)
	{
		iSelectedID = CamInfoTemp.CameraID;
		if(!ConnectCamera[iSelectedID].pASICameraInfo)
			ConnectCamera[iSelectedID].pASICameraInfo = new ASI_CAMERA_INFO;
		memcpy(ConnectCamera[iSelectedID].pASICameraInfo, &CamInfoTemp, sizeof(ASI_CAMERA_INFO));
		ChangeButtonAppearence(ConnectCamera[iSelectedID].Status);
	}
	else
		ChangeButtonAppearence(closed);


//	ASIGetCameraProperty(iSelectedID, ConnectCamera[iSelectedID].pASICameraInfo);

	RefreshInfoCtrl();
	DrawControl();

}
struct Dlg_Thread
{
	CdemoDlg* pDlg;
	int ID;
	HWND cvHandle;

};
void onMouse(int Event,int x,int y,int flags,void* param)
{
	Dlg_Thread* pDlgThread =(Dlg_Thread*)param;
	CdemoDlg *dlg;
	dlg = pDlgThread->pDlg;
	switch (Event)
	{
	case CV_EVENT_LBUTTONDOWN:
		if(dlg->ConnectCamera[pDlgThread->ID].Status != dlg->closed && pDlgThread->ID != dlg->iSelectedID)
		{
			dlg->bDrawRect = false;
			dlg->iSelectedID = pDlgThread->ID;
			dlg->bThreadCall = true;//call UpdateData() in thread  will cause error
			if(dlg->ConnectCamera[dlg->iSelectedID].Status == dlg->capturing)
				dlg->ChangeButtonAppearence(dlg->capturing);
			else if(dlg->ConnectCamera[dlg->iSelectedID].Status == dlg->snaping)
				dlg->ChangeButtonAppearence(dlg->snaping);

			dlg->RefreshInfoCtrl();
			SendMessage(dlg->m_hWnd, WM_MY_MSG, 3210, FALSE);//call DrawControl()
			dlg->bThreadCall = false;	
			ASI_CAMERA_INFO CamInfoTemp;
			for(int i = 0; i < dlg->iCamNum; i++)
			{
//				ASIGetCameraID(i, &id);
				ASIGetCameraProperty(&CamInfoTemp, i);				
				if(CamInfoTemp.CameraID == dlg->iSelectedID)
				{
					dlg->CComboCamera.SetCurSel(i);
					break;
				}
			}
			SendMessage(dlg->m_hWnd, WM_MY_MSG, WM_UPDATEUISTATE, FALSE);
		}
		else
		{
			if(dlg->bDrawRect)
				dlg->bDrawRect = false;
			dlg->bLBDown = true;
			dlg->pt0.x = x;
			dlg->pt0.y = y;
		}
		break;

	case CV_EVENT_MOUSEMOVE:
		if(dlg->bLBDown)
		{
			dlg->pt1.x = x;
			dlg->pt1.y = y;
			dlg->bDrawRect = true;
		}
		
		break;

	case CV_EVENT_LBUTTONUP:
	
		dlg->bLBDown = false;
		break;

	case CV_EVENT_RBUTTONDOWN:
		dlg->SaveImage.CamID = pDlgThread->ID;
		dlg->SaveImage.cvHwnd = pDlgThread->cvHandle;//used to locate messagebox window
		PostMessage(dlg->m_hWnd, WM_MY_MSG, WM_RBUTTONDOWN, FALSE);//not wait message process function return 
		break;
	}


}
void CaptureVideo(LPVOID params)
{
	CdemoDlg* dlg =(CdemoDlg*)params; 
	int iCamIDInThread = dlg->iSelectedID;
	int time1,time2;
	time1 = GetTickCount();
	int iDropFrame;
	long lBuffSize;
	char buf[128]={0};
	int count=0;

	lBuffSize = dlg->ConnectCamera[iCamIDInThread].width*dlg->ConnectCamera[iCamIDInThread].height*
		dlg->ConnectCamera[iCamIDInThread].pRgb->depth*dlg->ConnectCamera[iCamIDInThread].pRgb->nChannels/8;

	long expMs;
	ASI_BOOL bAuto = ASI_FALSE;
	while(dlg->ConnectCamera[iCamIDInThread].Status == dlg->capturing)
	{
		time2 = GetTickCount();
		if(time2-time1 > 1000 )
		{
			ASIGetDroppedFrames(iCamIDInThread, &iDropFrame);
			sprintf_s(buf, "fps:%d dropped frames:%d",count, iDropFrame);
			count = 0;
			time1=GetTickCount();
		}
		ASIGetControlValue(iCamIDInThread, ASI_EXPOSURE, &expMs, &bAuto);
		expMs /= 1000;
		if(ASIGetVideoData(iCamIDInThread, (BYTE*)(dlg->ConnectCamera[iCamIDInThread].pTempImg->imageData), lBuffSize, expMs*2 + 500) == ASI_SUCCESS)
		{
			dlg->ConnectCamera[iCamIDInThread].bNewImg = true;
			count++;
		}

		if(iCamIDInThread == dlg->iSelectedID&&dlg->ConnectCamera[iCamIDInThread].Status == dlg->capturing)
		{
			dlg->m_static_msg = buf;
			SendMessage(dlg->m_hWnd, WM_MY_MSG, WM_UPDATEUISTATE, FALSE);
		}
	}
	dlg->m_static_msg = "capture stop";
	SendMessage(dlg->m_hWnd, WM_MY_MSG, WM_UPDATEUISTATE, FALSE);

	ASIStopVideoCapture(iCamIDInThread);

	_endthread();


}
void Display(LPVOID params)
{
	CdemoDlg* dlg =(CdemoDlg*)params;
	int iCamIDInThread;
	iCamIDInThread = dlg->iSelectedID;

	char buf[128]={0};

	char name[64], temp[64];
	if(dlg->ConnectCamera[iCamIDInThread].Status == dlg->snaping)
		sprintf(name, "snap");
	else if(dlg->ConnectCamera[iCamIDInThread].Status == dlg->capturing)
		sprintf(name, "video");
	strcat(name, dlg->ConnectCamera[iCamIDInThread].pASICameraInfo->Name);
	sprintf(temp, " ID%d", iCamIDInThread);
	strcat(name, temp);

	cvNamedWindow(name, 1);


	Dlg_Thread dlgindex;
	dlgindex.pDlg = dlg;
	dlgindex.ID = iCamIDInThread;
	cvSetMouseCallback(name,onMouse,(void*)&dlgindex);

	//	RECT rectCenter;
	int x, y, centerX, centerY;
	centerX = dlg->ConnectCamera[iCamIDInThread].width/2;
	centerY = dlg->ConnectCamera[iCamIDInThread].height/2;
	CvPoint cvPt0, cvPt1;
	while(dlg->ConnectCamera[iCamIDInThread].Status == dlg->snaping || dlg->ConnectCamera[iCamIDInThread].Status == dlg->capturing)
	{				

		if(dlg->ConnectCamera[iCamIDInThread].bNewImg)
		{


			if(iCamIDInThread == dlg->iSelectedID && dlg->bDrawRect)
			{
				cvPt0 = cvPoint(dlg->pt0.x, dlg->pt0.y);
				cvPt1 = cvPoint(dlg->pt1.x, dlg->pt0.y);
				cvLine(dlg->ConnectCamera[iCamIDInThread].pTempImg, cvPt0, cvPt1, CV_RGB(255, 255, 0), 1);
				cvPt0 = cvPoint(dlg->pt1.x, dlg->pt0.y);
				cvPt1 = cvPoint(dlg->pt1.x, dlg->pt1.y);
				cvLine(dlg->ConnectCamera[iCamIDInThread].pTempImg, cvPt0, cvPt1, CV_RGB(255, 255, 0), 1);
				cvPt0 = cvPoint(dlg->pt1.x, dlg->pt1.y);
				cvPt1 = cvPoint(dlg->pt0.x, dlg->pt1.y);
				cvLine(dlg->ConnectCamera[iCamIDInThread].pTempImg, cvPt0, cvPt1, CV_RGB(255, 255, 0), 1);
				cvPt0 = cvPoint(dlg->pt0.x, dlg->pt0.y);
				cvPt1 = cvPoint(dlg->pt0.x, dlg->pt1.y);
				cvLine(dlg->ConnectCamera[iCamIDInThread].pTempImg, cvPt0, cvPt1, CV_RGB(255, 255, 0), 1);
			}
		}

		if(dlg->fScale < 1)
		{	

			if(!dlg->ConnectCamera[iCamIDInThread].pTempImgScaled 
				|| dlg->ConnectCamera[iCamIDInThread].fOldScale != dlg->fScale
				|| dlg->ConnectCamera[iCamIDInThread].bNewImg)
			{
				dlg->ConnectCamera[iCamIDInThread].bNewImg = false;
				int WidthScaled, HeightScaled;
				WidthScaled = dlg->ConnectCamera[iCamIDInThread].width*dlg->fScale;
				HeightScaled = dlg->ConnectCamera[iCamIDInThread].height*dlg->fScale;

				dlg->ConnectCamera[iCamIDInThread].fOldScale = dlg->fScale;
				cvReleaseImage(&dlg->ConnectCamera[iCamIDInThread].pTempImgScaled);
				dlg->ConnectCamera[iCamIDInThread].pTempImgScaled = cvCreateImage(cvSize(WidthScaled, HeightScaled), dlg->ConnectCamera[iCamIDInThread].pTempImg->depth, 
					dlg->ConnectCamera[iCamIDInThread].pTempImg->nChannels);

				cvResize(dlg->ConnectCamera[iCamIDInThread].pTempImg, dlg->ConnectCamera[iCamIDInThread].pTempImgScaled, CV_INTER_AREA);
				cvSetImageROI(dlg->ConnectCamera[iCamIDInThread].pRgb , cvRect(0, 0, WidthScaled, HeightScaled));
				cvCopy(dlg->ConnectCamera[iCamIDInThread].pTempImgScaled ,dlg->ConnectCamera[iCamIDInThread].pRgb);

				cvShowImage(name, dlg->ConnectCamera[iCamIDInThread].pRgb);
			}


		}
		else
		{

			if(dlg->ConnectCamera[iCamIDInThread].bNewImg || dlg->ConnectCamera[iCamIDInThread].fOldScale != dlg->fScale)
			{
				dlg->ConnectCamera[iCamIDInThread].fOldScale = dlg->fScale;
				cvResetImageROI(dlg->ConnectCamera[iCamIDInThread].pRgb);
				memcpy((BYTE*)(dlg->ConnectCamera[iCamIDInThread].pRgb->imageData), (BYTE*)(dlg->ConnectCamera[iCamIDInThread].pTempImg->imageData), 
					dlg->ConnectCamera[iCamIDInThread].pTempImg->width*dlg->ConnectCamera[iCamIDInThread].pTempImg->height*dlg->ConnectCamera[iCamIDInThread].pTempImg->depth*dlg->ConnectCamera[iCamIDInThread].pTempImg->nChannels/8);
				dlg->ConnectCamera[iCamIDInThread].bNewImg = false;

				cvShowImage(name, dlg->ConnectCamera[iCamIDInThread].pRgb);
			}
		}

		//	cvText(dlg->ConnectCamera[iCamIDInThread].pRgb, buf, iTextX, iTextY);
		//	cvShowImage(WindowName, dlg->ConnectCamera[iCamIDInThread].pRgb);
		cvWaitKey(1);
	}	

	cvDestroyWindow(name);
	_endthread();

}
void CdemoDlg::ReleaseImg(int iID)
{
	if(ConnectCamera[iID].pRgb)
		cvReleaseImage(&ConnectCamera[iID].pRgb);
	if(ConnectCamera[iID].pTempImg)
		cvReleaseImage(&ConnectCamera[iID].pTempImg);
	if(ConnectCamera[iID].pTempImgScaled)
		cvReleaseImage(&ConnectCamera[iID].pTempImgScaled);
}
void CdemoDlg::MallocImg(int iID)
{
	int width, height, bin;
	long lBuffSize;
	ASI_IMG_TYPE image_type;
	ASIGetROIFormat(iID, &width, &height, &bin, &image_type);
	ConnectCamera[iID].ImageType = image_type;
	ConnectCamera[iID].width = width;
	ConnectCamera[iID].height = height;

	ReleaseImg(iID);
	switch(image_type)
	{
	case ASI_IMG_Y8:
	case ASI_IMG_RAW8:
		ConnectCamera[iID].pRgb = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 1);
		lBuffSize = width*height;
		break;

	case ASI_IMG_RGB24:
		ConnectCamera[iID].pRgb = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 3);
		lBuffSize = width*height*3;
		break;

	case ASI_IMG_RAW16:
		ConnectCamera[iID].pRgb = cvCreateImage(cvSize(width,height), IPL_DEPTH_16U, 1);
		lBuffSize = width*height*2;
		break;
	}
	ConnectCamera[iID].pTempImg = cvCreateImage(cvSize(width,height), ConnectCamera[iID].pRgb->depth, ConnectCamera[iID].pRgb->nChannels);
}

void CdemoDlg::OnBnClickedButtonStartVideo()
{

	UpdateData(TRUE);

	if(iSelectedID > -1 && iSelectedID < ASICAMERA_ID_MAX && ConnectCamera[iSelectedID].Status != closed)
	{	
		ASIStartVideoCapture(iSelectedID);

		ConnectCamera[iSelectedID].Status = capturing; 

	MallocImg(iSelectedID);
	ConnectCamera[iSelectedID].Thr_Display = (HANDLE)_beginthread(Display,NULL, this);
	ConnectCamera[iSelectedID].Thr_CapVideo = (HANDLE)_beginthread(CaptureVideo,NULL, this);
		ChangeButtonAppearence(capturing);
	}
	m_static_msg = "capture start";
	UpdateData(FALSE);
}


void CdemoDlg::OnBnClickedButtonSetformat()
{

	UpdateData(TRUE);
	int i = m_ComboImgType.GetCurSel();
	if(i==-1)
		return;
	if(iSelectedID > -1 && iSelectedID < ASICAMERA_ID_MAX && ConnectCamera[iSelectedID].Status != closed)
	{
		int startx, starty;
		ASIGetStartPos(iSelectedID, &startx, &starty);
		if(ASISetROIFormat(iSelectedID, m_iROIWidth, m_iROIHeight, m_BinArray[m_ComboBin.GetCurSel()], m_ImgTypeArray[i])!= ASI_SUCCESS)		
			MessageBox("Set format error\n");		
		else
			ASISetStartPos(iSelectedID, startx, starty);
	}
	UpdateData(false);
}  
void CdemoDlg::StopCam(int iID)
{
	if(iID < 0)
		return ;
	if(ConnectCamera[iID].Status == snaping)
	{
		AbortSnap(iID);
	}
	if(ConnectCamera[iID].Status == capturing)
	{
		ConnectCamera[iID].Status = opened;	
		WaitForSingleObject(ConnectCamera[iID].Thr_CapVideo, 1000);
	}
	if(ConnectCamera[iID].Status == capturing || ConnectCamera[iID].Status == snaping )
	{
		ConnectCamera[iID].Status = opened;	
		WaitForSingleObject(ConnectCamera[iID].Thr_Display, 1000);
		ReleaseImg(iID);
	}
	if(iSelectedID == iID)
		ChangeButtonAppearence(opened);
}
void CdemoDlg::OnBnClickedButtonstop()
{
	m_butStop.EnableWindow(FALSE);
	StopCam(iSelectedID);
}
void CdemoDlg::OnBnClickedButtonStartpos()
{
	UpdateData(TRUE);
	if(iSelectedID > -1 && iSelectedID < ASICAMERA_ID_MAX && ConnectCamera[iSelectedID].Status != closed)
	{
		ASISetStartPos(iSelectedID, m_iStartX, m_iStartY);
		ASIGetStartPos(iSelectedID, &m_iStartX, &m_iStartY);
	}
	UpdateData(false);
}
void ST4Test(LPVOID params)
{
	CdemoDlg* dlg =(CdemoDlg*)params; 
	dlg->m_btAutoTest.EnableWindow(false);

	dlg->m_btNorth.EnableWindow(false);

	dlg->m_btSouth.EnableWindow(false);

	dlg->m_btWest.EnableWindow(false);

	dlg->m_btEast.EnableWindow(false);

	ASIPulseGuideOn(dlg->iSelectedID, dlg->Dir);
	Sleep(dlg->m_ST4time);
	ASIPulseGuideOff(dlg->iSelectedID,dlg->Dir);
	dlg->m_btAutoTest.EnableWindow(true);

	dlg->m_btNorth.EnableWindow(true);

	dlg->m_btSouth.EnableWindow(true);

	dlg->m_btWest.EnableWindow(true);

	dlg->m_btEast.EnableWindow(true);

	_endthread();
}

void CdemoDlg::OnBnClickedButtonNorth()
{
	UpdateData(TRUE);
	Dir = ASI_GUIDE_NORTH;
	_beginthread(ST4Test, NULL, this);
}

void CdemoDlg::OnBnClickedButtonSouth()
{

	UpdateData(TRUE);
	Dir = ASI_GUIDE_SOUTH;
	_beginthread(ST4Test, NULL, this);

}


void CdemoDlg::OnBnClickedButtonWest()
{

	UpdateData(TRUE);
	Dir = ASI_GUIDE_WEST;
	_beginthread(ST4Test, NULL, this);

}

void CdemoDlg::OnBnClickedButtonEast()
{
	UpdateData(TRUE);
	Dir = ASI_GUIDE_EAST;
	_beginthread(ST4Test, NULL, this);

}

void CdemoDlg::OnBnClickedChecksubdark()
{

	if ( BST_CHECKED == IsDlgButtonChecked( IDC_CHECK_SUBDARK ) )
	{		
		char *FilePath;
		CString temp;
		CFileDialog dlg(TRUE);
		if(dlg.DoModal()==IDOK) 
		{
			temp=dlg.GetPathName();
			FilePath = (LPSTR)(LPCTSTR)temp;		
			if(ASIEnableDarkSubtract(iSelectedID, FilePath) != ASI_SUCCESS)
			{
				AfxMessageBox("operate failed!");
				((CButton*)GetDlgItem(IDC_CHECK_SUBDARK))->SetCheck(false);
			}
		}
		else
			((CButton*)GetDlgItem(IDC_CHECK_SUBDARK))->SetCheck(false);
	}
	else
	{
		ASIDisableDarkSubtract(iSelectedID);
	}
}

void CdemoDlg::OnClose()
{

	for(int i = 0; i < sizeof(ConnectCamera)/sizeof(ConnectedCam); i++)
		CloseCam(i);

	if(!SaveImage.FilePath.IsEmpty())
		SaveFilePathToReg(SaveImage.FilePath);

	delete pCtrlPanel;

	CDialog::OnClose();
}

void CdemoDlg::ChangeButtonAppearence(CamStatus status)
{
	switch(status)
	{
	case closed:
		m_butOpen.EnableWindow(TRUE);
		m_butClose.EnableWindow(FALSE);

		m_butStart.EnableWindow(FALSE);
		m_butStop.EnableWindow(FALSE);

		m_btnSetFormat.EnableWindow(FALSE);
		m_btnSetPostion.EnableWindow(FALSE);

		m_btNorth.EnableWindow(FALSE);
		m_btWest.EnableWindow(FALSE);
		m_btEast.EnableWindow(FALSE);
		m_btSouth.EnableWindow(FALSE);
		m_btAutoTest.EnableWindow(FALSE);


		((CSliderCtrl*)GetDlgItem(IDC_BUTTON_CLOSEALL))->EnableWindow(false);
		((CSliderCtrl*)GetDlgItem(IDC_BUTTON_STOPALL))->EnableWindow(false);

		for(int i = 0; i <sizeof(ConnectCamera)/sizeof(ConnectedCam); i++)
		{
			if(ConnectCamera[i].Status == capturing || ConnectCamera[i].Status == snaping)
			{
				((CSliderCtrl*)GetDlgItem(IDC_BUTTON_STOPALL))->EnableWindow(true);
			}
			if(ConnectCamera[i].Status != closed)
			{
				((CSliderCtrl*)GetDlgItem(IDC_BUTTON_CLOSEALL))->EnableWindow(true);
				break;
			}

		}
		((CEdit*)GetDlgItem(IDC_STATIC_FPS))->SetWindowText(" ");
		m_btGetOneImage.EnableWindow(FALSE);
		m_btnAbortSnap.EnableWindow(FALSE); 
		m_static_SnapTime = "";
		m_btn_get_id.EnableWindow(FALSE);
		m_btn_set_id.EnableWindow(FALSE);

		break;
	case opened:
		m_butOpen.EnableWindow(FALSE);
		m_butClose.EnableWindow(TRUE);

		m_butStart.EnableWindow(TRUE);
		m_butStop.EnableWindow(FALSE);


		m_btnSetFormat.EnableWindow(TRUE);
		m_btnSetPostion.EnableWindow(TRUE);

		m_btNorth.EnableWindow(TRUE);
		m_btWest.EnableWindow(TRUE);
		m_btEast.EnableWindow(TRUE);
		m_btSouth.EnableWindow(TRUE);
		m_btAutoTest.EnableWindow(TRUE);
		m_btnSetFormat.EnableWindow(TRUE);

		m_btnAbortSnap.EnableWindow(FALSE); 
		m_static_SnapTime = "";

		((CSliderCtrl*)GetDlgItem(IDC_BUTTON_CLOSEALL))->EnableWindow(true);


		((CSliderCtrl*)GetDlgItem(IDC_BUTTON_STOPALL))->EnableWindow(false);
		for(int i = 0; i <sizeof(ConnectCamera)/sizeof(ConnectedCam); i++)
		{
			if(ConnectCamera[i].Status == capturing || ConnectCamera[i].Status == snaping)
			{
				((CSliderCtrl*)GetDlgItem(IDC_BUTTON_STOPALL))->EnableWindow(true);
				break;
			}
		}

		((CEdit*)GetDlgItem(IDC_STATIC_FPS))->SetWindowText(" ");
		m_btGetOneImage.EnableWindow(TRUE);
		m_btn_get_id.EnableWindow(TRUE);
		m_btn_set_id.EnableWindow(TRUE);
		break;

	case capturing:
		m_butOpen.EnableWindow(FALSE);
		m_butClose.EnableWindow(TRUE);

		m_btnSetFormat.EnableWindow(FALSE);
		m_btnSetPostion.EnableWindow(TRUE);

		m_btNorth.EnableWindow(TRUE);
		m_btWest.EnableWindow(TRUE);
		m_btEast.EnableWindow(TRUE);
		m_btSouth.EnableWindow(TRUE);
		m_btAutoTest.EnableWindow(TRUE);


		m_butStart.EnableWindow(FALSE);
		m_butStop.EnableWindow(TRUE);

		m_btnStopAll.EnableWindow(true);

		m_btGetOneImage.EnableWindow(FALSE);
		m_btnAbortSnap.EnableWindow(FALSE); 
//		m_btn_get_id.EnableWindow(FALSE);
//		m_btn_set_id.EnableWindow(FALSE);

		break;

	case snaping:

		m_btnStopAll.EnableWindow(true);

		m_btnSetFormat.EnableWindow(FALSE);
		m_btGetOneImage.EnableWindow(TRUE);
		m_butStart.EnableWindow(FALSE);
		m_static_msg = "";
		m_btnAbortSnap.EnableWindow(FALSE);
		m_butStop.EnableWindow(TRUE);
//		m_btn_get_id.EnableWindow(FALSE);
//		m_btn_set_id.EnableWindow(FALSE);

		break;
	}


	if(!bThreadCall)
		UpdateData(FALSE);

}
void CdemoDlg::RefreshInfoCtrl()
{

	m_ComboImgType.Clear();
	m_ComboImgType.ResetContent();
	m_ComboBin.Clear();
	m_ComboBin.ResetContent();

	if(iSelectedID > -1 && iSelectedID < ASICAMERA_ID_MAX)
	{


		m_iHeight = ConnectCamera[iSelectedID].pASICameraInfo->MaxHeight;
		m_iWidth = ConnectCamera[iSelectedID].pASICameraInfo->MaxWidth;
		int i = 0, count =0;
		while (ConnectCamera[iSelectedID].pASICameraInfo->SupportedVideoFormat[i] != ASI_IMG_END)
		{
			switch(ConnectCamera[iSelectedID].pASICameraInfo->SupportedVideoFormat[i])
			{
			case ASI_IMG_RAW8:
				m_ComboImgType.AddString("RAW8");
				m_ImgTypeArray[count++] = ASI_IMG_RAW8;
				break;
			case ASI_IMG_RGB24:
				m_ComboImgType.AddString("RGB24");
				m_ImgTypeArray[count++] = ASI_IMG_RGB24;
				break;
			case ASI_IMG_RAW16:
				m_ComboImgType.AddString("RAW16");
				m_ImgTypeArray[count++] = ASI_IMG_RAW16;
				break;
			case ASI_IMG_Y8:
				m_ComboImgType.AddString("Y8");
				m_ImgTypeArray[count++] = ASI_IMG_Y8;
				break;
			}
			i++;
		}
		m_ComboImgType.SetCurSel(0);

		i = 0;


		count = 0;
		CString cs;
		while (ConnectCamera[iSelectedID].pASICameraInfo->SupportedBins[i] != 0)
		{
			cs.Format("%d",ConnectCamera[iSelectedID].pASICameraInfo->SupportedBins[i]);
			m_ComboBin.AddString(cs);
			m_BinArray[i] = ConnectCamera[iSelectedID].pASICameraInfo->SupportedBins[i];
			i++;
		}
		m_ComboBin.SetCurSel(0);
		if(ConnectCamera[iSelectedID].pASICameraInfo->IsColorCam == ASI_TRUE)
			m_strCameraType = "Color Camera";
		else
			m_strCameraType = "Mono Camera";

		if(ConnectCamera[iSelectedID].pASICameraInfo->IsUSB3Camera == ASI_TRUE)
			m_strCameraType += " USB3Camera";
		else
			m_strCameraType += " USB2Camera";

		if(ConnectCamera[iSelectedID].pASICameraInfo->IsUSB3Host == ASI_TRUE)
			m_strCameraType += " USB3Host";
		else
			m_strCameraType += " USB2Host";
		cs.Format(" %dbit", ConnectCamera[iSelectedID].pASICameraInfo->BitDepth);
		m_strCameraType += cs;


		if(ConnectCamera[iSelectedID].Status != closed)
		{
			int width, height, bin;
			ASI_IMG_TYPE imgType;
			ASIGetROIFormat(iSelectedID, &width, &height, &bin, &imgType);

			m_iROIHeight = height;
			m_iROIWidth = width;
			m_ComboImgType.SetCurSel(0);
			m_ComboBin.SetCurSel(0);
			for(i = 0; i < m_ComboImgType.GetCount(); i++)
			{
				if(m_ImgTypeArray[i] == imgType)
					m_ComboImgType.SetCurSel(i);
			}
			for(i = 0; i < m_ComboBin.GetCount(); i++)
			{
				if(m_BinArray[i] == bin)
					m_ComboBin.SetCurSel(i);
			}

			int StartX, StartY;
			ASIGetStartPos(iSelectedID, &StartX, &StartY);
			m_iStartX = StartX;
			m_iStartY = StartY;





			char* pChar = "";
			if(ASIEnableDarkSubtract(iSelectedID, pChar) == ASI_SUCCESS)
				((CButton*)GetDlgItem(IDC_CHECK_SUBDARK))->SetCheck(TRUE);
			else
				((CButton*)GetDlgItem(IDC_CHECK_SUBDARK))->SetCheck(FALSE);

			m_check_continuous = ConnectCamera[iSelectedID].bSnapContinuous;

		}
	}
	else
	{
		m_iHeight = 0;
		m_iWidth = 0;
		m_iStartY = 0;
		m_iStartX = 0;
		m_iROIHeight = 0;
		m_iROIWidth = 0;

		//		m_ComboImgType.Clear();

		//		m_ComboImgType.ResetContent();

		//		m_ComboBin.Clear();
		//		m_ComboBin.ResetContent();
		((CButton*)GetDlgItem(IDC_CHECK_SUBDARK))->SetCheck(false);
		m_check_continuous = false;

		m_strCameraType = "";
	}	
	if(!bThreadCall)
		UpdateData(FALSE);

}

void CdemoDlg::DrawControl()
{
	if(iSelectedID > -1 && iSelectedID < ASICAMERA_ID_MAX && ConnectCamera[iSelectedID].Status != closed)
	{	
		bCtrlDrawDone = false;
		if(ConnectCamera[iSelectedID].Status == opened || ConnectCamera[iSelectedID].Status ==capturing  || ConnectCamera[iSelectedID].Status ==snaping)
			pCtrlPanel->GetCtrlsData(ConnectCamera[iSelectedID].iCtrlNum, ConnectCamera[iSelectedID].pControlCaps, ConnectCamera[iSelectedID].pRefreshedIndex, true, iSelectedID);
	}
	else
		pCtrlPanel->GetCtrlsData(0, 0, 0, false, 0);

	CRect CtrlRc;
	GetDlgItem(IDC_STATIC_CTRLPANEL)->GetWindowRect(&CtrlRc);
	ScreenToClient(&CtrlRc);
	InvalidateRect(CtrlRc);

	bCtrlDrawDone = true;

}


BOOL CdemoDlg::PreTranslateMessage(MSG* pMsg)
{
	if(iSelectedID > -1 && iSelectedID < ASICAMERA_ID_MAX && ConnectCamera[iSelectedID].Status != closed)
	{
		m_ToolTip.RelayEvent(pMsg);
	}
	return CDialog::PreTranslateMessage(pMsg);
}



void CdemoDlg::OnBnClickedButtonCloseall()
{
	m_btn_close_all.EnableWindow(FALSE);
	m_butClose.EnableWindow(FALSE);
	int i;
	for(i = 0; i < sizeof(ConnectCamera)/sizeof(ConnectedCam); i++)
			CloseCam(i);
	OnBnClickedButtonScan();
}

void CdemoDlg::OnBnClickedButtonStopall()
{
	m_butStop.EnableWindow(false);
	m_btnStopAll.EnableWindow(false);
	int i;
	for(i = 0; i < sizeof(ConnectCamera)/sizeof(ConnectedCam); i++)
		if(i != iSelectedID)
			StopCam(i);
	StopCam(iSelectedID);
	UpdateData(FALSE);
}

LRESULT CdemoDlg::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	CMenu *pPopMenu;
	CMenu menu;	
	POINT pt;
	CRect rc;

	switch (wParam)
	{
	case WM_RBUTTONDOWN:

		menu.LoadMenu(IDR_MENU_RBDOWN);
		pPopMenu = menu.GetSubMenu(0);
		GetCursorPos(&pt);
		pPopMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);

		break;
	case WM_UPDATEUISTATE:
		if(lParam)
		{
			GetDlgItem(lParam)->GetWindowRect(&rc);
			ScreenToClient(&rc);
			InvalidateRect(rc);
		}
		else
			UpdateData(false);
		break;
	case 3210:
		DrawControl();
		break;
	}			

	return 0;
}


void SnapThr(LPVOID params)
{
	CdemoDlg* dlg =(CdemoDlg*)params; 
	int iCamIndexInThr = dlg->iSelectedID;
	long exp = 0;
	ASI_BOOL bAuto;

	while(true)
	{
		ASIGetControlValue(iCamIndexInThr, ASI_EXPOSURE, &exp, &bAuto);

		exp/=1000;//ms
		if( exp>=2000 )
		{
			dlg->ConnectCamera[iCamIndexInThr].bSnap = false;//start when value is false 
		}
		else
			dlg->m_static_SnapTime = "";

		int time = GetTickCount(), deltaMs, OldDeltaMs = 0;
		ASIStartExposure(iCamIndexInThr, ASI_FALSE);//start exposure

		ASI_EXPOSURE_STATUS status;

		char szBuf[32];
		do
		{
			ASIGetExpStatus(iCamIndexInThr, &status);
			if(!dlg->ConnectCamera[iCamIndexInThr].bSnap)  
			{
				deltaMs = GetTickCount() - time;
				deltaMs/=100;
				if( deltaMs!= OldDeltaMs)
				{
					OldDeltaMs = deltaMs;
					sprintf(szBuf, "%.1fsec", (float)deltaMs/10);
					dlg->m_static_SnapTime = szBuf;
					SendMessage(dlg->m_hWnd, WM_MY_MSG, WM_UPDATEUISTATE, FALSE);

				}
			}
			Sleep(1);

		} while (status == ASI_EXP_WORKING);

		char buf[64];
		if(status == ASI_EXP_FAILED)
		{		
			MessageBox(dlg->m_hWnd,"Get Image failed!", "error", MB_OK);
		}
		else if (status == ASI_EXP_SUCCESS)
		{	
			dlg->ConnectCamera[iCamIndexInThr].iSnapTime = GetTickCount() - time;	
			sprintf_s(buf , "snap cost time: %d ms", dlg->ConnectCamera[iCamIndexInThr].iSnapTime);
			dlg->m_static_msg = buf;
			if(dlg->ConnectCamera[iCamIndexInThr].pTempImg)
				ASIGetDataAfterExp(iCamIndexInThr, (BYTE*)dlg->ConnectCamera[iCamIndexInThr].pTempImg->imageData, dlg->ConnectCamera[iCamIndexInThr].pTempImg->imageSize);

			/*if(dlg->ConnectCamera[dlg->iCamIndex].ImageType != ASI_IMG_RGB24 && dlg->ConnectCamera[dlg->iCamIndex].ImageType != ASI_IMG_RAW16)
			{
				int iStrLen = 0, iTextX = 40, iTextY = 60;
				iStrLen = strlen(buf);
				CvRect rect = cvRect(iTextX, iTextY - 15, iStrLen* 11, 20);
				cvSetImageROI(dlg->ConnectCamera[dlg->iCamIndex].pTempImg , rect);
				cvSet(dlg->ConnectCamera[dlg->iCamIndex].pTempImg, CV_RGB(180, 180, 180));
				cvResetImageROI(dlg->ConnectCamera[dlg->iCamIndex].pTempImg);
				cvText(dlg->ConnectCamera[dlg->iCamIndex].pTempImg, buf, iTextX, iTextY);
			}*/
			dlg->ConnectCamera[iCamIndexInThr].bNewImg = true;
			SendMessage(dlg->m_hWnd, WM_MY_MSG, WM_UPDATEUISTATE, FALSE);

		}
		if(!dlg->ConnectCamera[iCamIndexInThr].bSnapContinuous)
			break;
	}
	dlg->ConnectCamera[iCamIndexInThr].bSnap = true;//stop 

	if(dlg->iSelectedID == iCamIndexInThr)
	{
		dlg->m_btGetOneImage.EnableWindow(true);
		dlg->m_btnAbortSnap.EnableWindow(FALSE);
	}
	OutputDbgPrint("%d snap exit\n",iCamIndexInThr);
	_endthread();
}


void CdemoDlg::OnBnClickedButtonGetlongimage()
{


	if(ConnectCamera[iSelectedID].Status == opened)
	{
		MallocImg(iSelectedID);
		ConnectCamera[iSelectedID].Status = snaping;

		ConnectCamera[iSelectedID].Thr_Display = (HANDLE)_beginthread(Display, NULL, this);

		ChangeButtonAppearence(snaping);
	}

	m_btGetOneImage.EnableWindow(FALSE);
	m_btnAbortSnap.EnableWindow(TRUE);

	UpdateData(true);
	ConnectCamera[iSelectedID].bSnapContinuous = m_check_continuous;
	ConnectCamera[iSelectedID].Thr_Snap = (HANDLE)_beginthread(SnapThr, NULL, this);


}




void CdemoDlg::SaveBMP(CString FilePath)
{
	long lBuffSize;
	SaveImage.Width = ConnectCamera[SaveImage.CamID].width;
	SaveImage.Height = ConnectCamera[SaveImage.CamID].height;
	SaveImage.Type = ConnectCamera[SaveImage.CamID].ImageType;
	if(SaveImage.pBuf)
	{
		delete[] SaveImage.pBuf;
		SaveImage.pBuf = NULL;
	}
	switch (SaveImage.Type)
	{
	case ASI_IMG_Y8:
	case ASI_IMG_RAW8:

		lBuffSize = SaveImage.Width*SaveImage.Height;
		SaveImage.pBuf = new BYTE[lBuffSize];
		memcpy(SaveImage.pBuf, (BYTE*)ConnectCamera[SaveImage.CamID].pTempImg->imageData, lBuffSize);
		break;
	case ASI_IMG_RGB24:
		lBuffSize = SaveImage.Width*SaveImage.Height*3;
		SaveImage.pBuf = new BYTE[lBuffSize];
		memcpy(SaveImage.pBuf, (BYTE*)ConnectCamera[SaveImage.CamID].pTempImg->imageData, lBuffSize);
		break;
	case ASI_IMG_RAW16:
		lBuffSize = SaveImage.Width*SaveImage.Height;
		SaveImage.pBuf = new BYTE[lBuffSize];
		for(long i = 0; i < lBuffSize; i++ )
			SaveImage.pBuf[i] = ((BYTE*)ConnectCamera[SaveImage.CamID].pTempImg->imageData)[2*i + 1];
		break;

	}

	char* pszFileName;
	pszFileName = (LPSTR)(LPCTSTR)FilePath;
	switch(SaveImage.Type)
	{
	case ASI_IMG_Y8:
	case ASI_IMG_RAW8:
	case ASI_IMG_RAW16:
		saveBmp(pszFileName, SaveImage.pBuf, SaveImage.Width, SaveImage.Height, 8);
		break;
	case ASI_IMG_RGB24:
		saveBmp(pszFileName, SaveImage.pBuf, SaveImage.Width, SaveImage.Height, 24);
		break;
	}

	if(SaveImage.pBuf)
	{
		delete[] SaveImage.pBuf;
		SaveImage.pBuf = NULL;
	}
}
CString CdemoDlg::GetFolderPath(CString Path)
{
	CString cs;
	int TailPos;
	for(int i = Path.GetLength(); i >= 0; i--)
	{
		if(Path.GetAt(i) == '\\')
		{
			TailPos = i;
			cs = Path.Mid(0, TailPos + 1);
			break;						
		}
	}
	return cs;
}
void CdemoDlg::OnMenuSaveimageas()
{
	CFileDialog fDialog(false, "BMP", NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT , "Bitmap file(*.BMP)|*.BMP|");
	if(IDOK == fDialog.DoModal())
	{
		SaveBMP(fDialog.GetPathName());
		SaveImage.FilePath = GetFolderPath(fDialog.GetPathName());
	}

}

char *szDefPath;
int CALLBACK SHBrowseForFolderCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{

	switch(uMsg)
	{

	case BFFM_INITIALIZED:    
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szDefPath);   
		break;
	case BFFM_SELCHANGED:   
		{
			char curr[MAX_PATH];   
			SHGetPathFromIDList((LPCITEMIDLIST)lParam,curr);   
			::SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)&curr[0]);  
		}
		break;

	default:
		break;
	} 
	return 0;
}

void CdemoDlg::OnMenuSetfilepath()
{

	szDefPath = (LPSTR)(LPCTSTR)SaveImage.FilePath;
	TCHAR szPath[MAX_PATH];
	BROWSEINFO br;
	ITEMIDLIST* pItem; 
	br.hwndOwner = this->GetSafeHwnd();
	br.pidlRoot = 0;
	br.pszDisplayName = 0;
	br.lpszTitle = "choose a folder to save image";
	br.ulFlags = BIF_STATUSTEXT;
	br.lpfn = SHBrowseForFolderCallbackProc ;        
	br.iImage = 0;
	br.lParam = 0;   
	pItem = SHBrowseForFolder(&br);
	if(pItem != NULL)
	{
		if(SHGetPathFromIDList(pItem,szPath) == TRUE)
		{
			SaveImage.FilePath = szPath;
		}
	} 
}

void CdemoDlg::OnMenuSave()
{

	if(!SaveImage.FilePath.IsEmpty())
	{
		SYSTEMTIME st;
		CString strDate, strTime;
		GetLocalTime(&st);
		strDate.Format("%4d%02d%02d",st.wYear,st.wMonth,st.wDay);
		strTime.Format("%02d%02d%02d",st.wHour,st.wMinute,st.wSecond);
		if(SaveImage.FilePath != "" && SaveImage.FilePath.Right(1) != '\\')
			SaveImage.FilePath += '\\';
		SaveBMP(SaveImage.FilePath + strDate + strTime + ".BMP");
	}
	else
		::MessageBoxA(SaveImage.cvHwnd,"please set file path first", "error", MB_OK);
}

#define REG_NAME "SOFTWARE\\ASIDemo2"
void CdemoDlg::SaveFilePathToReg(CString FilePath)
{

	CString lpSubKey;
	lpSubKey = REG_NAME;
	LRESULT  lRet; 
	HKEY hkey;

	lRet = RegOpenKey(HKEY_CURRENT_USER, lpSubKey, &hkey);
	if(lRet != ERROR_SUCCESS)
		RegCreateKey(HKEY_CURRENT_USER, lpSubKey, &hkey);

	char *pChar =  (LPSTR)(LPCTSTR)FilePath;

	RegSetValueEx(hkey, "FILEPATH",  NULL, REG_BINARY, (PBYTE)pChar, FilePath.GetLength());
	RegCloseKey(hkey);

}

CString CdemoDlg::LoadFilePathFromReg()
{
	CString FilePath;
	LRESULT  lRet; 
	DWORD dwSize;
	HKEY hkey;
	DWORD       dwType=4; 
	char ToChar[MAX_PATH];
	ZeroMemory(ToChar, MAX_PATH);
	CString lpSubKey = REG_NAME;
	lRet = RegOpenKey(HKEY_CURRENT_USER, lpSubKey, &hkey);
	if(lRet == ERROR_SUCCESS)
	{
		dwSize = sizeof(ToChar);
		RegQueryValueEx(hkey, "FILEPATH",  NULL, &dwType, (PBYTE)ToChar, &dwSize);

		RegCloseKey(hkey);
	}
	FilePath.Format("%s", ToChar);
	return FilePath;

}

bool CdemoDlg::saveBmp(char *bmpName, unsigned char *imgBuf, 
					   int width, int height, 
					   int biBitCount)
{
	if(!imgBuf)
		return 0;
	int colorTablesize=0;
	if(biBitCount==8)
		colorTablesize=1024;
	int lineByte=(width * biBitCount/8+3)/4*4;
	FILE *fp=fopen(bmpName,"wb");
	if(fp==0) return 0;
	BITMAPFILEHEADER fileHead;
	fileHead.bfType = 0x4D42;
	fileHead.bfSize= sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
		+ colorTablesize + lineByte*height;
	fileHead.bfReserved1 = 0;
	fileHead.bfReserved2 = 0;
	fileHead.bfOffBits=54+colorTablesize;
	fwrite(&fileHead, sizeof(BITMAPFILEHEADER),1, fp);
	BITMAPINFOHEADER head; 
	head.biBitCount=biBitCount;
	head.biClrImportant=0;
	head.biClrUsed=0;
	head.biCompression=0;
	head.biHeight=height;
	head.biPlanes=1;
	head.biSize=40;
	head.biSizeImage=lineByte*height;
	head.biWidth=width;
	head.biXPelsPerMeter=0;
	head.biYPelsPerMeter=0;
	fwrite(&head, sizeof(BITMAPINFOHEADER),1, fp);
	if(biBitCount==8)
	{
		RGBQUAD pColorTable[256];
		for(int i=0;i<256;i++)
		{
			pColorTable[i].rgbBlue = pColorTable[i].rgbGreen = pColorTable[i].rgbRed = pColorTable[i].rgbReserved = (UCHAR)i;
		}
		fwrite(pColorTable, sizeof(RGBQUAD),256, fp);
	}
	fwrite(imgBuf, height*lineByte, 1, fp);
	fclose(fp);
	return 1;

}

void CdemoDlg::AbortSnap(int iID)
{	
	if(ConnectCamera[iID].Status == snaping)
	{
		if(iSelectedID == iID)
			m_btnAbortSnap.EnableWindow(FALSE);
		ASIStopExposure(iID);
		Sleep(50);
	}
}

void CdemoDlg::OnBnClickedButtonAbortsnap()
{
	ConnectCamera[iSelectedID].bSnapContinuous = false;
	UpdateData(false);
	AbortSnap(iSelectedID);
}

void AutoTest(LPVOID params)
{
	CdemoDlg* dlg =(CdemoDlg*)params; 
	dlg->m_btAutoTest.EnableWindow(false);
	dlg->m_btNorth.EnableWindow(false);

	dlg->m_btSouth.EnableWindow(false);

	dlg->m_btWest.EnableWindow(false);

	dlg->m_btEast.EnableWindow(false);

	ASIPulseGuideOn(dlg->iSelectedID,ASI_GUIDE_NORTH);
	Sleep(100);
	ASIPulseGuideOff(dlg->iSelectedID,ASI_GUIDE_NORTH);

	ASIPulseGuideOn(dlg->iSelectedID,ASI_GUIDE_WEST);
	Sleep(100);
	ASIPulseGuideOff(dlg->iSelectedID,ASI_GUIDE_WEST);

	ASIPulseGuideOn(dlg->iSelectedID,ASI_GUIDE_SOUTH);
	Sleep(100);
	ASIPulseGuideOff(dlg->iSelectedID,ASI_GUIDE_SOUTH);

	ASIPulseGuideOn(dlg->iSelectedID,ASI_GUIDE_EAST);
	Sleep(100);
	ASIPulseGuideOff(dlg->iSelectedID,ASI_GUIDE_EAST);
	dlg->m_btAutoTest.EnableWindow(true);
	dlg->m_btNorth.EnableWindow(true);

	dlg->m_btSouth.EnableWindow(true);

	dlg->m_btWest.EnableWindow(true);

	dlg->m_btEast.EnableWindow(true);

	_endthread();
}
void CdemoDlg::OnBnClickedButtonAutotest()
{
	// TODO: 在此添加控件通知处理程序代码
	_beginthread(AutoTest, NULL, this);
}

void CdemoDlg::OnBnClickedButtonReadId()
{
	// TODO: 在此添加控件通知处理程序代码

	ASI_ID ASI_id = {0};
	m_edit_id_in_spi.Empty();
	if(ASIGetID(iSelectedID, &ASI_id)== ASI_SUCCESS)
		m_static_msg = "get ID success";
	else
		m_static_msg = "get ID fail";

	for (int i = 0; i < sizeof(ASI_id.id)/sizeof(char); i++)
	{
		m_edit_id_in_spi += ASI_id.id[i];
	}
	UpdateData(false);
}

void CdemoDlg::OnBnClickedButtonWriteId()
{
	// TODO: 在此添加控件通知处理程序代码
	ASI_ID ASI_id;
	UpdateData(true);
	if(m_edit_id_in_spi.GetLength() > sizeof(ASI_id.id)/sizeof(char))
	{
		char buf[64];
		sprintf(buf, "id length must be short than %d\n", sizeof(ASI_id.id)/sizeof(char));
		AfxMessageBox(buf);
		return;
	}
	int i;
	for (i = 0; i < m_edit_id_in_spi.GetLength(); i++)
	{
		ASI_id.id[i] = m_edit_id_in_spi.GetAt(i);
	}
	if(i < sizeof(ASI_id.id)/sizeof(char))
		ASI_id.id[i] = 0;
	if(ASISetID(iSelectedID, ASI_id) == ASI_SUCCESS)
		m_static_msg = "set ID success";
	else
		m_static_msg = "set ID fail";
	UpdateData(false);
}

void CdemoDlg::OnBnClickedButtonRoiSize()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if(bDrawRect)
	{
		int startx, starty;
		ASIGetStartPos(iSelectedID, &startx, &starty);
		m_iROIWidth = abs(pt0.x - pt1.x)/4*4;
		m_iROIHeight = abs(pt0.y - pt1.y)/4*4;
		m_iStartX = startx + std::min(pt0.x, pt1.x)*m_BinArray[m_ComboBin.GetCurSel()];
		m_iStartY = starty + std::min(pt0.y, pt1.y)*m_BinArray[m_ComboBin.GetCurSel()];
	}
	else
	{
		m_iROIWidth = m_iROIWidth/2;
		m_iROIHeight = m_iROIHeight/2;
		m_iStartX = m_iROIWidth/4*m_BinArray[m_ComboBin.GetCurSel()];
		m_iStartY = m_iROIHeight/4*m_BinArray[m_ComboBin.GetCurSel()];
	}
	UpdateData(false);
	
}

void CdemoDlg::OnBnClickedButtonMaxSize()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	m_iROIWidth = m_iWidth/m_BinArray[m_ComboBin.GetCurSel()];
	m_iROIHeight = m_iHeight/m_BinArray[m_ComboBin.GetCurSel()];
	m_iStartX = 0;
	m_iStartY = 0;
	UpdateData(false);
}

void CdemoDlg::OnCbnSelchangeComboScale()
{
	// TODO: 在此添加控件通知处理程序代码
	int index = combobox_scale.GetCurSel();
	if(index < 0)
	{
		fScale = 1;
		return;
	}
	fScale = ScaleArray[index]/100.0;
}

void CdemoDlg::OnBnClickedCheck3()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
 	ConnectCamera[iSelectedID].bSnapContinuous = m_check_continuous;
}
