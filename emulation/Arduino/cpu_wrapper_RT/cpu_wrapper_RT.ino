/*
  CPU_6502's PE SystemC Co-Emulation (FPGA)
  Real-Time mode (Memory Embedded)
*/
#if defined(__GNUC__)
#pragma GCC optimize ("Ofast")
#pragma GCC optimize ("inline-functions")
#endif

inline void digitalWriteDirect(int pin, boolean val)
{
  if(val) g_APinDescription[pin].pPort -> PIO_SODR = g_APinDescription[pin].ulPin;
  else    g_APinDescription[pin].pPort -> PIO_CODR = g_APinDescription[pin].ulPin;
}

inline int digitalReadDirect(int pin)
{
  return !!(g_APinDescription[pin].pPort -> PIO_PDSR & g_APinDescription[pin].ulPin);
}

//#define PRINT_MEMORY_RW
//#define DELAY_MICROS    1

#define PIN_GET_EMU     26
#define PIN_LOAD_EMU    28
#define PIN_CLK_EMU     29
#define PIN_CLK_DUT     24
#define PIN_IO_REQ      27

#define PIN_DATA_IN_0   36
#define PIN_DATA_IN_1   34
#define PIN_DATA_IN_2   32
#define PIN_DATA_IN_3   30
#define PIN_DATA_IN_4   37
#define PIN_DATA_IN_5   35
#define PIN_DATA_IN_6   33
#define PIN_DATA_IN_7   31

#define PIN_DATA_OUT_0  44
#define PIN_DATA_OUT_1  42
#define PIN_DATA_OUT_2  40
#define PIN_DATA_OUT_3  38
#define PIN_DATA_OUT_4  45
#define PIN_DATA_OUT_5  43
#define PIN_DATA_OUT_6  41
#define PIN_DATA_OUT_7  39

#define PIN_ADDR_0      52
#define PIN_ADDR_1      50
#define PIN_ADDR_2      48
#define PIN_ADDR_3      46
#define PIN_ADDR_4      53
#define PIN_ADDR_5      51
#define PIN_ADDR_6      49
#define PIN_ADDR_7      47

