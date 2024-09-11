/*
 * flash_config_hm.h
 *
 *  Created on: 17.07.2020
 *      Author: harald.schmittle
 *
 *
 * Defines the data layout in the flash configuration page as well as global system defaults.
 * The overall layout of the flash page is as follows:
 *	[begin]		Fixed (readonly) area (firmware version)
 *  [middle]	Configuration data.
 *  [end]		Fixed (readonly) area (firmware configuration options)
 *
 * The [begin] and [end] areas are only written during production and/or firmware updates.
 * Do *never* overwrite these areas, as they must match the firmware.
 */

#ifndef INC_FLASH_CONFIG_HM_H_
#define INC_FLASH_CONFIG_HM_H_

#include <stdint.h>
#include <he_MSTKN_defines.h>

// Global definitions
#define CFG_EEPROM_PAGE_SIZE	8				// Flash configuration page size is 16 bytes
#define CFG_FLASH_SIZE			256				// Flash configuration size is 256 bytes
#define CFG_MAX_NUM_SOCKETS		8 				// Max. number of allowed sockets for this board -- with this layout max. 13 possible

#define WDT_TIMEOUT 2000000   // 2000 ms.

// for external simulation access: master - master
#define INPUT_PAGE_SIZE  2
#define OUTPUT_PAGE_SIZE 2

// The "Version number" flags
#pragma pack(push, 1)
typedef struct _tVersion
{
    union
	{
        uint32_t val;       // Note: is == 0 or == 0xFFFFFFFF if not configured
        struct {
        	// Bits 0-23: version (major, minor, build/patch)
        	uint32_t    version : 24;
        	// Bits 24-30: reserved (for now)
        	uint32_t    _unused_24 : 1;
        	uint32_t    _unused_25 : 1;
        	uint32_t    _unused_26 : 1;
        	uint32_t    _unused_27 : 1;
        	uint32_t    _unused_28 : 1;
        	uint32_t    _unused_29 : 1;
        	uint32_t    _unused_30 : 1;
        	// Bit 31: set, if wdt enabled
        	uint32_t    wdt_enabled : 1;
        } ;
    };
} tVersion;
#pragma pack(pop)


// The "Bootloader Unique ID" flags
#pragma pack(push, 1)
typedef struct _tBUID
{
    union
	{
        uint32_t val;       // Note: is == 0 or == 0xFFFFFFFF if not configured
        struct {
        	// Bits 0-1: Dispswitch settings
        	uint32_t    dipswitch : 2;
        	// Bit 2-15: 13 bits date/timestamp (number of days since )
        	uint32_t    datestamp : 14;
        	// Bit 16-31: CRC over Chip-ID
        	uint32_t    crc       : 16;
        } ;
    };
} tBUID;
#pragma pack(pop)


// The "network settings" area
#define	CFG_FLASH_NETSETTING_HOSTNAME_LEN	32
#pragma pack(push, 1)
typedef struct _tNetSettings
{
    uint8_t	 IP[4];
    uint8_t	 Gateway[4];
    uint8_t	 Subnet[4];
    char     Hostname[CFG_FLASH_NETSETTING_HOSTNAME_LEN+1];
    union
    {
        uint8_t flags;      
        struct {
        	// Bit 0: DHCP enabled
        	uint8_t    flag_dhcp : 1;		///! @TODO: Brauchen wir das? Ist doch DIP-Schalter...??
          // Bit 1-7: unused
        	uint8_t    flag_bit1 : 1;
        	uint8_t    flag_bit2 : 1;
        	uint8_t    flag_bit3 : 1;
        	uint8_t    flag_bit4 : 1;
        	uint8_t    flag_bit5 : 1;
        	uint8_t    flag_bit6 : 1;
        	uint8_t    flag_bit7 : 1;
        };
    };
} tNetSettings;
#pragma pack(pop)

