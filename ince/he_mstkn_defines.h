/*
 * he_mstkn_defines.h
 *
 * Generic defines for the MSTKN system
 *
 */

#ifndef INC_HE_MSTKN_DEFINES_H_
#define INC_HE_MSTKN_DEFINES_H_

// System limits
#ifdef __MSTKN_HM_IO_LINK
	#define HE_MSTKN_MAX_SLAVES		7	// max. number of slaves/sensor boards (was SLAVE_NUMBER)
	#define HE_MSTKN_MAX_PLACES		28	// max. number of overall slots (was SLOT_NUMBER)
	#define HE_MSTKN_MAX_GROUPS		15	// max. number of groups (was GROUP_SIZE)
#else
	#define HE_MSTKN_MAX_SLAVES		15	// max. number of slaves/sensor boards (was SLAVE_NUMBER)
	#define HE_MSTKN_MAX_PLACES		60	// max. number of overall slots (was SLOT_NUMBER)
	#define HE_MSTKN_MAX_GROUPS		30	// max. number of groups (was GROUP_SIZE)
#endif

// LED color defines
#define	HE_MSTKN_LED_COLOR_OFF	0	// off
#define	HE_MSTKN_LED_COLOR_GN	1	// green
#define	HE_MSTKN_LED_COLOR_YL	2	// yellow
#define	HE_MSTKN_LED_COLOR_RD	4	// red


// Status values for tHE_MSTKN_CmdByte.cmd
// TODO: change to enum
#define HE_MSTKN_STA_NOCHOICE    	0   // no tool selected
#define HE_MSTKN_STA_ERROR          1   // wrong tool selected
#define HE_MSTKN_STA_OK             2   // correct tool selected
#define HE_MSTKN_STA_SENSOR_ERR     3   // correct tool selected
#define HE_MSTKN_STA_CALIB_STARTED  4   // calibration started
#define HE_MSTKN_STA_CALIB_DONE_OK  6   // calibration done successfully
#define HE_MSTKN_STA_CALIB_DONE_ERR 7   // calibration done with error

// Command values for tHE_MSTKN_CmdByte.cmd
// TODO: change to enum
#define HE_MSTKN_CMD_NORM_MODE   	0 	//  Normaler Betrieb
#define HE_MSTKN_CMD_NOCHOICE    	1 	//  Keine Auswahl
#define HE_MSTKN_CMD_DIRECT_MODE 	2 	//  Direktbetrieb
#define HE_MSTKN_CMD_3_RESERVED  	3 	//  reserviert (momentan als Keine Auswahl)
#define HE_MSTKN_CMD_TEACHING    	4 	//  Kalibrieren
#define HE_MSTKN_CMD_5_RESERVED  	5 	//  reserviert (momentan als Keine Auswahl)
#define HE_MSTKN_CMD_6_RESERVED  	6 	//  reserviert (momentan als Keine Auswahl)
#define HE_MSTKN_CMD_CABLE_TEST  	7 	//  Leitungsbruch (Echo)

// Operation modes
#define HE_MSTKN_OPMODE_INTELLIGENT	0	// "Intelligent" mode
#define HE_MSTKN_OPMODE_DIRECTIO	1	// "DirectIO" mode, control each LED individually

///! @Brief command byte to control the socket tray
#pragma pack(push, 1)
typedef struct _tHE_MSTKN_CmdByte
{
    union
	{
        uint8_t val;
        struct {
        	// Bits 0-2: Command code (see command defines above)
        	uint8_t    	cmd : 3;			///< 3 bits command code
        	// Bits 3-5: reserved
        	uint8_t    	_unused_3 : 1;
        	uint8_t    	_unused_4 : 1;
        	uint8_t    	_unused_5 : 1;
        	// Bit 6,7 OK/NOK-LEDs
        	uint8_t    	led_rd : 1;			///< red led
        	uint8_t    	led_gn : 1;			///< green led
        } ;
    };
} tHE_MSTKN_CmdByte;
#pragma pack(pop)