//--------------------------------------------------------------------
void establishContact()
{
  while (Serial.available() <= 0)
  {
    Serial.print('A');  // send a capital A
    delay(300);
    if (Serial.read()==(int)'A')
      break;
  }
}
//------------------------------------------------------------------------------
void setup()
{
  // start serial port at 38400 bps:
  Serial.begin(38400);
  while (!Serial)
  {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  establishContact();  // send a byte to establish contact until receiver responds

  // Set digital pins to output connecting FPGA's INPUT
  pinMode(PIN_GET_EMU  , OUTPUT);   digitalWriteDirect(PIN_GET_EMU  , LOW);
  pinMode(PIN_LOAD_EMU , OUTPUT);   digitalWriteDirect(PIN_LOAD_EMU , LOW);
  pinMode(PIN_CLK_EMU  , OUTPUT);   digitalWriteDirect(PIN_CLK_EMU  , LOW);
  pinMode(PIN_CLK_DUT  , OUTPUT);   digitalWriteDirect(PIN_CLK_DUT  , LOW);
  pinMode(PIN_IO_REQ   , INPUT);

  pinMode(PIN_DATA_IN_0, OUTPUT);   digitalWriteDirect(PIN_DATA_IN_0, LOW);
  pinMode(PIN_DATA_IN_1, OUTPUT);   digitalWriteDirect(PIN_DATA_IN_1, LOW);
  pinMode(PIN_DATA_IN_2, OUTPUT);   digitalWriteDirect(PIN_DATA_IN_2, LOW);
  pinMode(PIN_DATA_IN_3, OUTPUT);   digitalWriteDirect(PIN_DATA_IN_3, LOW);
  pinMode(PIN_DATA_IN_4, OUTPUT);   digitalWriteDirect(PIN_DATA_IN_4, LOW);
  pinMode(PIN_DATA_IN_5, OUTPUT);   digitalWriteDirect(PIN_DATA_IN_5, LOW);
  pinMode(PIN_DATA_IN_6, OUTPUT);   digitalWriteDirect(PIN_DATA_IN_6, LOW);
  pinMode(PIN_DATA_IN_7, OUTPUT);   digitalWriteDirect(PIN_DATA_IN_7, LOW);
  
  pinMode(PIN_ADDR_0, OUTPUT);      digitalWriteDirect(PIN_ADDR_0, LOW);
  pinMode(PIN_ADDR_1, OUTPUT);      digitalWriteDirect(PIN_ADDR_1, LOW);
  pinMode(PIN_ADDR_2, OUTPUT);      digitalWriteDirect(PIN_ADDR_2, LOW);
  pinMode(PIN_ADDR_3, OUTPUT);      digitalWriteDirect(PIN_ADDR_3, LOW);
  pinMode(PIN_ADDR_4, OUTPUT);      digitalWriteDirect(PIN_ADDR_4, LOW);
  pinMode(PIN_ADDR_5, OUTPUT);      digitalWriteDirect(PIN_ADDR_5, LOW);
  pinMode(PIN_ADDR_6, OUTPUT);      digitalWriteDirect(PIN_ADDR_6, LOW);
  pinMode(PIN_ADDR_7, OUTPUT);      digitalWriteDirect(PIN_ADDR_7, LOW);

  // Set digital pins to input connecting FPGA's OUTPUT
  pinMode(PIN_DATA_OUT_0, INPUT);
  pinMode(PIN_DATA_OUT_1, INPUT);
  pinMode(PIN_DATA_OUT_2, INPUT);
  pinMode(PIN_DATA_OUT_3, INPUT);
  pinMode(PIN_DATA_OUT_4, INPUT);
  pinMode(PIN_DATA_OUT_5, INPUT);
  pinMode(PIN_DATA_OUT_6, INPUT);
  pinMode(PIN_DATA_OUT_7, INPUT);

  // Monitoring LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWriteDirect(LED_BUILTIN, HIGH);
}

// Co-Emulation interface ------------------------------------------------------------
#define N_RX  2 // xxxx|RDY|NMI|IRQ|reset, DI
#define N_TX  4 // AB[15:8], AB[7:0], DO, xxxxxxx|WE

uint8_t rxBuf[N_RX], txBuf[N_TX];

// Write Address to Emulation wrapper
inline void Write_Address(uint8_t address)
{
  // Address
  digitalWriteDirect(PIN_ADDR_0, address & 0x01);
  digitalWriteDirect(PIN_ADDR_1, address & 0x02);
//  digitalWriteDirect(PIN_ADDR_2, address & 0x04);
//  digitalWriteDirect(PIN_ADDR_3, address & 0x08);
//  digitalWriteDirect(PIN_ADDR_4, address & 0x10);
//  digitalWriteDirect(PIN_ADDR_5, address & 0x20);
//  digitalWriteDirect(PIN_ADDR_6, address & 0x40);
//  digitalWriteDirect(PIN_ADDR_7, address & 0x80);
}
// Write Data to Emulation wrapper
inline void Write_Data(uint8_t data)
{
  // Data
  digitalWriteDirect(PIN_DATA_IN_0, data & 0x01);
  digitalWriteDirect(PIN_DATA_IN_1, data & 0x02);
  digitalWriteDirect(PIN_DATA_IN_2, data & 0x04);
  digitalWriteDirect(PIN_DATA_IN_3, data & 0x08);
  digitalWriteDirect(PIN_DATA_IN_4, data & 0x10);
  digitalWriteDirect(PIN_DATA_IN_5, data & 0x20);
  digitalWriteDirect(PIN_DATA_IN_6, data & 0x40);
  digitalWriteDirect(PIN_DATA_IN_7, data & 0x80);
}
// Read Data from Emulation wrapper
inline uint8_t Read_Data()
{
  uint8_t ret;
  ret = (digitalReadDirect(PIN_DATA_OUT_0)? 0x01 : 0x00) |
        (digitalReadDirect(PIN_DATA_OUT_1)? 0x02 : 0x00) |
        (digitalReadDirect(PIN_DATA_OUT_2)? 0x04 : 0x00) |
        (digitalReadDirect(PIN_DATA_OUT_3)? 0x08 : 0x00) |
        (digitalReadDirect(PIN_DATA_OUT_4)? 0x10 : 0x00) |
        (digitalReadDirect(PIN_DATA_OUT_5)? 0x20 : 0x00) |
        (digitalReadDirect(PIN_DATA_OUT_6)? 0x40 : 0x00) |
        (digitalReadDirect(PIN_DATA_OUT_7)? 0x80 : 0x00);
  return ret;
}
// Clocking for Emulation wrapper
inline void Clk_EMU()
{
  // Set
  digitalWriteDirect(PIN_CLK_EMU, HIGH);
  //delayMicroseconds(DELAY_MICROS);
  digitalWriteDirect(PIN_CLK_EMU, LOW);
  //delayMicroseconds(DELAY_MICROS);
}
// Set Inputs for Emulator
inline void EMU_Input(uint8_t address, uint8_t data)
{
  Write_Address(address);
  Write_Data(data);

  Clk_EMU();
}
// Get output from Emulator
inline uint8_t EMU_Output(uint8_t address)
{
  uint8_t ret;
  
  Write_Address(address);

  Clk_EMU();

  ret = Read_Data();

  return ret;
}
// Set Inputs for DUT
inline void DUT_Input()
{
  digitalWriteDirect(PIN_LOAD_EMU, HIGH);
  Clk_EMU();
  digitalWriteDirect(PIN_LOAD_EMU, LOW);
  Clk_EMU();
}
// Get Outputs from DUT
inline void DUT_Output()
{
  digitalWriteDirect(PIN_GET_EMU, HIGH);
  Clk_EMU();
  digitalWriteDirect(PIN_GET_EMU, LOW);
  Clk_EMU();
}

// DUT Specific defs. ----------------------------------------------
#define CLK_HI  0x10
#define CLK_LO  0x00
#define RDY_HI  0x08
#define MNI_HI  0x04
#define IRQ_HI  0x02
#define RESET   0x01

#define PIA_KBD_REG 0xD010
#define PIA_KBD_CTL 0xD011
#define PIA_DSP_REG 0xD012
#define PIA_DSP_CTL 0xD013

uint16_t  Mem_Addr;
uint8_t   counter, kbd_reg, dsp_reg, Mem_Din, Mem_Dout, Mem_WE;

void Machine_Reset(bool bReset)
{
  if (bReset) EMU_Input(0, (RDY_HI|RESET));
  else        EMU_Input(0, (RDY_HI));
  EMU_Input(1, 0x00);   // DI[7:0]
  DUT_Input();

  digitalWriteDirect(PIN_CLK_DUT, HIGH);
  digitalWriteDirect(PIN_CLK_DUT, LOW);
}

void Machine_Clock_Cycle()
{
  digitalWriteDirect(PIN_CLK_DUT, HIGH);
  DUT_Output(); // Capture DUT's output

  Mem_Addr  = (uint16_t)EMU_Output(0)<<8; // AB[15:8]
  Mem_Addr |= (uint16_t)EMU_Output(1);    // AB[7:0]

  if (Mem_Addr==PIA_KBD_REG)
  {
    Mem_Dout  = EMU_Output(2);              // DO[7:0]
    Mem_WE    = EMU_Output(3) & 0x01;       // xxxxxxx|WE
    if (Mem_WE)                     kbd_reg = Mem_Dout;
    else
    {
      if (Serial.available() >= 1)  kbd_reg = (uint8_t)Serial.read() | 0x80;  // Valid Keyboard Input
      else                          kbd_reg = kbd_reg & 0x7F;                 // No Keyboard input
    }
    Mem_Din = kbd_reg;
  }
  else if (Mem_Addr==PIA_KBD_CTL)
  {
    Mem_Dout  = EMU_Output(2);              // DO[7:0]
    Mem_WE    = EMU_Output(3) & 0x01;       // xxxxxxx|WE
    if (Mem_WE)                     kbd_reg = Mem_Dout;
    else                            kbd_reg = kbd_reg & 0x7F;  // No Keyboard input
    Mem_Din = kbd_reg;
  }
  else if (Mem_Addr==PIA_DSP_REG)
  {
    Mem_Dout  = EMU_Output(2);              // DO[7:0]
    Mem_WE    = EMU_Output(3) & 0x01;       // xxxxxxx|WE
    if (Mem_WE)
    {
      while(Serial.availableForWrite() <= 0)  delay(1);
      Serial.write((int)Mem_Dout & 0x7F);
      dsp_reg = Mem_Dout & 0x7F;
    }
    Mem_Din = dsp_reg;
  }
  else if (Mem_Addr==PIA_DSP_CTL)
  {
    Mem_Dout  = EMU_Output(2);              // DO[7:0]
    Mem_WE    = EMU_Output(3) & 0x01;       // xxxxxxx|WE
    if (Mem_WE) dsp_reg = Mem_Dout;
    Mem_Din = dsp_reg;
  }
  EMU_Input(1, Mem_Din); // DI[7:0]
  DUT_Input();           // Set input vectors to DUT

  digitalWriteDirect(PIN_CLK_DUT, LOW);
  delayMicroseconds(5);
}

void loop()
{
  Machine_Reset(true);
  Machine_Reset(true);
  Machine_Reset(true);
  Machine_Reset(true);
  Machine_Reset(false);

  while(true)
  {
    Machine_Clock_Cycle();
  }
  //while(true)
  //{
  //  digitalWriteDirect(PIN_CLK_DUT, HIGH);
  //  digitalWriteDirect(PIN_CLK_DUT, LOW);
  //}
}
