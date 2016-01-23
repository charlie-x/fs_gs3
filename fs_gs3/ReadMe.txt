Modbus part not tested yet. 

Needs libmodbus modbus.dll, my untested windows x64 version is here.
	https://github.com/charlie-x/libmodbus

FlashCut 4.5.24
	I don't recommend using other versions just in case the GUI changes, easy to check though

DuraPulse GS3 Automation Direct
	http://www.automationdirect.com/adc/Shopping/Catalog/Drives/DURApulse_-_GS3_(230_-z-_460_VAC_Sensorless_Vector_Control)/DURApulse_-_GS3_Drive_Units_(230_-z-_460_VAC_SLV)

RS485 to RS232/USB adapter
	http://www.automationdirect.com/adc/Shopping/Catalog/Communications/Serial/Serial_Isolators_-a-_Converters/USB-485M

Install USB drivers for RS485 adapter, test connection with GSoft
	http://support.automationdirect.com/products/gsoft.html

Instructions
	Run FlashCut, switch on drive RPM options in Variable Output config (Main Screen / Show controls On Main Screen )
	Keep the default name as "Spindle RPM" and then add RPM values you want to use in the flashcut config page just 
	as if you were configuring a direct connection

	Choose the COM port and connect, the VFD should be preconfigured for RTU mode, default is ASCII mode

Todo
	Change the serial COM port to use >9 modern windows style
	Test it, so far written without VFD/motor/testing
