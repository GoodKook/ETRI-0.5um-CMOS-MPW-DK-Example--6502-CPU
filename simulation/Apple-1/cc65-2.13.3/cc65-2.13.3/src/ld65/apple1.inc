static const char CfgApple1 [] = 
"MEMORY {\n"
"ZP:     start = $0020, size = $00C0, define = yes;\n"
"HEADER: start = $0000, size = $0004, file = %O;\n"
"RAM1:    start = $0280, size = $0B80, file = %O;\n"
"RAM2:    start = $E000, size = $1000, file = %O;\n"
"}\n"
"SEGMENTS {\n"
"EXEHDR:   load = HEADER, type = ro;\n"
"STARTUP:  load = RAM1,    type = ro, define = yes;\n"
"LOWCODE:  load = RAM1,    type = ro,               optional = yes;\n"
"INIT:     load = RAM1,    type = ro, define = yes, optional = yes;\n"
"CODE:     load = RAM1,    type = ro;\n"
"RODATA:   load = RAM1,    type = ro;\n"
"DATA:     load = RAM1,    type = rw;\n"
"BSS:      load = RAM2,    type = bss, define = yes;\n"
"HEAP:     load = RAM2,    type = bss, optional = yes;\n" /* must sit just below stack */
"ZEROPAGE: load = ZP,     type = zp;\n"
"}\n"
"FEATURES {\n"
"CONDES: segment = INIT,\n"
"type = constructor,\n"
"label = __CONSTRUCTOR_TABLE__,\n"
"count = __CONSTRUCTOR_COUNT__;\n"
"CONDES: segment = RODATA,\n"
"type = destructor,\n"
"label = __DESTRUCTOR_TABLE__,\n"
"count = __DESTRUCTOR_COUNT__;\n"
"CONDES: type = interruptor,\n"
"segment = RODATA,\n"
"label = __INTERRUPTOR_TABLE__,\n"
"count = __INTERRUPTOR_COUNT__;\n"
"}\n"
"SYMBOLS {\n"
"__STACKSIZE__ = $800;\n"	/* 2K stack */
"}\n"
"\n"
"\n"
;