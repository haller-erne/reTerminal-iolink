/**
 * leds.h
 *
 * This defines an interface for MSTKN system common indicator LEDs for
 * main modules. The actual implementation of the functions must be done
 * in the board specific code.
 *
 * Created on: 15.04.2019
 *      Author: harald.schmittle
 */
#ifndef LEDS_H_
#define LEDS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>

enum  clLED        { LED_BLIND = 0, LED_GREEN = 1, LED_YELLOW = 2, LED_RED = 4};
enum  clbValueType { VAL_NORM = 0, VAL_EMPTY = 1, VAL_MIN = 2 , VAL_TOOL = 3 , VAL_RAW = 4};
enum               { xx = 0, xG, Gx, GG, RR, Rx, xR, RG = 9, GR = 10 };
#define F(x,y) ((x<<4)|y)


void I2C_SetOkNokLeds(uint8_t color);

/** Set the Ok/Nok main module state LEDs (if available).
 *
 *  Note, that not all main modules might have the Ok/Nok LEDs, but
 *  this function must be implemented anyway (st least as an empty stub).
 *
 *  The parameter values are 0 = off, 1 = R, 2 = G, 3 = R+G
 *  @param OkNok LED color for the main modules red/green LED.
 */
void leds_SetOkNok(uint8_t OkNok);

//void SetNetworkLeds(void);
//void SetOkNokLeds(uint8_t color);
/** Set the modules two diagnostic bicolor LED states 
 *
 *  All MSTKN main modules have two bicolor LEDs to indicate the system
 *  status - the actual implementation of this functions must be provided
 *  in the board specific code, as the hardware is different per board.
 *
 *  Note that this function must be called cyclically (best is at least
 *  every few milliseconds), as some board multiplex the red/green lines
 *  (which is also the reason, that mixed colors are not possible).
 *
 *  The parameter values are xx, xG, Gx, GG, RR, Rx, xR, RG, GR, they
 *  allow blinking, but no color mixing (both red and green cannot be on
 *  at the same time).
 *  @param led1 LED color for LED1
 *  @param led2 LED color for LED2
 */
void leds_SetDiagnose(uint8_t led1, uint8_t led2);


/** Set the Ethernet link state LEDs (only for Ethernet based devices).
 *
 *  Note, that this function is only implemented/used for devices with
 *  an ethernet interface (obviously), but defined here nevertheless. The
 *  compiler/linker will strip the definition, if it is not used in the 
 *  code.
 *
 *  The parameter values are 0 = off, 1 = Y, 2 = G, 3 = Y+G
 *  @param LinkA LED color for first port
 *  @param LinkB LED color for second port
 */
void leds_SetLinkState(uint8_t LinkA, uint8_t LinkB);


/** Set the Module and Network state LEDs for HMS NP30/NP40 modules
 *
 *  Note, that this function is only implemented/used for devices with
 *  a HMS interface module (obviously), but defined here nevertheless. The
 *  compiler/linker will strip the definition, if it is not used in the 
 *  code.
 *
 *  The parameter values are 0 = off, 1 = R, 2 = G, 3 = R+G
 *  @param MS LED color for module status LED
 *  @param NS LED color for network status LED
 */
void leds_SetMSNS(uint8_t MS, uint8_t NS);


/** Run a lamp test (usually during startup)
 *
 *  The function should show all possible states of the boards LEDs to
 *  visually check during startup, that all LEDs work.
 */
void leds_LampTest();


#ifdef __cplusplus
}
#endif

#endif /* LEDS_H_ */
