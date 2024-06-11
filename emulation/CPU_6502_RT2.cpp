/*
    CPU_6502_SA

    CPU_6502 Stand-Alone FPGA Emulator
    
    g++ -o CPU_6502_SA CPU_6502_RT2.cpp
*/
#include <stdio.h>
#include <stdlib.h>

// Includes for accessing Arduino via serial port
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h> // For FIONREAD

#include <dirent.h>

//------------------------------------------------------------------------
// DUT interface,
//  See DUT's wrapper Verilog
//
// Inputs to DUT (6-Bytes)
//  [0] = {--|Dsp_Rd|Kbd_Wr|RDY|NMI|IRQ|reset}
//  [1] = Kbd_In
// Memory Emulation
//  [2] = {-----|Mem_Emu_Clk|Mem_Emu_Wen|Mem_Emu_Ena}
//  [3] = Mem_Emu_Adr[15:8]
//  [4] = Mem_Emu_Adr[ 7:0]
//  [5] = Mem_Emu_Din
// Outputs from DUT (5-bytes)
//  [0]  <= Dsp_Reg_Out;
//  [1]  <= Kbd_Reg_Out;
//  [2]  <= Kbd_Ctl_Out;
//  [3]  <= Mem_Emu_Dout

#define N_TX    6
#define N_RX    4

#define DSP_RD  0x20
#define KBD_WR  0x10
#define CPU_RDY 0x08
#define CPU_NMI 0x04
#define CPU_IRQ 0x02
#define CPU_RST 0x01

// Arduino Serial IF
int fd;                 // Serial port file descriptor
struct termios options; // Serial port setting

int ConnectEmulator()
{
    // ------------------------------------------------------------
    // Init COM port to communicate Arduino Emulator 
    //fd = open("/dev/ttyACM0", O_RDWR | O_NDELAY | O_NOCTTY);
    fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror("Error opening serial port");
        return -1;
    }

    // Set up serial port/B38400/B115200
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;

    // Apply the settings
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);

    // Establish Contact
    int len = 0;
    unsigned char rx, tx[4];
    
    while(!len)
        len = read(fd, &rx, 1);
    if (rx=='A')
        write(fd, &rx, 1);
    printf("Connection established...\n");
    
    return 1;
}

void TransactEmulator(unsigned char* txPacket, unsigned char* rxPacket)
{
    unsigned char x, y;

    for (int i=0; i<N_TX; i++)
    {
        x = txPacket[i];
        while(write(fd, &x, 1)<=0)  usleep(1);
    }

    for (int i=0; i<N_RX; i++)
    {
        while(read(fd, &y, 1)<=0)   usleep(1);
        rxPacket[i] = y;
    }
}

void CPU_Resetting()
{
    unsigned char txPack[N_TX], rxPack[N_RX];

    for (int i=0; i<N_TX; i++)  txPack[i] = 0x00;
    for (int i=0; i<N_RX; i++)  rxPack[i] = 0x00;

    txPack[0] = CPU_RDY | CPU_RST | DSP_RD | KBD_WR;
    TransactEmulator(txPack, rxPack);
    usleep(1);

    txPack[0] = CPU_RDY | DSP_RD | KBD_WR;
    TransactEmulator(txPack, rxPack);
    usleep(1);
}

unsigned char CPU_ReadDisplay()
{
    unsigned char txPack[N_TX], rxPack[N_RX];

    for (int i=0; i<N_TX; i++)  txPack[i] = 0x00;
    for (int i=0; i<N_RX; i++)  rxPack[i] = 0x00;

    txPack[0] = CPU_RDY | DSP_RD | KBD_WR;
    TransactEmulator(txPack, rxPack);
    
    return rxPack[0];
}

void CPU_AckDisplay()
{
    unsigned char txPack[N_TX], rxPack[N_RX];

    for (int i=0; i<N_TX; i++)  txPack[i] = 0x00;
    for (int i=0; i<N_RX; i++)  rxPack[i] = 0x00;

    txPack[0] = CPU_RDY | KBD_WR;
    TransactEmulator(txPack, rxPack);
    
    usleep(1);
    
    txPack[0] = CPU_RDY | DSP_RD | KBD_WR;
    TransactEmulator(txPack, rxPack);
}

