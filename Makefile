#---------------------------------------------------------------------------------
.SUFFIXES:

#-----------------------------------------------------------------------------------
#Some project information
#-----------------------------------------------------------------------------------
#NAME 		:= $(shell basename $(CURDIR))
NAME 		:=	sasm
TARGET		:=	$(NAME)
BUILD		:=	build

#set source and include directories
SOURCES		:=	source
INCLUDES	:=	includes


#compiler settings
CC 		:=  gcc
CFLAGS	:=	-g -Wall -O2 -pedantic -std=gnu99
CFLAGS	+=	$(INCLUDE)
LDFLAGS	=	-g -Wl

UNAME := $(shell uname)


ifeq ($(UNAME), Darwin)
#GCC is replaced by Clang on macs, so
#disable clang's warnings for using gcc features
#it it clearly supports
CFLAGS += -Wno-gnu

#otherwise, there are no warnings when using gcc to compile
endif

#-----------------------------------------------------------------------------------
#Assembler Testing stuff
#-----------------------------------------------------------------------------------
ASMBIN		:= ./$(TARGET)

ASMROOT		:= asmBuilds
ASMSRC		:= $(ASMROOT)/srcFiles
ASMOUT		:= $(ASMROOT)/myBins

SVMBIN 		:= $(ASMROOT)/svm
TESTBIN		:= $(ASMROOT)/finalBins

ASMFILES	:= $(foreach dir, $(ASMSRC), $(wildcard $(dir)/*.svm))
OUTFILES	:= $(addprefix $(ASMOUT)/, $(notdir $(ASMFILES:.svm=.out)))
TESTFILES	:= $(foreach dir, $(TESTBIN), $(wildcard $(dir)/*.out))


#-----------------------------------------------------------------------------------
#Building rules - Super awesome recursive makefile calling!
#-----------------------------------------------------------------------------------

#if in project root directory
ifneq ($(BUILD),$(notdir $(CURDIR)))

#collect source files and set up paths
export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))

#generate o files from c files
export OFILES	:=	$(CFILES:.c=.o)

#include directories
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					-I$(CURDIR)/$(BUILD)

export LD	:=	$(CC)

.PHONY: all $(BUILD) clean release test

all: $(BUILD)

#create build directory, change into it, then call make on this makefile again
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile


clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET) $(ASMOUT)/*.out

#-----------------------------------------------------------------------------------
#Build Targets when not in root directory
#-----------------------------------------------------------------------------------
else

#collection dependencies
DEPENDS	:=	$(OFILES:.o=.d)

#build targets
$(OUTPUT).bin	:	$(OFILES)


%.o: %.c
	@echo $(notdir $<)
	$(CC) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) -c $< -o $@


%.bin:
	@echo linking $(notdir $@)
	@$(LD)  $(LDFLAGS) $(OFILES) -o $@

	@mv $@ $(shell dirname $@)/$(NAME)

endif


#-----------------------------------------------------------------------------------
#Clean and package project directory in tar file
#-----------------------------------------------------------------------------------
release: clean
	tar -zcvf "goldd5_$(NAME).tar.gz" *


md5:
	@md5 -r  $(OUTFILES) $(TESTFILES)



#Assemble all .svm files within the ASMSRC directory
test: $(OUTFILES)

$(ASMOUT)/%.out: $(ASMSRC)/%.svm
	@$(ASMBIN) < $< > $@
	@echo "----------------------------------------------------------------"
