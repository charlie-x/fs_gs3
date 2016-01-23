#pragma once
#include "afxwin.h"

extern const CString strSection;
extern const CString strCOMPort;
extern const CString strBAUDRate;
extern const CString strStopBits;
extern const CString strParity;
extern const CString strBits;
extern const CString strRatio;
extern const CString strWindowPos;

// SerialSetup dialog

class SerialSetup : public CDialogEx
{
        DECLARE_DYNAMIC ( SerialSetup )

    public:
        SerialSetup ( CWnd* pParent = NULL ); // standard constructor
        virtual ~SerialSetup();

// Dialog Data
#ifdef AFX_DESIGN_TIME
        enum { IDD = IDD_SERIAL };
#endif

    protected:
        virtual void DoDataExchange ( CDataExchange* pDX ); // DDX/DDV support

        DECLARE_MESSAGE_MAP()
    public:
        uint32_t	m_Baud;
        uint8_t		m_Bits;
        double		m_StopBits;
        uint8_t		m_Parity;
        CWinApp* pApp;
        CComboBox m_BaudRate;
        CComboBox m_BitsCombo;
        CComboBox m_ParityCombo;
        CComboBox m_StopBitsCombo;
        afx_msg void OnCbnSelchangeBaudrate();
        afx_msg void OnBnClickedOk();
        afx_msg void OnBnClickedCancel();
        afx_msg void OnCbnKillfocusBits();
        afx_msg void OnCbnKillfocusParity();
        afx_msg void OnCbnKillfocusStopbits();
        virtual void PostNcDestroy();
        virtual BOOL OnInitDialog();
        CEdit m_COMPort;
        CEdit m_Ratio;
        afx_msg void OnKillfocusComport();
        afx_msg void OnKillfocusRatio();
};
