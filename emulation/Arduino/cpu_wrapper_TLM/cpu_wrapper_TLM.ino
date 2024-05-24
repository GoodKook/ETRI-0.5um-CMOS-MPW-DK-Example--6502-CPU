/*
  CPU_6502's PE SystemC Co-Emulation (FPGA)
  TLM mode
*/

//#define PRINT_MEMORY_RW
#define DELAY_MICROS    1

#define PIN_GET_EMU     26
#define PIN_LOAD_EMU    28
#define PIN_CLK_EMU     29
#define PIN_CLK_DUT     24

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
  pinMode(PIN_GET_EMU  , OUTPUT);   digitalWrite(PIN_GET_EMU  , LOW);
  pinMode(PIN_LOAD_EMU , OUTPUT);   digitalWrite(PIN_LOAD_EMU , LOW);
  pinMode(PIN_CLK_EMU  , OUTPUT);   digitalWrite(PIN_CLK_EMU  , LOW);
  pinMode(PIN_CLK_DUT  , OUTPUT);   digitalWrite(PIN_CLK_DUT  , LOW);

  pinMode(PIN_DATA_IN_0, OUTPUT);   digitalWrite(PIN_DATA_IN_0, LOW);
  pinMode(PIN_DATA_IN_1, OUTPUT);   digitalWrite(PIN_DATA_IN_1, LOW);
  pinMode(PIN_DATA_IN_2, OUTPUT);   digitalWrite(PIN_DATA_IN_2, LOW);
  pinMode(PIN_DATA_IN_3, OUTPUT);   digitalWrite(PIN_DATA_IN_3, LOW);
  pinMode(PIN_DATA_IN_4, OUTPUT);   digitalWrite(PIN_DATA_IN_4, LOW);
  pinMode(PIN_DATA_IN_5, OUTPUT);   digitalWrite(PIN_DATA_IN_5, LOW);
  pinMode(PIN_DATA_IN_6, OUTPUT);   digitalWrite(PIN_DATA_IN_6, LOW);
  pinMode(PIN_DATA_IN_7, OUTPUT);   digitalWrite(PIN_DATA_IN_7, LOW);
  
  pinMode(PIN_ADDR_0, OUTPUT);      digitalWrite(PIN_ADDR_0, LOW);
  pinMode(PIN_ADDR_1, OUTPUT);      digitalWrite(PIN_ADDR_1, LOW);
  pinMode(PIN_ADDR_2, OUTPUT);      digitalWrite(PIN_ADDR_2, LOW);
  pinMode(PIN_ADDR_3, OUTPUT);      digitalWrite(PIN_ADDR_3, LOW);
  pinMode(PIN_ADDR_4, OUTPUT);      digitalWrite(PIN_ADDR_4, LOW);
  pinMode(PIN_ADDR_5, OUTPUT);      digitalWrite(PIN_ADDR_5, LOW);
  pinMode(PIN_ADDR_6, OUTPUT);      digitalWrite(PIN_ADDR_6, LOW);
  pinMode(PIN_ADDR_7, OUTPUT);      digitalWrite(PIN_ADDR_7, LOW);

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
  digitalWrite(LED_BUILTIN, HIGH);
}

// Co-Emulation interface ------------------------------------------------------------
#define N_RX  2 // xxxx|RDY|NMI|IRQ|reset, DI
#define N_TX  4 // AB[15:8], AB[7:0], DO, xxxxxxx|WE

uint8_t rxBuf[N_RX], txBuf[N_TX];

