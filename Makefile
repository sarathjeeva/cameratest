#############################################################################
# Makefile for building: cameratest
# Generated by qmake (2.01a) (Qt 4.7.2) on: ?? 9? 13 14:24:02 2012
# Project:  cameratest.pro
# Template: app
# Command: /usr/bin/qmake -o Makefile cameratest.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -I. -I.
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS)  -L/usr/lib -lQtGui -lQtCore -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = camera.cpp \
		main.cpp \
		videowidget.cpp \
		cameraattr.cpp \
		convert/convertor.cpp \
		convert/tabconvert.cpp moc_videowidget.cpp \
		moc_cameraattr.cpp
OBJECTS       = camera.o \
		main.o \
		videowidget.o \
		cameraattr.o \
		convertor.o \
		tabconvert.o \
		moc_videowidget.o \
		moc_cameraattr.o
DIST          = /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		cameratest.pro
QMAKE_TARGET  = cameratest
DESTDIR       = 
TARGET        = cameratest

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): ui_cameraattr.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: cameratest.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/libQtGui.prl \
		/usr/lib/libQtCore.prl
	$(QMAKE) -o Makefile cameratest.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/libQtGui.prl:
/usr/lib/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -o Makefile cameratest.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/cameratest1.0.0 || $(MKDIR) .tmp/cameratest1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/cameratest1.0.0/ && $(COPY_FILE) --parents camera.h videowidget.h cameraattr.h convert/convertor.h convert/convertor_registry.h convert/globals.h tabconvert.h .tmp/cameratest1.0.0/ && $(COPY_FILE) --parents camera.cpp main.cpp videowidget.cpp cameraattr.cpp convert/convertor.cpp convert/tabconvert.cpp .tmp/cameratest1.0.0/ && $(COPY_FILE) --parents cameraattr.ui .tmp/cameratest1.0.0/ && (cd `dirname .tmp/cameratest1.0.0` && $(TAR) cameratest1.0.0.tar cameratest1.0.0 && $(COMPRESS) cameratest1.0.0.tar) && $(MOVE) `dirname .tmp/cameratest1.0.0`/cameratest1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/cameratest1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_videowidget.cpp moc_cameraattr.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_videowidget.cpp moc_cameraattr.cpp
moc_videowidget.cpp: videowidget.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) videowidget.h -o moc_videowidget.cpp

moc_cameraattr.cpp: ui_cameraattr.h \
		cameraattr.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) cameraattr.h -o moc_cameraattr.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_cameraattr.h
compiler_uic_clean:
	-$(DEL_FILE) ui_cameraattr.h
ui_cameraattr.h: cameraattr.ui
	/usr/bin/uic-qt4 cameraattr.ui -o ui_cameraattr.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 

####### Compile

camera.o: camera.cpp camera.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o camera.o camera.cpp

main.o: main.cpp videowidget.h \
		camera.h \
		cameraattr.h \
		ui_cameraattr.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

videowidget.o: videowidget.cpp videowidget.h \
		tabconvert.h \
		convert/convertor.h \
		convert/convertor_registry.h \
		camera.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o videowidget.o videowidget.cpp

cameraattr.o: cameraattr.cpp cameraattr.h \
		ui_cameraattr.h \
		camera.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o cameraattr.o cameraattr.cpp

convertor.o: convert/convertor.cpp convert/globals.h \
		convert/convertor.h \
		convert/convertor_registry.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o convertor.o convert/convertor.cpp

tabconvert.o: convert/tabconvert.cpp convert/tabconvert.h \
		convert/convertor.h \
		convert/convertor_registry.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o tabconvert.o convert/tabconvert.cpp

moc_videowidget.o: moc_videowidget.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_videowidget.o moc_videowidget.cpp

moc_cameraattr.o: moc_cameraattr.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_cameraattr.o moc_cameraattr.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

