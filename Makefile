#
# Makefile for msp430
#
# 'make' builds TARGET
# 'make clean' deletes everything except source files and Makefile
# 'make program' programs flash on msp430 with TARGET, then resets it.
# 'make reset' resets the msp430

# You need to set TARGET, MCU and SOURCES for your project.
# TARGET is the name of the executable file to be produced (.elf)
# eg if TARGET is foo.elf, then foo.elf will be produced, which can be programmed
#    into the msp430 flash by typing 'make program'
#
# If you want to see the assembler source from file foo.c, then use make foo.lst

TARGET   = quadcopter.elf
MCU      = msp430g2553

# List all the source files here
# eg if you have a source file foo.c then list it here
SOURCES = \
src/main.c \
src/main.h \
src/shared_interrupt_vector.c \
src/motors.c \
src/motors.h \
src/radio.c \
src/radio.h \
src/protocol.c \
src/protocol.h \
src/i2c.c \
src/i2c.h \
src/mpu6050.c \
src/mpu6050.h \
src/agent.c

# Use lines like those below to include your own libraries, include files (if you have any).
# Changing a library won't cause a rebuild - use make clean then make.
# this will link libboard430.a (use LIBPATH to say where it is, and take care of the order):
#LIBS = -lcc2420 -lboard430

# paths to extra libraries and extra standard includes
#ROOTPATH = ../..
#LIBPATH = -L$(ROOTPATH)/lib
#INCLUDES = -I$(ROOTPATH)/include


# You probably don't need to change anything below this line.
#######################################################################################
CFLAGS   = -mmcu=$(MCU) -g -O3 -Wall -Wcast-align -Wcast-qual -Wimplicit \
	   -Wmissing-declarations -Wmissing-prototypes -Wnested-externs \
	   -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow \
	   -Wstrict-prototypes -Wswitch -Wunused $(INCLUDES)
ASFLAGS  = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
#LDFLAGS  = -mmcu=$(MCU) -Wl $(LIBPATH) ##changed
LDFLAGS  = -mmcu=$(MCU)

########################################################################################

CC       = msp430-gcc
LD       = msp430-ld
AR       = msp430-ar
AS       = msp430-gcc
GASP     = msp430-gasp
NM       = msp430-nm
OBJCOPY  = msp430-objcopy
RANLIB   = msp430-ranlib
STRIP    = msp430-strip
SIZE     = msp430-size
READELF  = msp430-readelf
CP       = cp -p
RM       = rm -f
MV       = mv
#Linux jtag program
JTAGPROG = jtag.py
#Windows jtag program
#JTAGPROG = msp430-jtag
PROGRAM  = $(JTAGPROG) -mEpv
RESET    = $(JTAGPROG) -r
########################################################################################

# the file which will include dependencies
DEPEND = $(TARGET:.elf=.d)

# all the object files
OBJECTS = $(SOURCES:.c=.o)

$(TARGET): $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@

# rule for making assembler source listing, to see the code
%.lst : %.c
	$(CC) -c $(CFLAGS) -Wa,-anlhd $< > $@

# include the dependencies
-include $(DEPEND)

# dependencies file
# includes also considered, since some of these are our own
# (otherwise use -MM instead of -M)
$(DEPEND): $(SOURCES) Makefile
	$(CC) -M ${CFLAGS} $(SOURCES) >$@


.PHONY:	clean
clean:
	-$(RM) $(OBJECTS)
	-$(RM) $(TARGET)
	-$(RM) $(SOURCES:.c=.lst)
	-$(RM) $(DEPEND)	

.PHONY: program
program: $(TARGET)
	$(PROGRAM) $(TARGET)

.PHONY: reset
reset:
	$(RESET)