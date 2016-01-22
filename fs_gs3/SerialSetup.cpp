// SerialSetup.cpp : implementation file
//

#include "stdafx.h"
#include "fs_gs3.h"
#include "SerialSetup.h"
#include "afxdialogex.h"


// SerialSetup dialog

IMPLEMENT_DYNAMIC ( SerialSetup, CDialogEx )

SerialSetup::SerialSetup ( CWnd* pParent /*=NULL*/ )
    : CDialogEx ( IDD_SERIAL, pParent )
{
    // store it
    pApp = AfxGetApp();
    ASSERT ( pApp );

    if ( pApp == NULL ) {
        return;
    }

    // fill in defaults from registry, or default selections
    m_Baud		= pApp->GetProfileInt ( strSection, strBAUDRate, 115200 );
    m_Bits		= pApp->GetProfileInt ( strSection, strBits, 8 );

    m_StopBits  = ( double ) pApp->GetProfileInt ( strSection, strStopBits, 10 ) / 10.0;
    m_Parity	= pApp->GetProfileInt ( strSection, strParity, 'N' );

    // CComboBoxes aren't created yet, can't set cursels, do that in OnInitDialog

}

SerialSetup::~SerialSetup()
{
}

void SerialSetup::DoDataExchange ( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange ( pDX );
    DDX_Control ( pDX, IDC_BAUDRATE, m_BaudRate );
    //  DDX_Control(pDX, IDC_BITS, m);
    DDX_Control ( pDX, IDC_BITS, m_BitsCombo );
    DDX_Control ( pDX, IDC_PARITY, m_ParityCombo );
    DDX_Control ( pDX, IDC_STOPBITS, m_StopBitsCombo );
}


BEGIN_MESSAGE_MAP ( SerialSetup, CDialogEx )
    ON_CBN_KILLFOCUS ( IDC_BAUDRATE, &SerialSetup::OnCbnSelchangeBaudrate )
    ON_BN_CLICKED ( IDOK, &SerialSetup::OnBnClickedOk )
    ON_BN_CLICKED ( IDCANCEL, &SerialSetup::OnBnClickedCancel )
    ON_CBN_KILLFOCUS ( IDC_BITS, &SerialSetup::OnCbnKillfocusBits )
    ON_CBN_KILLFOCUS ( IDC_PARITY, &SerialSetup::OnCbnKillfocusParity )
    ON_CBN_KILLFOCUS ( IDC_STOPBITS, &SerialSetup::OnCbnKillfocusStopbits )
END_MESSAGE_MAP()

// SerialSetup message handlers

void SerialSetup::OnBnClickedOk()
{
    CDialogEx::OnOK();
    DestroyWindow();
}

void SerialSetup::OnBnClickedCancel()
{
    CDialogEx::OnCancel();
    DestroyWindow();
}

void SerialSetup::OnCbnKillfocusBits()
{
    CString temp;
    // update bits
    m_BitsCombo.GetWindowText ( temp );
    pApp->WriteProfileInt ( strSection, strBits, _ttoi ( temp ) );

}

void SerialSetup::OnCbnSelchangeBaudrate()
{
    CString temp;
    // update baud rate
    m_BaudRate.GetWindowText ( temp );
    pApp->WriteProfileInt ( strSection, strBAUDRate, _ttoi ( temp ) );
}

void SerialSetup::OnCbnKillfocusParity()
{
    CString temp;
    // update parity
    m_ParityCombo.GetWindowText ( temp );
    pApp->WriteProfileInt ( strSection, strParity, 'Y' );

    if ( temp == "N" ) {
        pApp->WriteProfileInt ( strSection, strParity, 'N' );
        m_Parity = 'N';
    }
}

void SerialSetup::OnCbnKillfocusStopbits()
{
    CString temp;
    // update stop bits rate
    m_StopBitsCombo.GetWindowText ( temp );
    int bits;
    bits = ( int ) ( ( double ) _ttof ( temp ) * 10.0 );
    pApp->WriteProfileInt ( strSection, strStopBits,  bits );
}


void SerialSetup::PostNcDestroy()
{
    CDialogEx::PostNcDestroy();
    // delete this;
}


BOOL SerialSetup::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    ASSERT_VALID ( &m_BaudRate );
    ASSERT_VALID ( &m_ParityCombo );
    ASSERT_VALID ( &m_BitsCombo );
    ASSERT_VALID ( &m_StopBitsCombo );

    CString temp;

    temp.Format ( _T ( "%d" ), m_Baud );
    m_BaudRate.SelectString ( 0, temp );

    temp.Format ( _T ( "%c" ), m_Parity );
    m_ParityCombo.SelectString ( 0, temp );

    temp.Format ( _T ( "%d" ), m_Bits );
    m_BitsCombo.SelectString ( 0, temp );

    temp.Format ( _T ( "%g" ), m_StopBits );
    // SelectString matches a partial
    m_StopBitsCombo.SetCurSel ( m_StopBitsCombo.FindStringExact ( 0, temp ) );


    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
