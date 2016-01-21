
// fs_gs3Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "fs_gs3.h"
#include "fs_gs3Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
    public:
        CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
        enum { IDD = IDD_ABOUTBOX };
#endif

    protected:
        virtual void DoDataExchange ( CDataExchange* pDX ); // DDX/DDV support

// Implementation
    protected:
        DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx ( IDD_ABOUTBOX )
{
}

void CAboutDlg::DoDataExchange ( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange ( pDX );
}

BEGIN_MESSAGE_MAP ( CAboutDlg, CDialogEx )
END_MESSAGE_MAP()


// Cfs_gs3Dlg dialog



Cfs_gs3Dlg::Cfs_gs3Dlg ( CWnd* pParent /*=NULL*/ )
    : CDialogEx ( IDD_FS_GS3_DIALOG, pParent )
{
    m_hIcon = AfxGetApp()->LoadIcon ( IDR_MAINFRAME );
    // choosen RPM
    m_RPMValue = 0;

    //status of spindle in flashcut
    m_Spindle = false;


}

void Cfs_gs3Dlg::DoDataExchange ( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange ( pDX );
    DDX_Control ( pDX, IDC_EDIT1, m_RPM );
    DDX_Control ( pDX, IDC_SPINDLE, m_SpindleState );
    DDX_Control ( pDX, IDC_STATUS, m_Status );
}

BEGIN_MESSAGE_MAP ( Cfs_gs3Dlg, CDialogEx )
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// Cfs_gs3Dlg message handlers

