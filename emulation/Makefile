# -----------------------------------------------------
export SYSTEMC			= /usr/local/systemc-3.0.0
export SYSTEMC_HOME		= $(SYSTEMC)
export SYSTEMC_INCLUDE	= $(SYSTEMC_HOME)/include
export SYSTEMC_LIBDIR	= $(SYSTEMC_HOME)/lib-linux64
export LD_LIBRARY_PATH	:=$(LD_LIBRARY_PATH):$(SYSTEMC_LIBDIR)
export CXX				= clang++
export CXXFLAGS			= -std=c++17

SC_SRCS		=	../simulation/sc_main.cpp \
				../simulation/sc_mem.cpp
SC_TOP_H	=	./Vcpu.h \
				./CPU_6502.h \
				../simulation/sc_CPU_6502_Top.h \
				../simulation/sc_mem.h
SC_DEFS		=	CR_LF_CR
#SC_DEFS		=	CR_LF_LF
#SC_DEFS		= 	CR_LF_NONE

TOP_MODULE   = cpu

all :
	@echo 'Makefile for CPU_6502 Co-Emulation: Verilog RTL is implemented into FPGA and'
	@echo '	   warapped with Arduino and simulate under SystemC Testbench'
	@echo ''
	@echo '    make build'
	@echo '        Builds executable of "cpu" with Verilated RTL and SystemC TB, and'
	@echo '          linux pipe app(fifo): "std_input", "std_output" for IO system.'
	@echo ''
	@echo '    make run'
	@echo '        Default firmware: "./Apple-1/program.bin"'
	@echo ''
	@echo '    make wave'
	@echo ''
	@echo '    make clean'
	@echo '        Clean working folder'
	@echo ''
	@echo 'CPU_6502 Stand-Alone:'
	@echo '    make standalone'
	@echo ''

build : $(TOP_MODULE) std_input std_output

$(TOP_MODULE) : $(VERILOG_SRCS) $(SC_SRCS) $(SC_TOP_H)
	$(CXX) $(CXXFLAGS) \
		-D$(SC_DEFS) \
		-I. -I../simulation -I$(SYSTEMC_INCLUDE) \
		-L$(SYSTEMC_LIBDIR) -Wl,-rpath=$(SYSTEMC_LIBDIR) \
		-lsystemc -lm -lgsl -lpthread \
		-o $(TOP_MODULE) $(SC_SRCS)

std_input : ../simulation/std_input.cpp
	gcc -o std_input ../simulation/std_input.cpp

std_output : ../simulation/std_output.cpp
	gcc -o std_output ../simulation/std_output.cpp

run : $(TOP_MODULE) std_input std_output
	rm -f *.fifo
	xterm -T "Apple-1 Keyboard" ./std_input &
	xterm -T "Apple-1 Display" ./std_output &
	sleep 1
	./$(TOP_MODULE)
	
standalone : CPU_6502_SA
	./CPU_6502_SA

CPU_6502_SA : CPU_6502_SA.cpp
	g++ -o CPU_6502_SA CPU_6502_SA.cpp

wave : sc_CPU_6502.vcd
	gtkwave sc_CPU_6502.vcd --save=sc_CPU_6502.gtkw &
	gtkwave sc_CPU_6502_Emul.vcd --save=sc_CPU_6502_Emul.gtkw &

clean :
	rm -rf $(TOP_MODULE)
	rm -f std_input
	rm -f std_output
	rm -f CPU_6502_SA
	rm -f CPU_terminal
	rm -f *.vcd
	rm -f *.fifo
	rm -f ./Vivado/CPU_6502/vivado*
	rm -f ./Vivado/CPU_6502_RT/vivado*

debug : $(TOP_MODULE)
	ddd $(TOP_MODULE)

