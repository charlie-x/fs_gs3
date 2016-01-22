// fs_gs3Dlg.h : header file
//

#pragma once
#include "afxwin.h"

// FlashCut GUI messages, these do a 0/1 state for start movement which explains
// some of the jerkiness and unpredictable behaviour of the GUI jog
#define FC_X_PLUS		( WM_USER + 257 )
#define FC_X_MINUS		( WM_USER + 258 )
#define FC_Y_PLUS		( WM_USER + 259 )
#define FC_Y_MINUS		( WM_USER + 260 )
#define FC_Z_PLUS		( WM_USER + 261 )
#define FC_Z_MINUS		( WM_USER + 262 )

#define MODBUS_CMD(maj,minor) ((maj<<8)+minor)

class VFD
{
        // commands for GS3 DuraPulse VFD
        enum {
            hz_addr         = MODBUS_CMD ( 9, 26 ),
            run_stop_addr   = MODBUS_CMD ( 9, 27 )
        } modbuscmds;

    public:

        VFD() : ratio ( 1.44 )
        {
        }

        void     set_ctx ( modbus_t *pctx )
        {
            ctx = pctx;
        }

        // display shows motor rpm, but we have to send VFD RPM
        void set_ratio ( double new_ratio )
        {
            ratio = new_ratio;
        }

        bool motor_running ( void )
        {
            return false;
        }

        bool turn_off_motor ( void )
        {
            if ( modbus_write_register ( ctx, run_stop_addr, 1 ) == 0 ) {
                return true;
            }

            return false;
        }

        bool turn_on_motor ( void )
        {
            if ( modbus_write_register ( ctx, run_stop_addr, 1 ) == 1 ) {
                return true;
            }

            return false;
        }

        bool update_rpm ( unsigned int rpm )
        {
            double  converted_rpm;

            // convert spindle to motor RPM
            converted_rpm = ( double ) rpm / ratio;

            // convert rpm to hZ

            // for now testing
            converted_rpm = 20;

            // hZ value is in decimal * 10, 60.0hZ is 600 decimal
            uint16_t hZ = ( uint16_t ) ( converted_rpm * 10.0f );

            // 01 node address
            // 10 command (write registers)
            // 09 1b register to write to 9.26
            // 00 02 number of registers to write, consecutive
            // 04 amount of data to write
            // 02 58 00 01 data to send 0258 to 9.26 and 0001 to 9.27
            // 5a 66 crc

            // 01 10 09 1b 00 02 04 02 58 00 01 5a 66

            // send to VFD as one command.
            uint16_t data[2];

            data[0] = ( hZ >> 8 );
            data[1] = ( hZ & 0xff );

            data[2] = 0;
            data[3] = 0; // motor on/off

            if ( modbus_write_register ( ctx, hz_addr, hZ ) == 1 ) {
                return true;
            }

            return false;
        }

    private:

        modbus_t *ctx;
        // conversion from VFD RPM to spindle RPM
        double ratio;
};


// Cfs_gs3Dlg dialog
class Cfs_gs3Dlg : public CDialogEx
{
// Construction
    public:
        Cfs_gs3Dlg ( CWnd* pParent = NULL );	// standard constructor
        ~Cfs_gs3Dlg()
        {
            if ( ctx ) {
                modbus_close ( ctx );
                modbus_free ( ctx );
                ctx = NULL;
            }

            delete vfd;
            vfd = NULL;
        }
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
        VFD *vfd;
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
