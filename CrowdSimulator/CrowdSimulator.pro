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
    include/CellPartition.h

SOURCES += \
    src/main.cpp \
    src/CrowdEngine.cpp \
    src/Agent.cpp \
    src/CellPartition.cpp


#######################
# GENERAL CONFIGURATION
#######################

QMAKE_CXX = clang++

QMAKE_CXXFLAGS = -fdiagnostics-fixit-info -g

OBJECTS_DIR = obj

INCLUDEPATH += include

DEFINES += LINUX


###################
# NEEDED LIBRARIES:
#
# opengl, glew, NGL
# boost
###################

#OpenGL QT
QT += opengl

LIBS+= -lGLEW

#include boost for tokenizer and reading from configFiles
#INCLUDEPATH += /usr/local/boost

#NGL library at home
INCLUDEPATH += $(HOME)/NGL/include
LIBS += -L$(HOME)/NGL/lib -lNGL


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
