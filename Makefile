BIN 					= main.elf
MM_DSP_SCHABLONE_PATH	= ../mm_dsp_schablone
MMDSCH_ALSA_PATH		= ../mmdsch_alsa
MMMIDI_PATH				= ../mmmidi
SRC						= $(notdir $(wildcard $(MMDSCH_ALSA_PATH)/src/*.c))
SRC					   += $(notdir $(wildcard $(MM_DSP_SCHABLONE_PATH)/src/*.c))
SRC					   += $(notdir $(wildcard $(MMMIDI_PATH)/src/*.c))
SRC					   += $(notdir $(wildcard src/*.c))
DEP						= $(notdir $(wildcard $(MMDSCH_ALSA_PATH)/inc/*.h))
DEP					   += $(notdir $(wildcard $(MM_DSP_SCHABLONE_PATH)/inc/*.h))
DEP					   += $(notdir $(wildcard $(MMMIDI_PATH)/inc/*.h))
VPATH					= $(MMDSCH_ALSA_PATH)/src:$(MMMIDI_PATH)/src:src
VPATH				   += :$(MM_DSP_SCHABLONE_PATH)/src
INC 					= $(MM_DSP_SCHABLONE_PATH)/inc
INC					   += $(MMDSCH_ALSA_PATH)/inc
INC					   += $(MMMIDI_PATH)/inc
CFLAGS					= $(foreach inc,$(INC),-I$(inc))
CFLAGS					+= -ggdb3
LDFLAGS				    = -lasound -lm
OBJSDIR					= objs
OBJS					= $(addprefix $(OBJSDIR)/,\
						  	$(addsuffix .o, $(basename $(SRC))))
CC 						= gcc

all: $(OBJSDIR) $(OBJS) $(BIN)

$(OBJSDIR):
	if [ ! -d "$(OBJSDIR)" ]; then mkdir $(OBJSDIR); fi

$(OBJS) : $(OBJSDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN) : $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm objs/*.o
