# Copyright Iliescu Miruna-Elena 312CA

CC=gcc
CFLAGS=-Wall -Wextra -std=c99

TARGETS= image_editor

build: $(TARGETS)

image_editor: image_editor.c
	$(CC) $(CFLAGS) image_editor.c -lm -o image_editor

pack:
	zip -FSr 312CA_IliescuMiruna-Elena_Tema3.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS)

.PHONY:pack clean