// Write Address to Emulation wrapper
void Write_Address(uint8_t address)
{
  // Address
  digitalWrite(PIN_ADDR_0, (address & 0x01)? HIGH:LOW);
  digitalWrite(PIN_ADDR_1, (address & 0x02)? HIGH:LOW);
  digitalWrite(PIN_ADDR_2, (address & 0x04)? HIGH:LOW);
  digitalWrite(PIN_ADDR_3, (address & 0x08)? HIGH:LOW);
  digitalWrite(PIN_ADDR_4, (address & 0x10)? HIGH:LOW);
  digitalWrite(PIN_ADDR_5, (address & 0x20)? HIGH:LOW);
  digitalWrite(PIN_ADDR_6, (address & 0x40)? HIGH:LOW);
  digitalWrite(PIN_ADDR_7, (address & 0x80)? HIGH:LOW);
}
// Write Data to Emulation wrapper
void Write_Data(uint8_t data)
{
  // Data
  digitalWrite(PIN_DATA_IN_0, (data & 0x01)? HIGH:LOW);
  digitalWrite(PIN_DATA_IN_1, (data & 0x02)? HIGH:LOW);
  digitalWrite(PIN_DATA_IN_2, (data & 0x04)? HIGH:LOW);
  digitalWrite(PIN_DATA_IN_3, (data & 0x08)? HIGH:LOW);
  digitalWrite(PIN_DATA_IN_4, (data & 0x10)? HIGH:LOW);
  digitalWrite(PIN_DATA_IN_5, (data & 0x20)? HIGH:LOW);
  digitalWrite(PIN_DATA_IN_6, (data & 0x40)? HIGH:LOW);
  digitalWrite(PIN_DATA_IN_7, (data & 0x80)? HIGH:LOW);
}
// Read Data from Emulation wrapper
uint8_t Read_Data()
{
  uint8_t ret;
  ret = (digitalRead(PIN_DATA_OUT_0)? 0x01 : 0x00) |
        (digitalRead(PIN_DATA_OUT_1)? 0x02 : 0x00) |
        (digitalRead(PIN_DATA_OUT_2)? 0x04 : 0x00) |
        (digitalRead(PIN_DATA_OUT_3)? 0x08 : 0x00) |
        (digitalRead(PIN_DATA_OUT_4)? 0x10 : 0x00) |
        (digitalRead(PIN_DATA_OUT_5)? 0x20 : 0x00) |
        (digitalRead(PIN_DATA_OUT_6)? 0x40 : 0x00) |
        (digitalRead(PIN_DATA_OUT_7)? 0x80 : 0x00);
  return ret;
}
// Clocking for Emulation wrapper
void Clk_EMU()
{
  // Set
  digitalWrite(PIN_CLK_EMU, HIGH);
  delayMicroseconds(DELAY_MICROS);
  digitalWrite(PIN_CLK_EMU, LOW);
  delayMicroseconds(DELAY_MICROS);
}
// Set Inputs for Emulator
void EMU_Input(uint8_t address, uint8_t data)
{
  Write_Address(address);
  Write_Data(data);
  Clk_EMU();
}
// Get output from Emulator
uint8_t EMU_Output(uint8_t address)
{
  uint8_t ret;
  
  Write_Address(address);
  Clk_EMU();
  ret = Read_Data();
  return ret;
}
// Set Inputs for DUT
void DUT_Input()
{
  digitalWrite(PIN_LOAD_EMU, HIGH);
  Clk_EMU();
  digitalWrite(PIN_LOAD_EMU, LOW);
  Clk_EMU();
}
// Get Outputs from DUT
void DUT_Output()
{
  digitalWrite(PIN_GET_EMU, HIGH);
  Clk_EMU();
  digitalWrite(PIN_GET_EMU, LOW);
  Clk_EMU();
}
// Clocking DUT
void DUT_Posedge_Clk()
{
  digitalWrite(PIN_CLK_DUT, LOW);
  delayMicroseconds(DELAY_MICROS);
  digitalWrite(PIN_CLK_DUT, HIGH);
  delayMicroseconds(DELAY_MICROS);
}