BOOL Cfs_gs3Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT ( ( IDM_ABOUTBOX & 0xFFF0 ) == IDM_ABOUTBOX );
    ASSERT ( IDM_ABOUTBOX < 0xF000 );

    CMenu* pSysMenu = GetSystemMenu ( FALSE );

    if ( pSysMenu != NULL ) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString ( IDS_ABOUTBOX );
        ASSERT ( bNameValid );

        if ( !strAboutMenu.IsEmpty() ) {
            pSysMenu->AppendMenu ( MF_SEPARATOR );
            pSysMenu->AppendMenu ( MF_STRING, IDM_ABOUTBOX, strAboutMenu );
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon ( m_hIcon, TRUE );			// Set big icon
    SetIcon ( m_hIcon, FALSE );		// Set small icon

    // TODO: Add extra initialization here

    // not connected
    m_Status.SetWindowText ( _T ( "Motor not connected" ) );

    SetTimer ( 1, 100, NULL );
    modbus_t *ctx;
    uint16_t tab_reg[64];
    int rc;
    int i;

    ctx = modbus_new_rtu ( "COM1", 115200, 'N', 8, 1 );

    if ( modbus_connect ( ctx ) == -1 ) {
        _RPT1 ( _CRT_WARN, "Connection failed: %s\n", modbus_strerror ( errno ) );
        modbus_free ( ctx );
        return -1;
    }

    modbus_set_slave ( ctx, 1 );

    rc = modbus_read_input_registers ( ctx, 0, 10, tab_reg );

    if ( rc == -1 ) {
        _RPT1 ( _CRT_WARN, "%s\n", modbus_strerror ( errno ) );
        return -1;
    }

    for ( i = 0; i < rc; i++ ) {
        _RPT2 ( _CRT_WARN, "reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i] );
    }

    modbus_close ( ctx );
    modbus_free ( ctx );

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void Cfs_gs3Dlg::OnSysCommand ( UINT nID, LPARAM lParam )
{
    if ( ( nID & 0xFFF0 ) == IDM_ABOUTBOX ) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();

    } else {
        CDialogEx::OnSysCommand ( nID, lParam );
    }
}

BOOL CALLBACK EnumWindowsProc ( HWND hwnd, LPARAM lParam )
{
    char buffer[256];
    int written = GetWindowTextA ( hwnd, buffer, sizeof ( buffer ) / sizeof ( buffer[0] ) - 1 );

    if ( written && strstr ( buffer, "FlashCut CNC - " ) != NULL ) {
        * ( HWND* ) lParam = hwnd;
        return FALSE;
    }

    return TRUE;
}

HWND GetFlashCutHWND ( void )
{
    HWND hWnd = NULL;
    EnumWindows ( EnumWindowsProc, ( LPARAM ) &hWnd );
    return hWnd;
}

void Cfs_gs3Dlg::Jog ( int direction )
{
    // find main flashcut window
    HWND hwnd = GetFlashCutHWND();

    if ( hwnd == NULL ) {
        return;
    }

    switch ( direction ) 	{

        case 0:
            // Z+
            // start moving
            ::SendMessage ( hwnd, FC_Z_PLUS, 0, 0 );
            // stop moving
            ::SendMessage ( hwnd, FC_Z_PLUS, 1, 0 );
            break;

        case 1:
            // Z-
            // start moving
            ::SendMessage ( hwnd, FC_Z_MINUS, 0, 0 );
            //stop moving
            ::SendMessage ( hwnd, FC_Z_MINUS, 1, 0 );
            break;

        case 2:
            // X+
            // start moving
            ::SendMessage ( hwnd, FC_X_PLUS, 0, 0 );
            //stop moving
            ::SendMessage ( hwnd, FC_X_PLUS, 1, 0 );
            break;

        case 3:
            // X-
            // start moving
            ::SendMessage ( hwnd, FC_X_MINUS, 0, 0 );
            //stop moving
            ::SendMessage ( hwnd, FC_X_MINUS, 1, 0 );
            break;

        case 4:
            // Y+
            // start moving
            ::SendMessage ( hwnd, FC_Y_PLUS, 0, 0 );
            //stop moving
            ::SendMessage ( hwnd, FC_Y_PLUS, 1, 0 );
            break;

        case 5:
            // Y-
            // start moving
            ::SendMessage ( hwnd, FC_Y_MINUS, 0, 0 );
            //stop moving
            ::SendMessage ( hwnd, FC_Y_MINUS, 1, 0 );
            break;
    }
}

void Cfs_gs3Dlg::OnTimer ( UINT nIDEvent )
{
    TCHAR szBuf[2048];
    LONG lResult;
    HWND spindle_speed = NULL;

    // find main flashcut window
    HWND hwnd = GetFlashCutHWND();

    if ( hwnd == NULL ) {
        m_RPM.SetWindowText ( _T ( "-0" ) );
        return;

    }

    // Spindle On/Off,
    HWND button_spindle = ::FindWindowEx ( hwnd, NULL, _T ( "Button" ), _T ( "Spindle (Off)" ) );

    if ( button_spindle == NULL ) {
        button_spindle = ::FindWindowEx ( hwnd, NULL, _T ( "Button" ), _T ( "Spindle (On)" ) );

        if ( button_spindle == NULL ) {
            m_RPM.SetWindowText ( _T ( "-1" ) );
            return;

        } else {
            // spindle is On
            m_Spindle = true;

        }

    } else {
        // spindle is Off
        m_Spindle = false;
    }

    // in case  layout of gui is different, search for "Spindle RPM" in a Static

    // Spindle RPM, next is the trackbar, next is the ComoboBox
    HWND static_spindle_text = ::FindWindowEx ( hwnd, NULL, _T ( "Static" ), _T ( "Spindle RPM" ) );

    if ( static_spindle_text == NULL ) {
        m_RPM.SetWindowText ( _T ( "-1" ) );
        return;
    }

    // iterate to right control
    spindle_speed = ::GetWindow ( static_spindle_text, GW_HWNDNEXT );
    spindle_speed = ::GetWindow ( spindle_speed, GW_HWNDNEXT );

    TCHAR className[256];

    // get the class name
    GetClassName ( spindle_speed, className, sizeof ( className ) / sizeof ( className[0] ) );

    // check is class ComboBox
    if ( StrCmp ( className, _T ( "ComboBox" ) ) != 0 ) {
        m_RPM.SetWindowText ( _T ( "-2" ) );
        return;
    }

    // less precise way of finding it.
//	spindle_speed = ::FindWindowEx(hwnd, NULL, _T("msctls_trackbar32"), 0);
//	spindle_speed = ::GetWindow(spindle_speed, GW_HWNDNEXT);

    // fetch RPM  value
    lResult = ::SendMessage ( spindle_speed, WM_GETTEXT, sizeof ( szBuf ) / sizeof ( szBuf[0] ), ( LPARAM ) szBuf );

    if ( lResult ) {

        // convert to int
        m_RPMValue = _ttoi ( szBuf );

        // copy into dialog
        m_RPM.SetWindowText ( szBuf );

    } else {
        m_RPM.SetWindowText ( _T ( "-3" ) );
    }

    // if we get to here, spindle state is known m_Spindle (on/off) and speed in m_RPM

    // update the button state
    m_SpindleState.SetCheck ( m_Spindle );

    if ( m_Spindle ) {
        m_SpindleState.SetWindowText ( _T ( "Spindle (On)" ) );

    }	else {

        m_SpindleState.SetWindowText ( _T ( "Spindle (Off)" ) );
    }
}

void Cfs_gs3Dlg::OnPaint()
{
    if ( IsIconic() ) {
        CPaintDC dc ( this ); // device context for painting

        SendMessage ( WM_ICONERASEBKGND, reinterpret_cast<WPARAM> ( dc.GetSafeHdc() ), 0 );

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics ( SM_CXICON );
        int cyIcon = GetSystemMetrics ( SM_CYICON );
        CRect rect;
        GetClientRect ( &rect );
        int x = ( rect.Width() - cxIcon + 1 ) / 2;
        int y = ( rect.Height() - cyIcon + 1 ) / 2;

        // Draw the icon
        dc.DrawIcon ( x, y, m_hIcon );

    } else {
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cfs_gs3Dlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR> ( m_hIcon );
}

