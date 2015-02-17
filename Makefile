all: padviz

padviz: padviz.c padviz.ui.h
	gcc `pkg-config --cflags gtk+-3.0` -o padviz padviz.c `pkg-config --libs gtk+-3.0`

padviz.ui.h: padviz.ui
	xxd -i padviz.ui > padviz.ui.h

clean:
	rm -f padviz padviz.ui.h