// Transaction between Emulator driver and FPGA wrapper ------------------------------
void RxPacket()
{
  EMU_Input(0, rxBuf[0]);   // xxx|clk|RDY|NMI|IRQ|reset
  EMU_Input(1, rxBuf[1]);   // DI[7:0]

  DUT_Input();              // Set input vectors to DUT
  delayMicroseconds(DELAY_MICROS);

  //DUT_Posedge_Clk();        // DUT's Posedge-Clock
  if (rxBuf[0] & 0x10) 
    digitalWrite(PIN_CLK_DUT, HIGH);
  else
    digitalWrite(PIN_CLK_DUT, LOW);

  return;
}

void TxPacket()
{
  DUT_Output(); // Capture DUT's output
  delayMicroseconds(DELAY_MICROS);

  txBuf[0] = EMU_Output(0); // AB[15:8]
  txBuf[1] = EMU_Output(1); // AB[7:0]
  txBuf[2] = EMU_Output(2); // DO[7:0]
  txBuf[3] = EMU_Output(3); // xxxxxxx|WE

  return;
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

#define BIN_FILE_LOAD 0xD018

volatile uint8_t Memory[65536]; // Well, 6502 has 64Kbyte of memory space

uint16_t  Mem_Addr[2], memIn_Addr;
uint8_t   counter, memOut, memIn_Data, rxByte;
bool      bMemory_WE = false, bKeyboard = false;

void loop()
{
#include "wozmon.bin.h" // Apple-1 Wozniak's System Monitor

  Machine_Clock_Rise(RESET);
  Machine_Clock_Fall(RESET);
  Machine_Clock_Rise(RESET);
  Machine_Clock_Fall(RESET);
  Machine_Clock_Rise(RESET);
  Machine_Clock_Fall(RESET);

  while(true)
  {
    Machine_Clock_Rise(0);
    Machine_Clock_Fall(0);
  }
}

void Machine_Clock_Rise(uint8_t Ctrl)
{
  // Monitoring -------------------------------------------------
  counter += 1;
  digitalWrite(LED_BUILTIN, (counter & 0x80)? HIGH:LOW);

  // Defered Write to Memory --------------------------------------
  if (bMemory_WE)
  {
    Memory[memIn_Addr] = memIn_Data;
    Print_Memory_RW(memIn_Addr, memIn_Data, false, false, true);
    if (memIn_Addr==PIA_DSP_REG)
    {
      while(true)
      {
        if (Serial.availableForWrite() >= 1)
        {
          Serial.write((int)(memIn_Data & 0x7F));
          Memory[PIA_DSP_REG] = memIn_Data & 0x7F;  // Clear Display
          break;
        }
      }
    }
    bMemory_WE = false;
  }

  // Check if keyboard input --------------------------------------
  if (Serial.available() >= 1)
  {
    rxByte = (uint8_t)Serial.read();
    bKeyboard = true;
  }

  // Read memory ----------------------------------------------------
  if (Mem_Addr[1]==PIA_KBD_REG) // CPU accessing Keyboard peripheral
  {
    Memory[PIA_KBD_REG] = (rxByte | 0x80);              // Valid input
    Memory[PIA_KBD_CTL] = (Memory[PIA_KBD_CTL] & 0x3F);	// Keyboard empty
    bKeyboard = false;
  }
  else if (Mem_Addr[1]==PIA_KBD_CTL)  // CPU accessing display peripheral
  {
    if (bKeyboard)
      Memory[PIA_KBD_CTL] = (Memory[PIA_KBD_CTL] | 0x80);	// Keyboard ready
  }
  else if (Mem_Addr[1]==BIN_FILE_LOAD)
  {
    Memory[BIN_FILE_LOAD] = Download_CC65_Bin();
  }

  memOut = Memory[Mem_Addr[1]];  // DI to CPU
  Print_Memory_RW(Mem_Addr[1], memOut, false, false, false);

  // Pose-edge DUT clock -----------------------------------------------
  rxBuf[0] = CLK_HI|RDY_HI|Ctrl;
  rxBuf[1] = memOut;  // DI to CPU

  RxPacket();
  delayMicroseconds(DELAY_MICROS);
  TxPacket();
  delayMicroseconds(DELAY_MICROS);

  Mem_Addr[1] = Mem_Addr[0];
  Mem_Addr[0] = (uint16_t)txBuf[0]<<8|(uint16_t)txBuf[1];
  //Print_Memory_RW(Mem_Addr[0], txBuf[2], true, false, false);

  if (txBuf[3] & 0x01)  // Write Enable?
  {
    bMemory_WE = true;  // for deffered write
    memIn_Data = txBuf[2];
    memIn_Addr = Mem_Addr[0];
  }
}

void Machine_Clock_Fall(uint8_t Ctrl)
{
  // Read memory ----------------------------------------------------
  memOut = Memory[Mem_Addr[1]];  // DI to CPU

  // Neg-edge DUT clock -----------------------------------------------
  rxBuf[0] = CLK_LO|RDY_HI|Ctrl;
  rxBuf[1] = memOut;  // DI to CPU

  RxPacket();
  delayMicroseconds(DELAY_MICROS);
  TxPacket();
  delayMicroseconds(DELAY_MICROS);

  Mem_Addr[0] = (uint16_t)txBuf[0]<<8|(uint16_t)txBuf[1];
  memIn_Data = txBuf[2];
  memIn_Addr = Mem_Addr[0];
  //Print_Memory_RW(Mem_Addr[0], txBuf[2], false, true, false);
}

// Monitor Memory access for Debugging purpose -----------------------------------------
void Print_Memory_RW(uint16_t Address, uint8_t Data, bool bRise, bool bFall, bool bWr)
{
#ifndef PRINT_MEMORY_RW
  delayMicroseconds(DELAY_MICROS);
  return;
#endif

  int txByte;
  char szBuff[16];

  if (bRise)
    sprintf(szBuff, "^%04X]%02X ", Address, Data);
  else if (bFall)
    sprintf(szBuff, "v%04X]%02X ", Address, Data);
  else
    sprintf(szBuff, "*%04X]%02X ", Address, Data);
  if (bWr)
    szBuff[5] = '<';
  else
    szBuff[5] = '>';

  while(true)
  {
    if (Serial.availableForWrite() >= strlen(szBuff))
    {
      for(int i=0; i<strlen(szBuff); i++)
      {
        txByte = (int)szBuff[i];
        Serial.write(txByte);
      }
      break;
    }
  }
}
//--------------------------------------------------------------------
uint8_t Download_CC65_Bin()
{
  uint16_t nAddress = 0, nLength = 0;
  uint8_t binBuff[4];

  // Read first 4 bytes for Start Address & Length
  binBuff[0] = GetEchoByte();
  binBuff[1] = GetEchoByte();
  binBuff[2] = GetEchoByte();
  binBuff[3] = GetEchoByte();

  nAddress = (unsigned int)binBuff[1] * 256 + (unsigned int)binBuff[0];
  nLength  = (unsigned int)binBuff[3] * 256 + (unsigned int)binBuff[2];

  for (int i=4; i<nLength; i++)
  {
    Memory[nAddress] = GetEchoByte();
    nAddress++;
  }

  // Set Reset vector
  Memory[0] = 0x20;  // JSR  $StartUp
  Memory[1] = binBuff[0];
  Memory[2] = binBuff[1];
  Memory[3] = 0x20;  // JSR  $FF00
  Memory[4] = 0x00;
  Memory[5] = 0xFF;

  return 1;
}

uint8_t GetEchoByte()
{
  uint8_t rxByte;

  // Wait for something arrived on serial port
  while(Serial.available() <= 0)          delay(1);
  rxByte = (uint8_t)Serial.read();  // Read
  // Wait for something to send via serial port
  while(Serial.availableForWrite() <= 0)  delay(1);
  Serial.write((int)rxByte);        // Echo

  return rxByte;
}
