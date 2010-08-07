EXE=x11colors

CC = cc
CFLAGS  += -pipe -O2 -W -Wall -pedantic -std=gnu99 -I/usr/include/freetype2
LDFLAGS += -lX11 -L/usr/X11R6/lib -lXft -lfontconfig -lfreetype

SRC = main.c x11colors.c
OBJ = $(SRC:.c=.o)
GARBAGE = 

PREFIX = /usr/local

all: $(EXE)
	
$(EXE): $(OBJ)
	$(CC) -o $(EXE) $(CFLAGS) $(OBJ) $(LDFLAGS)
	@ls -lh $(EXE)

install:
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -vf ${EXE} ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/${EXE}

clean:
	rm -f $(OBJ) $(EXE) $(GARBAGE) *.core
