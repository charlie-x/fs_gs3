All tested and working, x64 release and debug, x86 not tested but it should be fine

Needs libmodbus modbus.dll, my now tested windows x64 version is here, it has been windowified though.
	https://github.com/charlie-x/libmodbus

FlashCut 4.5.24
	I don't recommend using other versions just in case the GUI changes, easy to check though

DuraPulse GS3 Automation Direct, GS2 likely works too
	http://www.automationdirect.com/adc/Shopping/Catalog/Drives/DURApulse_-_GS3_(230_-z-_460_VAC_Sensorless_Vector_Control)/DURApulse_-_GS3_Drive_Units_(230_-z-_460_VAC_SLV)

RS485 to RS232/USB adapter
	http://www.automationdirect.com/adc/Shopping/Catalog/Communications/Serial/Serial_Isolators_-a-_Converters/USB-485M

Install USB drivers for RS485 adapter, test connection with GSoft
	http://support.automationdirect.com/products/gsoft.html

Instructions
	Run FlashCut, switch on drive RPM options in Variable Output config (Main Screen / Show controls On Main Screen )
	Keep the default name as "Spindle RPM" and then add RPM values you want to use in the flashcut config page just 
	as if you were configuring a direct connection

	Choose the COM port the RS485U shows up as and connect, the VFD should be preconfigured for RTU mode, 
    default is ASCII mode which libmodbus doesn't support
    
    P9.02 to 3 (MODBUS RTU 8,N,2)
    
    Software should be configured to
    9600,N,8,2
    
    Although this software asks you if you want to change thse values, it doesn't seem to allow it so use the keypad to change these settings. 
    GSoft doesn't allow it either than I can find. only keypad.

    Change P3.0 to 3 (allows modbus to control run/stop and keypad stop button to function), 4 does same without keypad stop control
    Change P4.0 to 5 (allows modbus to change frequency)

    Run and it set the RPM, use Spindle(On)/Spindle(Off) to switch it on and off, quitting this program won't turn off the spindle (yet)

Todo
	Change the serial COM port to use >9 modern windows style
    Add display of status parameters
    Graphs n stuff !
