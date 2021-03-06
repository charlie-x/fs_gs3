
// fs_gs3Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "fs_gs3.h"
#include "fs_gs3Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// names for registry
const CString strSection	= _T ( "FCGS3" );
const CString strCOMPort	= _T ( "COMPORT" );
const CString strBAUDRate	= _T ( "BAUD" );
const CString strStopBits	= _T ( "STOP" );
const CString strParity		= _T ( "PARITY" );
const CString strBits       = _T ( "BITS" );
const CString strRatio      = _T ( "RATIO" );
const CString strWindowPos  = _T ( "WINDOWPOS" );


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
    : CDialogEx ( IDD_FS_GS3_DIALOG, pParent ),
      ctx ( NULL ), m_RPMValue ( 0 ), m_Spindle ( false ),
      vfd ( NULL ), last_motor_state ( -1 ), motor_status ( -1 ),
      last_rpm_slider_pos ( -1 ),
      direction ( -1 )
{
    m_hIcon = AfxGetApp()->LoadIcon ( IDR_MAINFRAME );

}

void Cfs_gs3Dlg::DoDataExchange ( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange ( pDX );
    DDX_Control ( pDX, IDC_EDIT1, m_RPM );
    DDX_Control ( pDX, IDC_SPINDLE, m_SpindleState );
    DDX_Control ( pDX, IDC_STATUS, m_Status );
    DDX_Control ( pDX, IDC_DRIVE_STATUS, m_DriveStatus );
    DDX_Control ( pDX, IDC_DRIVE_DIRECTION, m_Direction );
}

BEGIN_MESSAGE_MAP ( Cfs_gs3Dlg, CDialogEx )
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED ( IDC_CONNECT, &Cfs_gs3Dlg::OnBnClickedConnect )
    ON_BN_CLICKED ( IDC_SERCONFIG, &Cfs_gs3Dlg::OnBnClickedSerconfig )
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// Cfs_gs3Dlg message handlers

BOOL Cfs_gs3Dlg::OnInitDialog()
{
    CWinApp* pApp = AfxGetApp();
    ASSERT ( pApp );

    if ( pApp == NULL ) {
        return FALSE;
    }

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

    // TODO: Add extra init here
    last_motor_state = -1;
    motor_status = -1;
    direction = -1;
    last_rpm_slider_pos = -1;

    vfd = new VFD;
    ASSERT ( vfd );

    // critical requirement
    if ( vfd == NULL ) {
        return FALSE;
    }

    // not connected
    m_Status.SetWindowText ( _T ( "Motor not connected" ) );


    // debug
    //SetTimer ( 1, 10, NULL );

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

    int text_length = GetWindowTextA ( hwnd, buffer, sizeof ( buffer ) / sizeof ( buffer[0] ) - 1 );

    // using the " - " differentiates from popup child windows. (check parent is desktop?)
    if ( text_length && strstr ( buffer, "FlashCut CNC - " ) != NULL ) {
        * ( HWND* ) lParam = hwnd;
        return FALSE;
    }

    * ( HWND* ) lParam = NULL;
    return TRUE;
}

/**
 * Find FlashCut window
 *
 * Returns: hWnd of GUI
 */
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

        case JOG_X_PLUS:
            // X+
            // start moving
            ::SendMessage ( hwnd, FC_X_PLUS, 0, 0 );
            // stop moving
            ::SendMessage ( hwnd, FC_X_PLUS, 1, 0 );
            break;

        case JOG_X_MINUS:
            // X-
            // start moving
            ::SendMessage ( hwnd, FC_X_MINUS, 0, 0 );
            // stop moving
            ::SendMessage ( hwnd, FC_X_MINUS, 1, 0 );
            break;

        case JOG_Y_PLUS:
            // Y+
            // start moving
            ::SendMessage ( hwnd, FC_Y_PLUS, 0, 0 );
            // stop moving
            ::SendMessage ( hwnd, FC_Y_PLUS, 1, 0 );
            break;

        case JOG_Y_MINUS:
            // Y-
            // start moving
            ::SendMessage ( hwnd, FC_Y_MINUS, 0, 0 );
            // stop moving
            ::SendMessage ( hwnd, FC_Y_MINUS, 1, 0 );
            break;

        case JOG_Z_PLUS:
            // Z+
            // start moving
            ::SendMessage ( hwnd, FC_Z_PLUS, 0, 0 );
            // stop moving
            ::SendMessage ( hwnd, FC_Z_PLUS, 1, 0 );
            break;

        case JOG_Z_MINUS:
            // Z-
            // start moving
            ::SendMessage ( hwnd, FC_Z_MINUS, 0, 0 );
            // stop moving
            ::SendMessage ( hwnd, FC_Z_MINUS, 1, 0 );
            break;
    }
}

