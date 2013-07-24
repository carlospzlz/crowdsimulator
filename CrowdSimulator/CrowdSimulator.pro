#############################################
# CROWD SIMULATION PROJECT CONFIGURATION FILE
#
# Author: Carlos Perez Lopez
# Master Thesis 2013
# MSc Computer Animation and VFX
# NCCA - Bournemouth University
#############################################

TARGET = CrowdSimulator


##############
# SOURCE FILES
##############

HEADERS += \
    include/Agent.h \
    include/CrowdEngine.h \
    include/CellPartition.h \
    include/GLWindow.h \
    include/MainWindow.h \
    include/QuadraticGridCP.h \
    include/Parser.h \
    include/TXTParser.h \
    include/PhysicsEngine.h \
    include/RadialPE.h

SOURCES += \
    src/main.cpp \
    src/CrowdEngine.cpp \
    src/Agent.cpp \
    src/GLWindow.cpp \
    src/MainWindow.cpp \
    src/QuadraticGridCP.cpp \
    src/TXTParser.cpp \
    src/Parser.cpp \
    src/RadialPE.cpp

FORMS += \
    ui/MainWindow.ui


#######################
# GENERAL CONFIGURATION
#######################

QMAKE_CXX = clang++

QMAKE_CXXFLAGS = -fdiagnostics-fixit-info -g

OBJECTS_DIR = obj

MOC_DIR = moc

INCLUDEPATH += include

DEFINES += LINUX


###################
# NEEDED LIBRARIES:
#
# opengl, glew, NGL
# lua, boost
###################

#OpenGL QT
QT += opengl
QT += \
    core \
    gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS+= -lGLEW

#NGL library at home
INCLUDEPATH += $(HOME)/NGL/include
LIBS += -L$(HOME)/NGL/lib -lNGL

#link lua dynamic libraries
LIBS += -llua -ldl

#include boost for tokenizer and reading from configFiles
#INCLUDEPATH += /usr/local/boost


######################################
# For the crap of WINDOWS, no offense
######################################

win32: {
        DEFINES+=USING_GLEW
        INCLUDEPATH+=-I c:/boost_1_44_0
        INCLUDEPATH+=-I c:/boost

        INCLUDEPATH+= -I C:/NGL/Support/glew
        LIBS+= -L C:/NGL/lib
        LIBS+= -lmingw32
        DEFINES += WIN32
        DEFINES += USING_GLEW
        DEFINES +=GLEW_STATIC
        DEFINES+=_WIN32
        SOURCES+=C:/NGL/Support/glew/glew.c
        INCLUDEPATH+=C:/NGL/Support/glew/
}


###############
# PRINTING INFO
###############

message(INCLUDEPATH: $$INCLUDEPATH)
message(LIBS: $$LIBS)
