## these are set in ../Makefile
## CC, AR, AROPT, CFLAGS, RANLIB
#
# all the sources
#
SRC	= time.c io.c main.c
HDR	= def.h

#
# common objects
#
OBJ = time.$(OEXT) io.$(OEXT) main.$(OEXT)

#
# all targets
#
all: libcacti.$(LEXT)
	@echo "my work is done here..."

libcacti.$(LEXT):	$(OBJ)
	$(RM) libcacti.$(LEXT)
	$(AR) $(AROPT)libcacti.$(LEXT) $(OBJ)
	$(RANLIB) libcacti.$(LEXT)

.c.$(OEXT):
	$(CC) $(CFLAGS) -c $*.c

filelist:
	@echo $(SRC) $(HDR) Makefile

diffs:
	-rcsdiff RCS/*

clean:
	-$(RM) *.o *.obj core *~ Makefile.bak libcacti.a

unpure:
	rm -f sim.pure *pure*.o sim.pure.pure_hardlink sim.pure.pure_linkinfo

depend:
	makedepend.local -n -x $(SRC)


# DO NOT DELETE THIS LINE -- make depend depends on it.

cacti.$(OEXT): def.h
io.$(OEXT): def.h

