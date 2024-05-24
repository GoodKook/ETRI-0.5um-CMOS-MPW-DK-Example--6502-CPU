/*
  CPU_6502's PE SystemC Co-Emulation (FPGA)
*/

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
  // start serial port at 9600 bps:
  Serial.begin(115200);
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

#define N_RX  2 // xxxx|RDY|NMI|IRQ|reset, DI
#define N_TX  4 // AB[15:8], AB[7:0], DO, xxxxxxx|WE

uint8_t rxBuf[N_RX], txBuf[N_TX];

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

void Clk_EMU()
{
  // Set
  digitalWrite(PIN_CLK_EMU, HIGH);
  delayMicroseconds(DELAY_MICROS);
  digitalWrite(PIN_CLK_EMU, LOW);
  delayMicroseconds(DELAY_MICROS);
}

void EMU_Input(uint8_t address, uint8_t data)
{
  Write_Address(address);
  Write_Data(data);
  Clk_EMU();
}

void DUT_Input()
{
  digitalWrite(PIN_LOAD_EMU, HIGH);
  Clk_EMU();
  digitalWrite(PIN_LOAD_EMU, LOW);
  Clk_EMU();
}

void DUT_Posedge_Clk()
{
  digitalWrite(PIN_CLK_DUT, LOW);
  delayMicroseconds(DELAY_MICROS);
  digitalWrite(PIN_CLK_DUT, HIGH);
  delayMicroseconds(DELAY_MICROS);
}

void DUT_Output()
{
  digitalWrite(PIN_GET_EMU, HIGH);
  Clk_EMU();
  digitalWrite(PIN_GET_EMU, LOW);
  Clk_EMU();
}

uint8_t EMU_Output(uint8_t address)
{
  uint8_t ret;
  
  Write_Address(address);
  Clk_EMU();
  ret = Read_Data();
  return ret;
}

void RxPacket()
{
  int rxByte;

  while(true)
  {
    if (Serial.available() >= N_RX)
    {
      for(int i=0; i<N_RX; i++)
      {
        rxByte = Serial.read();
        rxBuf[i] = (uint8_t)rxByte;
      }

      EMU_Input(0, rxBuf[0]);   // xxxx|RDY|NMI|IRQ|reset
      EMU_Input(1, rxBuf[1]);   // DI[7:0]

      DUT_Input();              // Set input vectors to DUT
      //DUT_Posedge_Clk();        // DUT's Posedge-Clock
      if (rxBuf[0] & 0x10) 
        digitalWrite(PIN_CLK_DUT, HIGH);
      else
        digitalWrite(PIN_CLK_DUT, LOW);
      return;
    }
  }
}

void TxPacket()
{
  int txByte;

  while(1)
  {
    if (Serial.availableForWrite() >= N_TX)
    {
      DUT_Output(); // Capture DUT's output
        
      txBuf[0] = EMU_Output(0); // AB[15:8]
      txBuf[1] = EMU_Output(1); // AB[7:0]
      txBuf[2] = EMU_Output(2); // DO[7:0]
      txBuf[3] = EMU_Output(3); // xxxxxxx|WE

      for(int i=0; i<N_TX; i++)
      {
        txByte = (int)txBuf[i];
        Serial.write(txByte);
      }

      return;
    }
  }
}

uint8_t counter;

void loop()
{
  counter += 1;
  digitalWrite(LED_BUILTIN, (counter & 0x10)? HIGH:LOW);

  RxPacket();
  TxPacket();
}
