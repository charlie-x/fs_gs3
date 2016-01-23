// fs_gs3Dlg.h : header file
//

#pragma once
#include "afxwin.h"

/// keeping these in here til they're figured out before moving to own class/headers

// FlashCut GUI messages, these do a 0/1 command for start/stop movement which explains
// some of the jerkiness and unpredictable behaviour of the GUI continous jog
#define FC_X_PLUS		( WM_USER + 257 )
#define FC_X_MINUS		( WM_USER + 258 )
#define FC_Y_PLUS		( WM_USER + 259 )
#define FC_Y_MINUS		( WM_USER + 260 )
#define FC_Z_PLUS		( WM_USER + 261 )
#define FC_Z_MINUS		( WM_USER + 262 )

enum {
    JOG_ALL_STOP,

    JOG_X_PLUS,
    JOG_X_MINUS,

    JOG_Y_PLUS,
    JOG_Y_MINUS,

    JOG_Z_PLUS,
    JOG_Z_MINUS
};

/*
	Modbus is strange (to me anyway) that the decimal is in a range, may map to a memory address
	so you have to know which range the register is in, then add the register value to that range.
	For the status monitors, seems to be 40000 + they're indexed at 1, versus 0, so it is +1,
	therefore Status Monitor 1 is 0x2100, which to get the decimal address is 40000+0x2100+1 = 48449
	Octal and Hex are just the address.

	Durapulse GS3 status registers form CH5 of manual

	Name						Hex	 Dec   Oct   Mode
	--------------------------- ---- ----- ----- ----
	Status Monitor 1			2100 48449 20400 RO
	Status Monitor 2			2101 48450 20401 RO
	Frequency Command F			2102 48451 20402 RO
	Output Frequency H			2103 48452 20403 RO
	Output Current A			2104 48453 20404 RO
	DC Bus Voltage d			2105 48454 20405 RO
	Output Voltage U			2106 48455 20406 RO
	Motor RPM					2107 48456 20407 RO
	Scale Frequency(Low Word)	2108 48457 20410 RO
	Scale Frequency(High Word)	2109 48458 20411 RO
	Power Factor Angle			210A 48459 20412 RO
	% Load						210B 48460 20413 RO
	PID Setpoint				210C 48461 20414 RO
	PID Feedback Signal(PV)		210D 48462 20415 RO
	Firmware Version			2110 48465 20420 RO
*/

// Helper macro
#define MODBUS_CMD(maj,minor) ((uint16_t)(maj<<8)+minor)


class VFD
{
    public:
        // commands for GS3 DuraPulse VFD
        enum modbuscmds  {
            hz_addr             = MODBUS_CMD ( 9, 26 ),
            run_stop_addr       = MODBUS_CMD ( 9, 27 ),

            control_freq_addr   = MODBUS_CMD ( 4, 0 ),
            control_drive_addr  = MODBUS_CMD ( 3, 0 ),

            status_monitor_2_addr = MODBUS_CMD ( 0x21, 01 ) //(BD42)

        } ;

    public:

        VFD() : ratio ( 1.105 ), status2_read ( false ), ctx ( NULL )
        {

        }

        void  set_ctx ( modbus_t *pctx )
        {
            ctx = pctx;
        }

        // display shows motor rpm, but we have to send VFD RPM
        void set_ratio ( double new_ratio )
        {
            ratio = new_ratio;
        }

        /*
        Status Monitor 2 - Memory Address h2101
        	Address	Bit(s) Val	AC Drive Status
        	Bit(s)	Binary(Dec)
        	-------	------		---------------
        	0 and 1	00 (0)		Drive operation stopped(STOP)
        			01 (1)		Run to Stop transition
        			10 (2)		Standby
        			11 (3)		Drive operation running(RUN)
        	2		1 (4)		JOG active
        	3 and 4	00 (0)		Rotational direction forward(FWD)
        			01 (8)		REV to FWD transition
        			10 (16)		FWD to REV transition
        			11 (24)		Rotational direction reverse(REV)
        	5 ~7	N/A			Reserved
        	8		1 (32)		Source of frequency determined by serial comm interface (P4.00 = 5)
        	9		1 (64)		Source of frequency determined by AI terminal(P4.00 = 2, 3, 4 or 6)
        	10		1 (128)		Source of operation determined by serial comm interface (P3.00 = 3 or 4)
        	11		1 (256)		Parameters have been locked (P9.07 = 1)
        	12		N/A			Copy command eable(sp?)
        */
        typedef struct  statusMonitor2_tag {
            uint16_t	drive_state : 2;
            uint16_t	jog_active : 1;
            uint16_t	direction : 2;
            uint16_t	reserved_1 : 3;
            uint16_t	freq_src_1 : 1;
            uint16_t	freq_src_2 : 1;
            uint16_t	freq_src_3 : 1;
            uint16_t	params_locked : 1;
            uint16_t	reserved_2 : 1;
        } statusMonitor2;