// Calculate flash block sizes
/*
#define CFG_FLASH_LEN_ROBLK_BEG 		(sizeof(uint32_t))
#define CFG_FLASH_LEN_ROBLK_END			(20)
#define CFG_FLASH_LEN_BLOCK_FILL        (CFG_FLASH_SIZE \
										- CFG_FLASH_LEN_ROBLK_BEG \
										- sizeof(tUserCfgFlags) \
                    - sizeof(tNetSettingsx) \
										- CFG_FLASH_LEN_ROBLK_END)
*/
typedef struct  {
    union {
        uint32_t val;
        struct {
        	uint32_t invalid 	: 1;
        	uint32_t direct_mode: 1;
        	uint32_t reserved  	: 30;
        } ;
    };
} tUserCfgFlags;


/////////////////////////////////////////////////////////////////////////////////////////////
// Calculate block sizes
#define CFG_FIRST_FIXED_BLOCK_SIZE ( \
		sizeof(tVersion) 						/* [4]  tVersion	    Version; */ \
		)
#define CFG_DYNAMIC_BLOCK_SIZE ( \
	    sizeof(unsigned char)*(HE_MSTKN_MAX_SLAVES+1) 	/* [16] unsigned char 	SlavesConfig[SLAVE_NUMBER+1]; */ \
		+ sizeof(unsigned int)*(HE_MSTKN_MAX_SLAVES)  	/* [60] unsigned int  	SlaveUID[SLAVE_NUMBER]; */ \
		+ sizeof(unsigned char)*(HE_MSTKN_MAX_GROUPS) 	/* [30] unsigned char 	Groups[GROUP_SIZE]; */ \
		+ sizeof(tNetSettings) 							/* [45] tNetSettings 	NetSettings; */ \
		)
#define CFG_LAST_STATIC_BLOCK_SIZE ( \
		sizeof(tUserCfgFlags) 					/* [ 4] tUserCfgFlags  	UserCfgFlags; */ \
		+ sizeof(unsigned short) 				/* [ 2] unsigned short 	uVendorID; */ \
		+ sizeof(unsigned short) 				/* [ 2] unsigned short 	uDeviceID; */ \
		+ sizeof(char) 							/* [ 1] char  			Input_Page_Size; */ \
		+ sizeof(char) 							/* [ 1] char  			Output_Page_Size; */ \
		)
#define CFG_LAST_FIXED_BLOCK_SIZE ( \
		sizeof(char)*16		 					/* [16] char  			sTTNR[16]; */ \
	    + sizeof(tBUID)							/* [ 4] tBUID  		    BUID; */ \
		)
// Calculate the unused block size (stub block)
#define CFG_STUB_BLOCK_SIZE ( 					/* --> sollte aktuell 70 sein */ \
		CFG_FLASH_SIZE							/* Overall config page size = 256 bytes */ \
		- CFG_FIRST_FIXED_BLOCK_SIZE 			/* */ \
		- CFG_DYNAMIC_BLOCK_SIZE				/* */ \
		- CFG_LAST_STATIC_BLOCK_SIZE 			/* */ \
		- CFG_LAST_FIXED_BLOCK_SIZE 			/* */ \
		)

// On-Chip flash configuration page layout.
//
#pragma pack(push, 1)
typedef struct _tFlashCFG
{
    // Fixed first block
	//
	tVersion	      	Version;				// Firmware version (0 if never configured)
                                                // MSB = 1 falls watchdog aktiv (sollte so sein!)
	// dynamic data block
    unsigned char 		SlavesConfig[HE_MSTKN_MAX_SLAVES+1];
    unsigned int  		SlaveUID[HE_MSTKN_MAX_SLAVES];
    unsigned char 		Groups[HE_MSTKN_MAX_GROUPS];
    tNetSettings 		NetSettings;

    // stub/filler block
    uint8_t 			_Stub[CFG_STUB_BLOCK_SIZE];

    // last static data block
    tUserCfgFlags  		UserCfgFlags; 			// user settable configuration flags (see mask)
    unsigned short 		uVendorID;
    unsigned short 		uDeviceID;
    char  				Input_Page_Size;
    char  				Output_Page_Size;

    // Fixed device info block - *always* located in the last 20 bytes
    //
    char  				sTTNR[16];  				// Unit order code (Typ-Teilenummer)
    tBUID  		        BUID;						// Unit serial (bits 0-1 = implicit bootloader ID)
} tFlashCFG;
#pragma pack(pop)


#endif /* INC_FLASH_CONFIG_HM_H_ */