///! @Brief status byte of the socket tray
#pragma pack(push, 1)
typedef struct _tHE_MSTKN_StaByte
{
    union
	{
        uint8_t val;
        struct {
        	// Bits 0-1: Status code (see status defines above)
        	uint8_t    	status : 3;			///< 3 bits status code
//        	// Bits 2-6: number of sockets/places (only during teaching, must be multiplied by 4 )
//        	uint8_t    	places : 5;		///< 5 bits places count for teaching (must be multiplied by 4)
//        								// this is old, actual number of sockets/places is at the same byte like in the order
//        	// Bit 7: teaching error (only during teaching)
//        	uint8_t    	teach_err : 1;	///< bit 7: =1 if teaching failed
        } ;
    };
} tHE_MSTKN_StaByte;
#pragma pack(pop)

///! @Brief LED control byte
#pragma pack(push, 1)
typedef struct _tHE_MSTKN_LEDByte
{
    union
	{
        uint8_t val;
        struct {
        	// Bits 0-1: Green LED (0 = 0ff, 1 = on, 0/1 = blink)
        	uint8_t    	gn : 2;
        	// Bits 2-3: Yellow LED  (0 = 0ff, 1 = on, 2/3 = blink)
        	uint8_t    	yl : 2;
        	// Bits 4-5: Red LED  (0 = 0ff, 1 = on, 4/5 = blink)
        	uint8_t    	rd : 2;
        	// Bit 6-7: reserved
        	uint8_t    	_unused_6 : 1;
        	uint8_t    	_unused_7 : 1;
        } ;
    };
} tHE_MSTKN_LEDByte;
#pragma pack(pop)


#ifdef __MSTKN_HM_IO_LINK
	///! @Brief command byte to control the socket tray
	#pragma pack(push, 1)
	typedef struct _tHE_MSTKN_CmdByte_IOLink
	{
		union
		{
			uint8_t val;
			struct {
				// Bits 0-2: Command code (see command defines above)
				uint8_t    	cmd : 3;			///< 3 bits command code
				uint8_t		OperationMode : 1;	// 0 = intelligent, 1 = Direct I/O, ...
				// Bits 4-5: reserved
				uint8_t    	_unused_4 : 1;
				uint8_t    	_unused_5 : 1;
				// Bit 6,7 OK/NOK-LEDs
				uint8_t    	led_rd : 1;			///< red led
				uint8_t    	led_gn : 1;			///< green led
			} ;
		};
	} tHE_MSTKN_CmdByte_IOLink;
	#pragma pack(pop)

	///! @Brief status byte of the socket tray
	#pragma pack(push, 1)
	typedef struct _tHE_MSTKN_StaByte_IOLink
	{
		union
		{
			uint8_t val;
			struct {
				// Bits 0-1: Status code (see status defines above)
				uint8_t    	status : 3;			///< 3 bits status code
				uint8_t		OperationMode : 1;	// 0 = intelligent, 1 = Direct I/O, ...
	//        	// Bits 2-6: number of sockets/places (only during teaching, must be multiplied by 4 )
	//        	uint8_t    	places : 5;		///< 5 bits places count for teaching (must be multiplied by 4)
	//        								// this is old, actual number of sockets/places is at the same byte like in the order
	//        	// Bit 7: teaching error (only during teaching)
	//        	uint8_t    	teach_err : 1;	///< bit 7: =1 if teaching failed
			} ;
		};
	} tHE_MSTKN_StaByte_IOLink;
	#pragma pack(pop)

	///! @brief: Internal data structure for "output" data, i.e. LEDs
	///! @note:  This data structure maps 1:1 to the fieldbus data (profinet/wiznet)
	#pragma pack(push, 1)
	typedef struct _tHE_MSTKN_Control_IOLink {
		union {
			struct {
				uint8_t						Preselect;	// #0 - free choice / place #1 /.../ #PlaceNumber)
				tHE_MSTKN_CmdByte_IOLink	Command;	// standard mode, loopback, calibrate, ...
			} Intelligent;
			struct {
				tHE_MSTKN_CmdByte_IOLink	Command;	// Command
				uint8_t	_reserved;						// unused so far
			} DirectIO;
			struct {
			} Data;
		};
		tHE_MSTKN_LEDByte	LEDs[HE_MSTKN_MAX_PLACES];
	} tHE_MSTKN_Control_IOLink;
	#pragma pack(pop)

	///! @brief: Internal data structure for "output" data, i.e. LEDs
	///! @note:  This data structure maps 1:1 to the fieldbus data (profinet/wiznet)
	#pragma pack(push, 1)
	typedef struct _tHE_MSTKN_Status_IOLink {
		union {
			struct {
				uint8_t						Selection;	// #0 - nothing / place #1 /.../ #PlaceNumber)
				tHE_MSTKN_StaByte_IOLink	Status;		// current status, correct selection, wrong select, ...
			} Intelligent;
			struct {
				tHE_MSTKN_StaByte_IOLink	Status;		// Control
				uint8_t	_reserved;						// unused so far
			} DirectIO;
			struct {
			} Data;
		};
		uint8_t			Sensors[HE_MSTKN_MAX_PLACES];
	} tHE_MSTKN_Status_IOLink;
	#pragma pack(pop)