void CPU_SendKeyboard(unsigned char KbdBuff)
{
    unsigned char txPack[N_TX], rxPack[N_RX];

    for (int i=0; i<N_TX; i++)  txPack[i] = 0x00;
    for (int i=0; i<N_RX; i++)  rxPack[i] = 0x00;

    txPack[0] = CPU_RDY | DSP_RD;
    TransactEmulator(txPack, rxPack);
    
    usleep(1);
    
    txPack[0] = CPU_RDY | DSP_RD | KBD_WR;
    txPack[1] = KbdBuff | 0x80;
    TransactEmulator(txPack, rxPack);
}
//------------------------------------------------------------------------

int kbhit(void)
{
    static bool initflag = false;
    static const int STDIN = 0;

    if (!initflag)
    {
        // Use termios to turn off line buffering
        struct termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        term.c_lflag &= ~ECHO;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initflag = true;
    }

    int nbbytes;
    ioctl(STDIN, FIONREAD, &nbbytes);  // 0 is STDIN
    return nbbytes;
}

void PrintHelp()
{
    printf("CPU_6502 Stand-Alone\n");
    printf("====================\n");
    printf("\t- Wozniak's System Monitor Loaded\n");
    printf("\t- Monitor command must be upper case charactor\n");
    printf("\t- Test Program: Type as follows,\n");
    printf("\t      0:A9 0 AA 20 EF FF E8 8A 4C 2 0(RET)\n");
    printf("\t      0.A(RET)\n");
    printf("\t      R(RET)\n");
    printf("\t- Useful routines in monitor which can be accessed by user programs:\n");
    printf("\t    GETLINE: location FF1F, Line buffer address=$0200\n");
    printf("\t    ECHO:    location FFEF, Echo a char. in ACC register\n");
    printf("\t    PRBYTE:  location FFDC, Print one byte(HEX) in ACC register\n");
    printf("\t    PRHEX:   location FFE5, Print least 4-bit(HEX) in ACC register\n");
    printf("\t    ENTRY:   location FF00, Monitor Entry\n");
    printf("\t    * Op-code: JSR=$20 / LDA=$A9\n");
    printf("\t- Access memory address DO18 to download cc65 binary\n");
    printf("\t- Press 'd' to start download at prompting \"D018: \"\n");
    printf("\t- Type 'h' for Help\n");
    printf("\t- Type 'q' for Exit\n");
}

char* List_Bin(void)
{
    DIR *d;
    struct dirent *dir;
    
    d = opendir(".//Arduino//cpu_wrapper_SA");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
    return(0);
}

int main(int argc, char* argv[])
{
    if (ConnectEmulator()<0)    return -1;
    PrintHelp();

    // Reset DUT
    printf("Resetting......\n");
    CPU_Resetting();
    usleep(1);

    // -------------------------------------------------------------
    // Command-Line Loop
    unsigned char Disp, KbdBuff[8];

    printf("Going command-line......\n");
    while(true)
    {
        // Keyboard input
        while (!kbhit())
        {
            //printf("Reading display from Emulator......\n");
            Disp = CPU_ReadDisplay();
            //printf("[0]%02X(%c)", rxPacket[0], rxPacket[0]);
            if (Disp & 0x80)   // Something to display?
            {
                if (Disp==0x8D) Disp  = '\n'; // CR ?
                else            Disp &= 0x7F;
                putchar(Disp);
                fflush(stdout);
                usleep(1);

                //printf("Acknowledge Display......\n");
                CPU_AckDisplay();
            }
        }
        KbdBuff[0] = getchar();
        KbdBuff[1] = '\0';

        if (KbdBuff[0]==0x0A)   KbdBuff[0] = 0x8D;  // CR
        else if (KbdBuff[0]=='q')    // Exit
        {
            close(fd);
            return 0;
        }
        else if (KbdBuff[0]=='h')    // Help
        {
            PrintHelp();
            continue;
        }
        else if (islower((int)KbdBuff[0]))
            continue;

        //printf("Writing Keyboard to Emulator......\n");
        CPU_SendKeyboard(KbdBuff[0]);
    }
    
    close(fd);
    return 0;
}
