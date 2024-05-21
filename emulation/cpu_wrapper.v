//
// Co-Emulation warapper for CPU_6502
//

module cpu_wrapper(Data_In, Data_Out, Addr, load_emu, get_emu, clk_emu, clk_dut, clk_LED);
    input  [7:0] Data_In;
    output [7:0] Data_Out;
    input  [7:0] Addr;
    input  load_emu, get_emu, clk_emu;
    input  clk_dut;
    output clk_LED;

    wire [15:0]  AB;
    wire [7:0]   DO;
    wire         WE;

    reg [7:0] Data_Out;

    // Stimulus & Output capture for DUT
    reg [7:0]   stimIn_1, stimIn_0;
    reg [7:0]   vectOut_0, vectOut_1, vectOut_2, vectOut_3;
    // DUT interface
    reg         reset;  // reset signal
    reg [15:0]  AB;     // address bus
    reg [7:0]   DI;     // data in, read bus
    reg [7:0]   DO;     // data out, write bus
    reg         WE;     // write enable
    reg         IRQ;    // interrupt request
    reg         NMI;    // non-maskable interrupt request
    reg         RDY;    // Ready signal. Pauses CPU when RDY=0 

    // Emulation Transactor
    always @(posedge clk_emu)
    begin
        if (load_emu)   // Input stimulus to DUT
        begin
            reset <= stimIn_0[0];
            IRQ   <= stimIn_0[1];
            NMI   <= stimIn_0[2];
            RDY   <= stimIn_0[3];
            DI    <= stimIn_1;
        end
        else if (get_emu)   // Capure output from DUT
        begin
            vectOut_0    <= AB[15:8];
            vectOut_1    <= AB[7:0];
            vectOut_2    <= DO;
            vectOut_3[0] <= WE;
        end
        else
        begin
            case( Addr )
            0: stimIn_0 <= Data_In;
            1: stimIn_1 <= Data_In;
            endcase
            
            case( Addr )
            0: Data_Out <= vectOut_0;
            1: Data_Out <= vectOut_1;
            2: Data_Out <= vectOut_2;
            3: Data_Out <= vectOut_3;
            endcase
        end
    end

    // This is for blinking LED
    reg [3:0] counter;
    always @(posedge clk_dut)
    begin
        counter <= counter + 1;
    end
    // Read-back DUT output
    assign clk_LED = counter[3];   // Monitor emulation process
    
    // DUT
    cpu u_cpu (
        .clk(clk_dut),
        .reset(reset),
        .AB(AB),
        .DI(DI),
        .DO(DO),
        .WE(WE),
        .IRQ(IRQ),
        .NMI(NMI),
        .RDY(RDY) );

endmodule
