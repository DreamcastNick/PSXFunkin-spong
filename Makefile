TARGET = funkin
TYPE = ps-exe

SRCS = src/main.c \
       src/mutil.c \
       src/random.c \
       src/archive.c \
       src/font.c \
       src/trans.c \
       src/loadscr.c \
       src/menu.c \
       src/stage.c \
       src/psx/psx.c \
       src/psx/io.c \
       src/psx/gfx.c \
       src/psx/audio.c \
       src/psx/pad.c \
       src/psx/timer.c \
       src/psx/movie.c \
       src/stage/dummy.c \
       src/stage/week1.c \
       src/stage/week2.c \
       src/stage/week3.c \
       src/stage/week4.c \
       src/stage/week5.c \
       src/stage/week6.c \
       src/stage/week7.c \
       src/stage/week8.c \
       src/stage/week9.c \
       src/animation.c \
       src/character.c \
       src/character/bf.c \
       src/character/bfmic.c \
       src/character/bfweeb.c \
       src/character/speaker.c \
       src/character/spong.c \
       src/character/spong2.c \
       src/character/utspong.c \
       src/character/spong3.c \
       src/character/sad.c \
       src/character/hellno.c \
       src/character/sp.c \
       src/character/old.c \
       src/character/old2.c \
       src/character/old3.c \
       src/character/older.c \
       src/character/older2.c \
       src/character/p1spong.c \
       src/character/viacom.c \
       src/character/gf.c \
       src/character/gf2.c \
       src/character/gfweeb.c \
       src/object.c \
       src/object/combo.c \
       src/object/splash.c \
       mips/common/crt0/crt0.s

CPPFLAGS += -Wall -Wextra -pedantic -mno-check-zero-division
LDFLAGS += -Wl,--start-group
# TODO: remove unused libraries
LDFLAGS += -lapi
#LDFLAGS += -lc
LDFLAGS += -lc2
#LDFLAGS += -lcard
LDFLAGS += -lcd
#LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
#LDFLAGS += -lgs
#LDFLAGS += -lgte
#LDFLAGS += -lgun
#LDFLAGS += -lhmd
#LDFLAGS += -lmath
#LDFLAGS += -lmcrd
#LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
#LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
#LDFLAGS += -ltap
LDFLAGS += -flto -Wl,--end-group

include mips/common.mk
