// fs_gs3Dlg.h : header file
//

#pragma once
#include "afxwin.h"

// FlashCut GUI messages
#define FC_X_PLUS		( WM_USER + 257 )
#define FC_X_MINUS		( WM_USER + 258 )
#define FC_Y_PLUS		( WM_USER + 259 )
#define FC_Y_MINUS		( WM_USER + 260 )
#define FC_Z_PLUS		( WM_USER + 261 )
#define FC_Z_MINUS		( WM_USER + 262 )

// Cfs_gs3Dlg dialog
class Cfs_gs3Dlg : public CDialogEx
{
// Construction
    public:
        Cfs_gs3Dlg ( CWnd* pParent = NULL );	// standard constructor

        void Jog ( int direction );

// Dialog Data
#ifdef AFX_DESIGN_TIME
        enum { IDD = IDD_FS_GS3_DIALOG };
#endif

    protected:
        virtual void DoDataExchange ( CDataExchange* pDX );	// DDX/DDV support


// Implementation
    protected:
        HICON m_hIcon;

        // Generated message map functions
        virtual BOOL OnInitDialog();
        afx_msg void OnSysCommand ( UINT nID, LPARAM lParam );
        afx_msg void OnPaint();
        afx_msg void OnTimer ( UINT_PTR nIDEvent );
        afx_msg HCURSOR OnQueryDragIcon();
        DECLARE_MESSAGE_MAP()
    public:
        modbus_t *ctx;
        CEdit m_RPM;
        int m_RPMValue;
        bool m_Spindle;
        CButton m_SpindleState;
        // status of connection
        CStatic m_Status;
        CEdit m_COMPort;
        afx_msg void OnBnClickedConnect();
        afx_msg void OnEnChangeComPort();
};
