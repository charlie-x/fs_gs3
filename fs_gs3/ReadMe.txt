TODO: Work in progress.

Modbus part not written/tested yet. 
Just pulling in data from FlashCut GUI.

Needs libmodbus modbus.dll, my untested windows x64 version is here.
	https://github.com/charlie-x/libmodbus

FlashCut 4.5.24
	I don't recommend using other versions just in case the GUI changes

DuraPulse GS3 Automation Direct
	http://www.automationdirect.com/adc/Shopping/Catalog/Drives/DURApulse_-_GS3_(230_-z-_460_VAC_Sensorless_Vector_Control)/DURApulse_-_GS3_Drive_Units_(230_-z-_460_VAC_SLV)

RS485 to RS232/USB adapter
	http://www.automationdirect.com/adc/Shopping/Catalog/Communications/Serial/Serial_Isolators_-a-_Converters/USB-485M

Install USB drivers for RS485 adapter, test connection with GSoft
	http://support.automationdirect.com/products/gsoft.html

Run FlashCut, switch on drive RPM options in Variable Output config (Main Screen / Show controls On Main Screen )
Keep Name Spindle RPM and add RPM values you want to use in the flashcut config page
