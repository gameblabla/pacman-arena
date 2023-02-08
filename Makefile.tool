PRGNAME     = 3dsconv.elf
CC			= gcc

SRCDIR		= ./lib3ds ./tools ./src
VPATH		= $(SRCDIR)
SRC_C		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
OBJ_C		= $(notdir $(patsubst %.c, %.o, $(SRC_C)))
OBJS		= $(OBJ_C)

CFLAGS		= -O0 -g3 -Wall -Wextra -I/usr/include/SDL -Wno-write-strings -Isrc -Isrc/include -I. -Iplatform/generic
CFLAGS		+= -DLSB_FIRST -std=gnu99 -DALIGN_DWORD -DNOYUV
CFLAGS		+= -IPisteEngineSDL -IPekkaKana -IPisteEngine -IPisteLanguage -Ilib3ds

ifeq ($(PROFILE), YES)
CFLAGS 		+= -fprofile-generate=./
else ifeq ($(PROFILE), APPLY)
CFLAGS		+= -fprofile-use -fbranch-probabilities
endif

LDFLAGS     = -lc -lgcc -lm -lSDL -lSDL_mixer -lSDL_image -lSDL_gfx -lSDL_net -lGL -lGLU

# Rules to make executable
$(PRGNAME): $(OBJS)  
	$(CC) $(CFLAGS) -o $(PRGNAME) $^ $(LDFLAGS)

$(OBJ_CP) : %.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(PRGNAME) *.o
