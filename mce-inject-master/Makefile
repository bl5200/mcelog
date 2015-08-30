prefix := /usr
manprefix := ${prefix}/share
CFLAGS := -Os -g -Wall
LDFLAGS += -pthread

OBJ := mce.tab.o lex.yy.o mce-inject.o util.o
GENSRC := mce.tab.c lex.yy.c
SRC := mce-inject.c util.c
CLEAN := ${OBJ} ${GENSRC} mce.tab.h inject mce-inject .depend
DISTCLEAN := .depend .gdb_history

.PHONY: clean depend install

mce-inject: ${OBJ}

lex.yy.c: mce.lex mce.tab.h
	flex mce.lex
	
mce.tab.c mce.tab.h: mce.y
	bison -d mce.y

install: mce-inject mce-inject.8
	install -d $(destdir)$(prefix)/sbin
	install -m 755 mce-inject $(destdir)$(prefix)/sbin/mce-inject
	install -d $(destdir)$(manprefix)/man/man8
	install -m 644 mce-inject.8 $(destdir)$(manprefix)/man/man8/mce-inject.8

clean:
	rm -f ${CLEAN}

distclean: clean
	rm -f ${DISTCLEAN} *~

depend: .depend

.depend: ${SRC} ${GENSRC}
	${CC} -MM -DDEPS_RUN -I. ${SRC} ${GENSRC} > .depend.X && \
		mv .depend.X .depend

Makefile: .depend

include .depend