#endif

///! @brief: Internal data structure for "output" data, i.e. LEDs
///! @note:  This data structure maps 1:1 to the fieldbus data (profinet/wiznet)
#pragma pack(push, 1)
typedef struct _tHE_MSTKN_Control {
	struct {
		uint8_t		ExpectedPlaces;			// 0 = use configured, don't check; else 4,8,...60 check against config
		uint8_t		OperationMode;			// 0 = intelligent, 1 = Direct I/O, ...
		uint8_t		_reserved_byte_03;
		uint8_t		_reserved_byte_04;
	} Device;
	union {
		struct {
			uint8_t				Preselect;	// #0 - free choice / place #1 /.../ #PlaceNumber)
			tHE_MSTKN_CmdByte	Command;	// standard mode, loopback, calibrate, ...
		} Intelligent;
		struct {
			tHE_MSTKN_CmdByte	Command;	// Command
			uint8_t	_reserved;				// unused so far
		} DirectIO;
		struct {
		} Data;
	};
	tHE_MSTKN_LEDByte	LEDs[HE_MSTKN_MAX_PLACES];
} tHE_MSTKN_Control;
#pragma pack(pop)

///! @brief: Internal data structure for "output" data, i.e. LEDs
///! @note:  This data structure maps 1:1 to the fieldbus data (profinet/wiznet)
#pragma pack(push, 1)
typedef struct _tHE_MSTKN_Status {
	struct {
		uint8_t		ActualPlaces;			// Bit [7]: Error, Bits [5-0] actual place number (4,8,...60)
		uint8_t		OperationMode;			// Actual operation mode (echo HE_MSTKN_Control.OperationMode)
		uint8_t		_reserved_byte_03;
		uint8_t		_reserved_byte_04;
	} Device;
	union {
		struct {
			uint8_t				Selection;	// #0 - nothing / place #1 /.../ #PlaceNumber)
			tHE_MSTKN_StaByte	Status;		// current status, correct selection, wrong select, ...
		} Intelligent;
		struct {
			tHE_MSTKN_StaByte	Status;		// Control
			uint8_t	_reserved;				// unused so far
		} DirectIO;
		struct {
		} Data;
	};
	uint8_t			Sensors[HE_MSTKN_MAX_PLACES];
} tHE_MSTKN_Status;
#pragma pack(pop)

// slot states
#define HE_MSTKN_SENSOR_NICHT_BELEGT    	0x80
#define HE_MSTKN_SENSOR_BELEGT          	0x81
#define HE_MSTKN_SENSOR_FEHLER1         	0x82
#define HE_MSTKN_SENSOR_FEHLER2         	0x83
#define HE_MSTKN_SENSOR_DEAKTIVIERT     	0x84

#define	HE_MSTKN_MULTIPLE_SELECTION 0xFF

///! @NOTE if an HAL_ERROR occurs, set this error code - we have to REBOOT
#define HE_MSTKN_HAL_ERROR			0xff
///! MODBUS-Collision-Error
#define HE_MSTKN_COL_ERROR			0xf0

#endif /* MSTKN_HE_MSTKN_DEFINES_H_ */
