/*
 * Version.h
 *
 *  Created on: 01.07.2020
 *      Author: harald.schmittle
 */

#ifndef INC_VERSION_H_
#define INC_VERSION_H_

/*
//_____________________________________________________________________________
// Version 2.0.1 (2019-11-13)
// - first release version
//
// ACHTUNG:
//
//    Firmwareversonen V1.x.x gilt für:     MSTKN-S-SG14-001
//
//    Firmwareversioen V2.x.x gelten für    MSTKN-SM-A1-0001
//                            und           MSTKN-S-SG14-002
//_____________________________________________________________________________
*/
// Das folgende sind jetzt die eigentlichen Versionen:

// Version 1.0.6 (2023-09-14)
// - DHCP implemented
#define VER_MAJOR   1
#define VER_MINOR   0
#define VER_BUILD   6

/*
// Version 1.0.5 (2023-09-14)
// - #ifdef _ONE_BYTE_INTERRUPT from Preprocessor and in code changed to #ifndef _RS485_POLL
//   so UART1 receive is working with interrupt per default
#define VER_MAJOR   1
#define VER_MINOR   0
#define VER_BUILD   5

// Version 1.0.4 (2023-08-07)
// - LampTest is activated
#define VER_MAJOR   1
#define VER_MINOR   0
#define VER_BUILD   4

// Version 1.0.3 (2023-03-27)
// - on the ProfiNet there is the AT24C02D for replacement
//   it has only 8bytes PageWrite, an 16bytes page EEPROM can also be written with 8bytes - twice
// - optimize EEPROM-Write (only changed pages are written)
#define VER_MAJOR   1
#define VER_MINOR   0
#define VER_BUILD   3

///! @NOTE Calibration bug correction - UID in CONFIG was not correct
#define VER_MAJOR   1
#define VER_MINOR   0
#define VER_BUILD   2

// CFG/TEACH korrigiert, Basis für Anstoss über BootLoader Schnittstelle
// blinkt jetzt wieder bei Anwahl, auch wenn nicht belegt
// nach Kalibrieren wird jetzt CFG nachgeladen, da hat etwas gefehlt
#define VER_MAJOR   0
#define VER_MINOR   9
#define VER_BUILD   9

///! @NOTE change HSE to 12MHz because F401RE has problem with 24MHz
//   check for F401RC or F401RE and configure clock then
#define VER_MINOR   9
#define VER_BUILD   8

///! @NOTE Calibration bug correction - UID in CONFIG was not correct
#define VER_MINOR   9
#define VER_BUILD   7

///! @NOTE Calibration bug correction and collision blink-code
#define VER_MINOR   9
#define VER_BUILD   6

#define VER_MINOR   9
#define VER_BUILD   5
*/

#define VERSION  ((((VER_MAJOR << 8)) + VER_MINOR) << 8) + VER_BUILD


//#ifdef __MSTKN_HM_IO_LINK
#ifdef VERSION          // added by ahmad
#define HW_TTNR "MSTKN-M-IO-A1-01"	// for PCB with STM32F401RC
#endif

#ifdef __MSTKN_HM_WZ_2
#define HW_TTNR "MSTKN-M-WZ-A1-02"	// for PCB with STM32F401RE
#endif

#ifdef __MSTKN_HM_WZ_1
#define HW_TTNR "MSTKN-M-WZ-A1-01"	// for PCB with STM32F401RC
#endif


#if !defined HW_TTNR
#  error "Must define hardware type code (TTNR)!"
#endif


/*
 UniqeID: [31-16]/[15-11]/[10-7]/[6-2]/[1-0] - [CRC code from chip UID]/[dd]/[mm]/[yy]/[boot loader address]
*/

#if !defined UNIQUE_ID
#  define UNIQUE_ID   0				// dummy - wird im Bootloader ersetzt.
#endif

#define BAUDRATE_INDEX 4



#endif /* INC_VERSION_H_ */
