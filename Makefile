#
# Disassembler makefile for Linux
#

OBJDIR = ./obj

CC = gcc
CFLAGS = -Wall -O2
LIBS =
D52OBJS = $(OBJDIR)/d52.o $(OBJDIR)/common.o $(OBJDIR)/d52pass1.o $(OBJDIR)/d52pass2.o $(OBJDIR)/d52table.o $(OBJDIR)/analyze52.o $(OBJDIR)/analyze.o
D48OBJS = $(OBJDIR)/d48.o $(OBJDIR)/common.o $(OBJDIR)/d48pass.o $(OBJDIR)/d48table.o
DZ80OBJS = $(OBJDIR)/dz80.o $(OBJDIR)/common.o $(OBJDIR)/dz80pass1.o $(OBJDIR)/dz80pass2.o $(OBJDIR)/dz80table.o $(OBJDIR)/d80table.o $(OBJDIR)/analyzez80.o $(OBJDIR)/analyze.o

WINCC=/usr/local/cross-tools/bin/i386-mingw32msvc-gcc
WINCFLAGS=-Wall -O2 -fomit-frame-pointer -s -I/usr/local/cross-tools/include -D_WIN32 -DWIN32
WINLIBS=
WIND52OBJS = $(OBJDIR)/d52.obj $(OBJDIR)/common.obj $(OBJDIR)/d52pass1.obj $(OBJDIR)/d52pass2.obj $(OBJDIR)/d52table.obj $(OBJDIR)/analyze52.obj $(OBJDIR)/analyze.obj
WIND48OBJS = $(OBJDIR)/d48.obj $(OBJDIR)/common.obj $(OBJDIR)/d48pass.obj $(OBJDIR)/d48table.obj
WINDZ80OBJS = $(OBJDIR)/dz80.obj $(OBJDIR)/common.obj $(OBJDIR)/dz80pass1.obj $(OBJDIR)/dz80pass2.obj $(OBJDIR)/dz80table.obj $(OBJDIR)/d80table.obj $(OBJDIR)/analyzez80.obj $(OBJDIR)/analyze.obj

all: d52 d48 dz80

install: d52 d48 dz80
	cp -f d52 /usr/local/bin
	cp -f d48 /usr/local/bin
	cp -f dz80 /usr/local/bin

d52: $(D52OBJS)
	$(CC) $(CFLAGS) $(D52OBJS) -o d52 $(LIBS)
	strip d52

d48: $(D48OBJS)
	$(CC) $(CFLAGS) $(D48OBJS) -o d48 $(LIBS)
	strip d48

dz80: $(DZ80OBJS)
	$(CC) $(CFLAGS) $(DZ80OBJS) -o dz80 $(LIBS)
	strip dz80

$(OBJDIR)/d52.o: d52.c defs.h d52.h dispass0.c d52pass1.h d52pass2.h dispass3.c d52table.h analyze.h analyze.c analyze52.h analyze52.c common.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/d48.o: d48.c defs.h d48.h dispass0.c d48pass.h dispass3.c d48table.h common.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/dz80.o: dz80.c defs.h dz80.h dispass0.c dz80pass1.h dz80pass2.h dispass3.c dz80table.h analyze.h analyze.c analyzez80.h analyzez80.c common.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/common.o: common.c defs.h d52.h common.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/d52pass1.o:  d52pass1.c defs.h d52.h d52pass1.h d52pass2.h d52table.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/d52pass2.o:  d52pass2.c defs.h d52.h d52pass1.h d52pass2.h d52table.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/d52table.o: d52table.c defs.h d52.h d52table.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/d48pass.o:  d48pass.c defs.h d48.h d48pass.h d48table.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/d48table.o: d48table.c defs.h d48.h d48table.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/dz80pass1.o:  dz80pass1.c defs.h dz80.h dz80pass1.h dz80pass2.h dz80table.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/dz80pass2.o:  dz80pass2.c defs.h dz80.h dz80pass1.h dz80pass2.h dz80table.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/dz80table.o: dz80table.c defs.h dz80.h dz80table.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/d80table.o: d80table.c defs.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/analyze.o: analyze.c defs.h analyze.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/analyze52.o: analyze52.c defs.h d52.h analyze.h analyze.c analyze52.h d52pass2.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/analyzez80.o: analyzez80.c defs.h d52.h analyze.h analyze.c analyzez80.h d52pass2.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o
	rm -f d52 d48 dz80

win: d52.exe d48.exe dz80.exe

wind52: d52.exe

wind48: d48.exe

windz80: dz80.exe

d52.exe: $(WIND52OBJS)
	$(WINCC) $(WINCFLAGS) $(WIND52OBJS) -o d52.exe $(WINLIBS)

d48.exe: $(WIND48OBJS)
	$(WINCC) $(WINCFLAGS) $(WIND48OBJS) -o d48.exe $(WINLIBS)

dz80.exe: $(WINDZ80OBJS)
	$(WINCC) $(WINCFLAGS) $(WINDZ80OBJS) -o dz80.exe $(WINLIBS)

$(OBJDIR)/d52.obj: d52.c defs.h d52.h dispass0.c d52pass1.h d52pass2.h dispass3.c d52table.h analyze.h analyze.c analyze52.h analyze52.c common.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/d48.obj: d48.c defs.h d48.h dispass0.c d48pass.h dispass3.c d48table.h common.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/dz80.obj: dz80.c defs.h dz80.h dispass0.c dz80pass1.h dz80pass2.h dispass3.c dz80table.h analyze.h analyze.c analyzez80.h analyzez80.c common.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/common.obj: common.c defs.h d52.h common.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/d52pass1.obj:  d52pass1.c defs.h d52.h d52pass1.h d52pass2.h d52table.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/d52pass2.obj:  d52pass2.c defs.h d52.h d52pass1.h d52pass2.h d52table.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/d52table.obj: d52table.c defs.h d52.h d52table.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/d48pass.obj:  d48pass.c defs.h d48.h d48pass.h d48table.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/d48table.obj: d48table.c defs.h d48.h d48table.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/dz80pass1.obj:  dz80pass1.c defs.h dz80.h dz80pass1.h dz80pass2.h dz80table.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/dz80pass2.obj:  dz80pass2.c defs.h dz80.h dz80pass1.h dz80pass2.h dz80table.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/dz80table.obj: dz80table.c defs.h dz80.h dz80table.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/d80table.obj: d80table.c defs.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/analyze.obj: analyze.c defs.h analyze.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/analyze52.obj: analyze52.c defs.h d52.h analyze.h analyze.c analyze52.h d52pass2.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

$(OBJDIR)/analyzez80.obj: analyzez80.c defs.h d52.h analyze.h analyze.c analyzez80.h d52pass2.h
	$(WINCC) -o $@ $(WINCFLAGS) -c $<

winclean:
	rm -f $(OBJDIR)/*.obj
	rm -f d52.exe d48.exe dz80.exe

# end of file