void Cfs_gs3Dlg::OnTimer ( UINT_PTR nIDEvent )
{
    TCHAR szBuf[2048];

    LRESULT lResult;
    HWND spindle_speed = NULL;

    // find main flashcut window
    HWND hwnd = GetFlashCutHWND();

    if ( hwnd == NULL ) {
        m_Status.SetWindowText ( _T ( "Couldn't find flashcut" ) );
        return;
    }

    // Spindle On/Off,
    HWND button_spindle = ::FindWindowEx ( hwnd, NULL, _T ( "Button" ), _T ( "Spindle (Off)" ) );

    if ( button_spindle == NULL ) {
        button_spindle = ::FindWindowEx ( hwnd, NULL, _T ( "Button" ), _T ( "Spindle (On)" ) );

        if ( button_spindle == NULL ) {
            m_Status.SetWindowText ( _T ( "Can't find Spindle RPM" ) );
            return;

        } else {
            // spindle is On
            m_Spindle = true;
        }

    } else {
        // spindle is Off
        m_Spindle = false;
    }

    // in case  layout of GUI is different, search for "Spindle RPM" in a Static

    // Spindle RPM, next is the trackbar, next is the ComoboBox
    HWND static_spindle_text = ::FindWindowEx ( hwnd, NULL, _T ( "Static" ), _T ( "Spindle RPM" ) );

    if ( static_spindle_text == NULL ) {
        m_SpindleState.SetWindowText ( _T ( "-1" ) );
        return;
    }

    // iterate to right control
    HWND spindle_speed_msctls_trackbar32 = ::GetWindow ( static_spindle_text, GW_HWNDNEXT );
    ASSERT ( spindle_speed_msctls_trackbar32 );

    if ( spindle_speed_msctls_trackbar32 == NULL ) {
        m_Status.SetWindowText ( _T ( "Can't find RPM trackbar" ) );
        return;
    }

    SetLastError ( ERROR_SUCCESS );
    // Slider POS is the index of the slider, which is just something like 0-100, it doesn't map to an RPM
    // if RPM is different and slider pos is the same, RPM hasn't updated, kill focus hasn't been received.
    LRESULT current_rpm_slider_pos = ::SendMessage ( spindle_speed_msctls_trackbar32, TBM_GETPOS, 0, 0 );

    if ( GetLastError() != ERROR_SUCCESS ) {
        m_Status.SetWindowText ( _T ( "Couldn't get slider position" ) );
        return;
    }

    spindle_speed = ::GetWindow ( spindle_speed_msctls_trackbar32, GW_HWNDNEXT );
    ASSERT ( spindle_speed );

    if ( spindle_speed == NULL ) {
        m_Status.SetWindowText ( _T ( "Can't find RPM edit box" ) );
        return;
    }

    TCHAR className[256];

    // get the class name
    GetClassName ( spindle_speed, className, sizeof ( className ) / sizeof ( className[0] ) );

    // check is class ComboBox
    if ( StrCmp ( className, _T ( "ComboBox" ) ) != 0 ) {
        m_Status.SetWindowText ( _T ( "Can't find spindle control" ) );
        return;
    }

    // SendMessage doesn't set a success value
    SetLastError ( ERROR_SUCCESS );
    // fetch RPM  value
    lResult = ::SendMessage ( spindle_speed, WM_GETTEXT, sizeof ( szBuf ) / sizeof ( szBuf[0] ), ( LPARAM ) szBuf );

    // lResult doesn't tell us about a failure since its dependant on the message type, WM_GETTEXT = length?
    if ( GetLastError() == ERROR_SUCCESS ) {

        // convert to int
        m_RPMValue = _ttoi ( szBuf );


    } else {
        m_RPM.SetWindowText ( _T ( "-3" ) );
    }

    // update the button state
    m_SpindleState.SetCheck ( m_Spindle );

    // show status
    if ( m_Spindle ) {
        m_SpindleState.SetWindowText ( _T ( "Spindle (On)" ) );

    }	else {

        m_SpindleState.SetWindowText ( _T ( "Spindle (Off)" ) );
    }

    // if we get to here, spindle state is known m_Spindle (on/off) and speed in m_RPM
    // from flashcut

    // send to VFD

    // adds about 300ms of latency when not connected to VFD for each call, timeout on libmodbus?

    // send new RPM to GSx, comms are slow to VFD so only update when you need too
    static int rpm = -1;

    // first check if RPM is different.
    if ( rpm != m_RPMValue ) {

        // it'd probably be ok to update last rpm here, but there might be an edge case where rpm doesn't change and slider does.

        // second check if RPM slider position is different as well.

        if ( current_rpm_slider_pos != last_rpm_slider_pos ) {

            // update rpm and slider pos last state.
            last_rpm_slider_pos = current_rpm_slider_pos;
            rpm = m_RPMValue;

            if ( vfd->update_rpm ( m_RPMValue ) == false ) {

                m_Status.SetWindowText ( _T ( "Unable to update RPM in VFD" ) );

                // wasn't able to pass to VFD, drop out early
                return;

            } else {

                //changed on VFD, so reflect in window
                // copy into dialog
                m_RPM.SetWindowText ( szBuf );

            }
        }
    }

    // once per loop
    vfd->read_status_2();

    direction = vfd->direction();

    switch ( direction ) {
        case -1:
            m_Direction.SetWindowText ( _T ( "Direction Error" ) );
            break;

        case 0:
            m_Direction.SetWindowText ( _T ( "Forward" ) );
            break;

        case 1:
            m_Direction.SetWindowText ( _T ( "Rev to Fwd" ) );
            break;

        case 2:
            m_Direction.SetWindowText ( _T ( "Fwd To Rev" ) );
            break;

        case 3:
            m_Direction.SetWindowText ( _T ( "Reverse" ) );
            break;

        default:
            m_Direction.SetWindowText ( _T ( "Unknown state" ) );
            break;
    }

    switch ( vfd->drive_state() ) {
        case -1:
            m_DriveStatus.SetWindowText ( _T ( "Motor Error" ) );
            break;

        case 0:
            m_DriveStatus.SetWindowText ( _T ( "Stopped" ) );
            break;

        case 1:
            m_DriveStatus.SetWindowText ( _T ( "Stopping" ) );
            break;

        case 2:
            m_DriveStatus.SetWindowText ( _T ( "Standby" ) );
            break;

        case 3:
            m_DriveStatus.SetWindowText ( _T ( "Running" ) );
            break;

        default:
            m_DriveStatus.SetWindowText ( _T ( "Unknown state" ) );
            break;

    }

    // if keypad stop is pressed, then don't turn the motor back on, regardless of setting?
    // caching state will do that, but prefer a better way. motor will change state to slowing ,
    // so if spindle on + motor slowing = keypad stop pressed?
    // todo: even better if we detect that the keypad stopped the motor and updates flashcut

    if ( last_motor_state != m_SpindleState.GetCheck() ) {

        last_motor_state = m_SpindleState.GetCheck();

        // check if motor is running

        // check motor status, 0 OFF, 1 ON, -1 ERROR
        motor_status = vfd->motor_running();

        if ( motor_status == VFD::DRIVE_RUNNING ) {

            // yes is running, is GUI button set to off?
            if ( m_SpindleState.GetCheck() == FALSE ) {

                // yes, turn off motor, can't turn off motor if option not selected
                vfd->turn_off_motor();
            }

            // otherwise leave it alone, already running.

        } else
            if ( motor_status == VFD::DRIVE_STOPPED ) {

                // motor is not running, then check if spindle control is on, if it is switch on the motor
                if ( m_SpindleState.GetCheck() == TRUE ) {
                    // turn on motor
                    vfd->turn_on_motor();
                }

                // otherwise leave it alone, already stopped

            } else
                if ( motor_status == -1 ) {
                    m_Status.SetWindowText ( _T ( "Error in reading VFD state" ) );

                    // somethings wrong
                    return;
                }

    }
}

