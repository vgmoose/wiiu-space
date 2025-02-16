# default this environment variable
DEVKITPRO ?= /opt/devkitpro
DEVKITPPC ?= $(DEVKITPRO)/devkitPPC
WUT_ROOT ?= $(DEVKITPRO)/wut
export DEVKITPRO
export DEVKITPPC
export WUT_ROOT

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/wut/share/wut_rules

export APP_NAME    ?= Space Game
export APP_AUTHOR  ?= vgmoose
export APP_VERSION ?= 2.0

export APP_TV_SPLASH  := $(PWD)/res/bootTvTex.png
export APP_DRC_SPLASH := $(PWD)/res/bootTvTex.png
export APP_ICON       := $(PWD)/res/iconTex.png
export APP_CONTENT	  := $(PWD)/data

TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	src
INCLUDES	:=	include

#-------------------------------------------------------------------------------
# options for code generation
#-------------------------------------------------------------------------------
CFLAGS		:=	-Wall -O2 -std=c11 \
			-ffast-math \
			$(MACHDEP)

CFLAGS		+=	$(INCLUDE) -D__WIIU__ -D__WUT__ -DMUSIC

CXXFLAGS	:=	$(CFLAGS)

ASFLAGS		:=	-g $(MACHDEP)
LDFLAGS		:=	-g $(MACHDEP) $(RPXSPECS) -Wl,-Map,$(notdir $*.map)

LIBS		:=	-lwut -lmpg123 -lm

#-------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level
# containing include and lib
#-------------------------------------------------------------------------------
LIBDIRS		:=	$(PORTLIBS) $(WUT_ROOT)


#-------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#-------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#-------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

#-------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#-------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#-------------------------------------------------------------------------------
	export LD	:=	$(CC)
#-------------------------------------------------------------------------------
else
#-------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#-------------------------------------------------------------------------------
endif
#-------------------------------------------------------------------------------

export SRCFILES		:=	$(CPPFILES) $(CFILES) $(SFILES)
export OFILES		:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export INCLUDE		:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
				$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
				-I$(CURDIR)/$(BUILD)

export LIBPATHS		:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean all

#-------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD): $(SRCFILES)
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#-------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).rpx $(TARGET).elf

#-------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS		:=	$(OFILES:.o=.d)

#-------------------------------------------------------------------------------
# main targets
#-------------------------------------------------------------------------------
all		:	$(OUTPUT).wuhb

$(OUTPUT).wuhb	:	$(OUTPUT).rpx
$(OUTPUT).rpx	:	$(OUTPUT).elf
$(OUTPUT).elf	:	$(OFILES)

-include $(DEPENDS)

#-------------------------------------------------------------------------------
endif
#-------------------------------------------------------------------------------
