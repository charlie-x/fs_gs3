
// fs_gs3Dlg.h : header file
//

#pragma once
#include "afxwin.h"


// Cfs_gs3Dlg dialog
class Cfs_gs3Dlg : public CDialogEx
{
// Construction
public:
	Cfs_gs3Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FS_GS3_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_RPM;
	int m_RPMValue;
	bool m_Spindle;
	CButton m_SpindleState;
	// status of connection
	CStatic m_Status;
};
