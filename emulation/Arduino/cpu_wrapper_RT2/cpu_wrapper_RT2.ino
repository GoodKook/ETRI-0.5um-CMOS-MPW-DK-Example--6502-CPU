/*
  Poorman's Standard-Emulator by GoodKook, goodkook@gmail.com
  PSCE Model Interface: SystemC to/from DUT Emulator
*/

// Standard Emulator ------------------------------------------------
#include "PSCE_APIs.h"

// Co-Emulation interface -------------------------------------------
// Followings are DUT specific defs
#define DELAY_MICROS    10

#define N_RX            6   // Number of byte to DUT's inputs
#define N_TX            4   // Number of byte from DUT's output

#define DUT_CLK_BYTE    0
#define DUT_CLK_BITMAP  0x00

PSCE psce(DELAY_MICROS);

void setup()
{
  psce.init();  // Overclocking=114Mhz, BPS=115200

  // PWM Set-up on pin: DAC1
  REG_PMC_PCER1 |= PMC_PCER1_PID36;                     // Enable PWM 
  REG_PIOB_ABSR |= PIO_ABSR_P16;                        // Set PWM pin perhipheral type A or B, in this case B
  REG_PIOB_PDR |= PIO_PDR_P16;                          // Set PWM pin to an output
  REG_PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(1);      // Set the PWM clock rate to 84MHz (84MHz/1) 
  REG_PWM_CMR0 = PWM_CMR_CPRE_CLKA;                     // Enable single slope PWM and set the clock source as CLKA
  REG_PWM_CPRD0 = 2;                                  // Set the PWM frequency 84MHz/40kHz = 2100 
  REG_PWM_CDTY0 = 1;                                  // Set the PWM duty cycle 50% (2100/2=1050)
  REG_PWM_ENA = PWM_ENA_CHID0;                          // Enable the PWM channel     
}

void loop()
{
  psce.EMU_Blinker(0x40);   // Blinker speed
  psce.RxPacket(N_RX, DUT_CLK_BYTE, DUT_CLK_BITMAP);  // CLK position: 0-th byte & 0x40
  psce.TxPacket(N_TX);
}