        enum {
            DRIVE_STOPPED,
            DRIVE_RUN_TO_STOP,
            DRIVE_STANDBY,
            DRIVE_RUNNING
        };

        statusMonitor2 status_monitor_2;

        bool status2_read;

        bool control_frequency ( void )
        {

            if ( status2_read == false ) {
                if ( read_status_2() == false ) {
                    return false;
                }
            }

            return ( status_monitor_2.freq_src_1 == 1 );
        }

        bool control_motor ( void )
        {

            if ( status2_read == false ) {
                if ( read_status_2() == false ) {
                    return false;
                }

            }

            return ( status_monitor_2.freq_src_3 == 1 );

        }

        bool read_status_2 ( void )
        {

            ASSERT ( ctx );

            if ( ctx == NULL ) {
                return false;
            }

            if ( modbus_read_registers ( ctx, status_monitor_2_addr, 1, ( uint16_t* ) &status_monitor_2 ) == -1 ) {

                _RPT1 ( _CRT_WARN, "modbus_read_registers failed: %s\n", modbus_strerror ( errno ) );
                return false;
            }

            // caches some results, cuts down on comms
            status2_read = true;

            return true;
        }

        int motor_running ( void )
        {

            if ( read_status_2()  == true ) {

                // can we control frequency
                if ( !control_frequency() ) {

                    _RPT0 ( _CRT_WARN, "Can't control frequency set P4.00 = 4\n" );
                }

                // Can we control drive on/off?
                if ( !control_motor() ) {

                    _RPT0 ( _CRT_WARN, "Can't control drive on/off set P3.0 = 3(suggested) or 4\n" );
                }

                if ( status_monitor_2.drive_state == DRIVE_STOPPED ) {
                    // not running
                    return 0;

                } else

                    if ( status_monitor_2.drive_state == DRIVE_RUNNING ) {
                        // running
                        return 1;
                    }

                // standby or slowing down
                return 1;
            }


            // error
            return -1;
        }

        bool turn_off_motor ( void )
        {

            ASSERT ( ctx );

            if ( ctx == NULL ) {
                return false;
            }

            if ( status_monitor_2.freq_src_3 == 1 ) {
                if ( modbus_write_register ( ctx, run_stop_addr, 0 ) == -1 ) {
                    _RPT1 ( _CRT_WARN, "modbus_write_register failed: %s\n", modbus_strerror ( errno ) );

                    return false;
                }
            }

            return true;
        }

        bool turn_on_motor ( void )
        {
            ASSERT ( ctx );

            if ( ctx == NULL ) {
                return false;
            }

            if ( status_monitor_2.freq_src_3 == 1 ) {
                if ( modbus_write_register ( ctx, run_stop_addr, 1 ) == -1 ) {

                    _RPT1 ( _CRT_WARN, "modbus_write_register failed: %s\n", modbus_strerror ( errno ) );
                    return false;
                }
            }

            return true;
        }

        bool update_rpm ( unsigned int rpm )
        {
            double  converted_rpm;

            // convert spindle to motor RPM

            if ( ratio ) {
                converted_rpm = ( double ) rpm / ratio;

            } else { converted_rpm = rpm; }

            // convert rpm to hZ
            // hZ value
            uint16_t hZ = ( uint16_t ) ( converted_rpm / 7 );

            // construction of a modbus message

            // 01 10 09 1b 00 02 04 02 58 00 01 5a 66

            // breakdown
            // 01 node address
            // 10 command (write registers)
            // 09 1b register to write to 9.26
            // 00 02 number of registers to write, consecutive
            // 04 amount of data to write
            // 02 58 00 01 data to send 0258 to 9.26 and 0001 to 9.27
            // 5a 66 crc

            ASSERT ( ctx );

            if ( ctx == NULL ) {
                return false;
            }

            // hard coded for testing
            // 208hZ = 1635 motor RPM
            // 104hZ = 797
            // 113hZ = 880
            // 124hZ = 973.3
            // 127hZ = 997
            // 128hZ = 998
            // 129hZ = 1014
            // 258hZ = 2048
            if ( modbus_write_register ( ctx, hz_addr, hZ ) == -1 ) {

                int err;
                _get_errno ( &err );
                _RPT1 ( _CRT_WARN, "modbus_write_register failed: %s\n", modbus_strerror ( err ) );

                return false;
            }

            return true;
        }

    private:

// this is a clone of the one in the dialog, should be in one place.
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
        afx_msg void OnBnClickedConnect();
        afx_msg void OnEnChangeComPort();
        afx_msg void OnBnClickedSerconfig();
        virtual void PostNcDestroy();
        virtual void OnCancel();
        virtual void OnOK();
};
