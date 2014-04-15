# -- Configuration -------------------------------------------------------------

CC?=cc
OPTIMIZATION?=-O3 -march=native -fomit-frame-pointer -funroll-loops

STD  = -std=c99 -pedantic
WARN = -Wall -Wextra
OPT  = $(OPTIMIZATION)

PREFIX?=/usr/local
DPREFIX=$(DESTDIR)$(PREFIX)
INSTALL_BIN=$(DPREFIX)/bin
INSTALL_LIB=$(DPREFIX)/lib
INSTALL_INCLUDE=$(DPREFIX)/include
INSTALL=install

# -- Setup ---------------------------------------------------------------------

USING_COMPCERT=$(shell sh -c '(($(CC) --help | grep CompCert) > /dev/null && echo YES) || echo NO')

ifdef DEBUG
DEBUGOPT=-g -gdb

ifdef ASAN
DEBUGOPT+=-fsanitize=address
endif
ifdef UBSAN
DEBUGOPT+=-fsanitize=undefined
endif
endif

ifeq ($(USING_COMPCERT),YES)
# CompCert has pretty non-standard flags
MY_CFLAGS=$(DEBUGOPT)
else
MY_CFLAGS=$(STD) $(WARN) $(OPT) $(DEBUGOPT) $(CFLAGS)
endif

CCCOLOR="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR="\033[33m"
BINCOLOR="\033[37;1m"
MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

ifndef V
QUIET_CC = @printf '    %b         %b\n' $(CCCOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_AR = @printf '    %b         %b\n' $(CCCOLOR)AR$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_LINK = @printf '    %b       %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_RANLIB = @printf '    %b     %b\n' $(LINKCOLOR)RANLIB$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_INSTALL = @printf '    %b    %b\n' $(LINKCOLOR)INSTALL$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_CLANG = @printf '    %b      %b\n' $(CCCOLOR)CLANG$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_LSS = @printf '    %b        %b\n' $(LINKCOLOR)LSS$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_FRAMAC = @printf '    %b    %b\n' $(LINKCOLOR)FRAMA-C$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_CRYPTOL_PROVE = @printf '    %b %b\n' $(LINKCOLOR)CRYPTOL[P]$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_CRYPTOL_CHECK = @printf '    %b %b\n' $(LINKCOLOR)CRYPTOL[C]$(ENDCOLOR) $(BINCOLOR)$<$(ENDCOLOR) 1>&2;
QUIET_CRYPTOL_GENTEST = @printf '    %b %b\n' $(LINKCOLOR)CRYPTOL[G]$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
endif

QCC=$(QUIET_CC) $(CC)
QAR=$(QUIET_AR) ar
QLINK=$(QUIET_LINK) $(CC)
QRANLIB=$(QUIET_RANLIB) ranlib
QINSTALL=$(QUIET_INSTALL) $(INSTALL)

E=@echo
ifndef V
Q=@
else
Q=
endif

# -- Rules ---------------------------------------------------------------------

all:  minibsdiff libminibsdiff.a libminibsdiff.so

DATE=`date +"%Y%m%d"`
RELNAME=minibsdiff-`git describe`
tag:
	$(Q)git tag -a -m 'Version $(DATE)' $(DATE)
tarball:
	$(Q)git archive --prefix=$(RELNAME)/ -o $(RELNAME).tar.xz HEAD

clean:
	$(Q)rm -f *.xz *.a *.so *.o *.dyn_o *~ minibsdiff

# -- Build rules ---------------------------------------------------------------

minibsdiff: minibsdiff.c
	$(QCC) $(MY_CFLAGS) -o $@ $<

libminibsdiff.so: bsdiff.dyn_o bspatch.dyn_o
	$(QLINK) -shared -o $@ bsdiff.dyn_o bspatch.dyn_o
libminibsdiff.a: bsdiff.o bspatch.o
	$(QAR) -rc $@ bsdiff.o bspatch.o
	$(QRANLIB) $@

%.o: %.c
	$(QCC) $(MY_CFLAGS) -o $@ -c $<
%.dyn_o: %.c
	$(QCC) $(MY_CFLAGS) -fPIC -o $@ -c $<

# -- Install rules -------------------------------------------------------------

install: 	$(INSTALL_LIB)/libminibsdiff.a \
	 	$(INSTALL_LIB)/libminibsdiff.so \
		$(INSTALL_INCLUDE)/bsdiff.h $(INSTALL_INCLUDE)/bspatch.h \
		$(INSTALL_BIN)/minibsdiff

$(INSTALL_INCLUDE)/bsdiff.h: bsdiff.h
	$(Q)mkdir -p $(INSTALL_INCLUDE)
	$(QINSTALL) $< $(INSTALL_INCLUDE)

$(INSTALL_INCLUDE)/bspatch.h: bspatch.h
	$(Q)mkdir -p $(INSTALL_INCLUDE)
	$(QINSTALL) $< $(INSTALL_INCLUDE)

$(INSTALL_LIB)/libminibsdiff.a: libminibsdiff.a
	$(Q)mkdir -p $(INSTALL_LIB)
	$(QINSTALL) $< $(INSTALL_LIB)

$(INSTALL_LIB)/libminibsdiff.so: libminibsdiff.so
	$(Q)mkdir -p $(INSTALL_LIB)
	$(QINSTALL) $< $(INSTALL_LIB)

$(INSTALL_BIN)/minibsdiff: minibsdiff
	$(Q)mkdir -p $(INSTALL_BIN)
	$(QINSTALL) $< $(INSTALL_BIN)

uninstall:
	$(Q)rm -f $(INSTALL_LIB)/libminibsdiff.a
	$(Q)rm -f $(INSTALL_LIB)/libminibsdiff.so
	$(Q)rm -f $(INSTALL_INCLUDE)/bsdiff.h $(INSTALL_INCLUDE)/bspatch.h
