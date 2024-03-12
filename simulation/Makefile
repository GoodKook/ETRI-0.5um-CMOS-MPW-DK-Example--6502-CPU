# -----------------------------------------------------
VERILOG_SRCS = ../source/cpu.v ../source/ALU.v
SC_SRCS      = sc_main.cpp sc_mem.cpp
SC_TOP_H     = sc_CPU_6502_Top.h sc_mem.h

VERILATOR    = verilator
#CFLAGS1      = -g
CFLAGS1      = -std=c++17
CFLAGS2      = -DCR_LF_CR
#CFLAGS2      = -DCR_LF_LF
#CFLAGS2      = -DCR_LF_NONE

TOP_MODULE   = cpu
TARGET       = V$(TOP_MODULE)
TARGET_DIR   = obj_dir

export SYSTEMC			= /usr/local/systemc-3.0.0
export SYSTEMC_HOME		= $(SYSTEMC)
export SYSTEMC_INCLUDE	= $(SYSTEMC_HOME)/include
export SYSTEMC_LIBDIR	= $(SYSTEMC_HOME)/lib-linux64
export LD_LIBRARY_PATH	:=$(LD_LIBRARY_PATH):$(SYSTEMC_LIBDIR)
export CXX				= clang++
export CXXFLAGS			= -std=c++17

all : $(TARGET_DIR)/$(TARGET) std_input std_output

$(TARGET_DIR)/$(TARGET) : $(VERILOG_SRCS) $(SC_SRCS) $(SC_TOP_H)
	$(VERILATOR) --sc -Wall --top-module $(TOP_MODULE) --exe --build \
		-CFLAGS $(CFLAGS1) \
		-CFLAGS $(CFLAGS2) \
		$(VERILOG_SRCS) $(SC_SRCS)

std_input : std_input.cpp
	gcc -o std_input std_input.cpp

std_output : std_output.cpp
	gcc -o std_output std_output.cpp

run : $(TARGET_DIR)/$(TARGET) std_input std_output
	rm -f *.fifo
	xterm -T "Apple-1 Keyboard" ./std_input &
	xterm -T "Apple-1 Display" ./std_output &
	sleep 1
	./$(TARGET_DIR)/$(TARGET)

clean :
	rm -rf $(TARGET_DIR)
	rm -f std_input
	rm -f std_output
	rm -f CPU_terminal
	rm -f *.vcd
	rm -f *.fifo

debug : $(TARGET_DIR)/$(TARGET)
	ddd $(TARGET_DIR)/$(TARGET)

# ===========================================================

mti_fun_build : std_input std_output
	vsim -c -do ./mti_sim/compile_fun.do

mti_net_build : std_input std_output
	vsim -c -do ./mti_sim/compile_net.do

mti_fun_run : std_input std_output
	xterm -T "Apple-1 Keyboard" ./std_input &
	xterm -T "Apple-1 Display" ./std_output &
	sleep 1
	vsim -c -do ./mti_sim/simulate_fun.do

mti_net_run : std_input std_output
	xterm -T "Apple-1 Keyboard" ./std_input &
	xterm -T "Apple-1 Display" ./std_output &
	sleep 1
	vsim -c -do ./mti_sim/simulate_net.do

mti_clean :
	rm -rf work
	rm -f transcript
	rm -f std_input
	rm -f std_output
	rm -f *.fifo