void Cfs_gs3Dlg::OnBnClickedSerconfig()
{
    CWinApp* pApp = AfxGetApp();
    ASSERT ( pApp );

    if ( pApp == NULL ) {
        return;
    }

    if ( ctx ) {
        AfxMessageBox ( _T ( "Can't config while connected!" ) );
        return;
    }

    SerialSetup dlg;
    dlg.DoModal();

    // update ratio
    vfd->set_ratio ( _ttof ( pApp->GetProfileString ( strSection, strRatio, _T ( "8.03" ) ) ) );

}


void Cfs_gs3Dlg::PostNcDestroy()
{
    CDialogEx::PostNcDestroy();
}


void Cfs_gs3Dlg::OnCancel()
{
    // ask if turn off motor if program closes,since accidental close
    if ( ctx &&  vfd->motor_running() != VFD::DRIVE_STOPPED ) {
        if ( AfxMessageBox ( _T ( "Spindle is running, turn it off? " ), MB_YESNO ) == IDYES ) {
            vfd->turn_off_motor();
        }
    }

    CDialogEx::OnCancel();
}


void Cfs_gs3Dlg::OnOK()
{
    // ask if turn off motor if program closes,since accidental close
    if ( ctx && vfd->motor_running() != VFD::DRIVE_STOPPED ) {
        if ( AfxMessageBox ( _T ( "Spindle is running, turn it off? " ), MB_YESNO ) == IDYES ) {
            vfd->turn_off_motor();
        }
    }

    CDialogEx::OnOK();
}


