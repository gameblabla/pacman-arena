# SDL 1.2 (original version)

PRGNAME     = pac.elf
CC			= gcc
NETWORK		= NO

SRCDIR		= ./lib3ds ./src ./src/main
SRCDIR		+= ./GLU/src/libutil GLU/src/libtess GLU/src/libnurbs/interface GLU/src/libnurbs/internals GLU/src/libnurbs/nurbtess
VPATH		= $(SRCDIR)
SRC_C		= $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
OBJ_C		= $(notdir $(patsubst %.c, %.o, $(SRC_C)))
OBJS		= $(OBJ_C)

CFLAGS		= -O0 -g3 -Wall -Wextra -I/usr/include/SDL -Wno-write-strings -Isrc -Isrc/include -I. -Iplatform/generic -IGLU/src/include
CFLAGS		+= -DLSB_FIRST -std=gnu99 -DALIGN_DWORD -DNOYUV
CFLAGS		+=  -Ilib3ds

ifeq ($(NETWORK), YES)
CFLAGS 		+= -DNETWORKING_GAME
endif

ifeq ($(PROFILE), YES)
CFLAGS 		+= -fprofile-generate=./
else ifeq ($(PROFILE), APPLY)
CFLAGS		+= -fprofile-use -fbranch-probabilities
endif

LDFLAGS     = -lc -lgcc -lm -lSDL -lSDL_mixer -lSDL_image -lGL
ifeq ($(NETWORK), YES)
LDFLAGS 	+= -lSDL_net
endif

# Rules to make executable
$(PRGNAME): $(OBJS)  
	$(CC) $(CFLAGS) -o $(PRGNAME) $^ $(LDFLAGS)

$(OBJ_CP) : %.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(PRGNAME) *.o