BOOL Cfs_gs3Dlg::DestroyWindow()
{
    WINDOWPLACEMENT wp;

    GetWindowPlacement ( &wp );
    AfxGetApp()->WriteProfileBinary ( strSection, strWindowPos, ( LPBYTE ) &wp, sizeof ( wp ) );

    return CDialogEx::DestroyWindow();
}


void Cfs_gs3Dlg::OnShowWindow ( BOOL bShow, UINT nStatus )
{
    CDialogEx::OnShowWindow ( bShow, nStatus );
    WINDOWPLACEMENT *lwp;
    UINT nl;

    if ( AfxGetApp()->GetProfileBinary ( strSection, strWindowPos, ( LPBYTE* ) &lwp, &nl ) ) {
        if ( lwp->flags == WPF_RESTORETOMAXIMIZED ) {
            ShowWindow ( SW_MAXIMIZE );

        } else {
            SetWindowPos ( NULL,
                           lwp->rcNormalPosition.left,
                           lwp->rcNormalPosition.top,
                           lwp->rcNormalPosition.right - lwp->rcNormalPosition.left,
                           lwp->rcNormalPosition.bottom - lwp->rcNormalPosition.top,
                           NULL );
        }

        delete[] lwp;
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

void Cfs_gs3Dlg::OnBnClickedConnect()
{
    // quick out
    if ( ctx ) {

        // already opened?
        m_Status.SetWindowText ( _T ( "Already opened!" ) );

        return;
    }

    CWinApp* pApp = AfxGetApp();
    ASSERT ( pApp );

    if ( pApp == NULL ) {
        return;
    }

    CString  comPort;

    // read from registry?
    comPort = pApp->GetProfileString ( strSection, strCOMPort );

    if ( comPort.IsEmpty() ) {
        return;
    }

    // GSx drives seem to default to ASCII,7 vs RTU,8.

    // pull in default values from registry, or use defaults
    uint32_t  baud = pApp->GetProfileInt ( strSection, strBAUDRate , 115200 );
    uint8_t bits = pApp->GetProfileInt ( strSection, strBits, 8 );
    double stopbits = pApp->GetProfileInt ( strSection, strStopBits, 1 ) / 10.0;
    uint8_t parity = pApp->GetProfileInt ( strSection, strParity, 'N' );

    // update ratio
    vfd->set_ratio ( _ttof ( pApp->GetProfileString ( strSection, strRatio, _T ( "8.03" ) ) ) );

    ctx = modbus_new_rtu ( CT2A ( ( LPCTSTR ) comPort ), baud, parity, bits, ( int ) stopbits );

    if ( modbus_connect ( ctx ) == -1 ) {

        _RPT1 ( _CRT_WARN, "Connection failed: %s\n", modbus_strerror ( errno ) );

        m_Status.SetWindowText ( _T ( "Connection failed" ) );

        modbus_free ( ctx );

        ctx = NULL;
        vfd->set_ctx ( ctx );

        return ;
    }

    // switch on debug mode
    //modbus_set_debug ( ctx, 1);

    // copy over to VFD class..todo: fix
    vfd->set_ctx ( ctx );

    if ( modbus_set_slave ( ctx, 1 ) == -1 ) {

        _RPT1 ( _CRT_WARN, "modbus_set_slave failed: %s\n", modbus_strerror ( errno ) );
        m_Status.SetWindowText ( CString ( modbus_strerror ( errno ) ) );
        return;
    }

    // we need to query the VFD and make sure its a valid connection here

    // all good so far
    m_Status.SetWindowText ( _T ( "Port opened" ) );

    if ( !vfd->control_frequency() ) {
        if ( AfxMessageBox ( _T ( "Can't control frequency set P4.0 to 5\nShall i do it, or use keypad?" ), MB_YESNO ) == IDYES ) {

            if ( modbus_write_register ( ctx, VFD::control_freq_addr, 5 ) == -1 ) {

                AfxMessageBox ( _T ( "Failed to change P4.0 to 5, use keypad" ) );
            }
        }
    }

    if ( !vfd->control_motor() ) {
        if ( AfxMessageBox ( _T ( "Can't control motor set P3.0 to 3(suggested) or 4\nShall i do it, or use keypad?" ), MB_YESNO ) == IDYES ) {
            if ( modbus_write_register ( ctx, VFD::control_drive_addr, 3 ) == -1 ) {

                AfxMessageBox ( _T ( "Failed to change P3.0 to 3(Suggested) or 4, use keypad" ) );
            }
        }
    }

    AfxMessageBox ( _T ( "Clicking OK will set drive to status of FlashCut spindle\nCheck if spindle is switched to on or off in FlashCut, and correct RPM is selected" ) );

    //modbus_set_debug ( ctx, 0 );
    //kick off timer
    SetTimer ( 1, 10, NULL );
}
