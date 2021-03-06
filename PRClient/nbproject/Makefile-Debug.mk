#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/7daaf93a/byteConverter.o \
	${OBJECTDIR}/_ext/7daaf93a/networkInterface.o \
	${OBJECTDIR}/src/Game.o \
	${OBJECTDIR}/src/Icon.o \
	${OBJECTDIR}/src/Map.o \
	${OBJECTDIR}/src/Network.o \
	${OBJECTDIR}/src/Player.o \
	${OBJECTDIR}/src/Renderer.o \
	${OBJECTDIR}/src/Sprite.o \
	${OBJECTDIR}/src/Texture.o \
	${OBJECTDIR}/src/Window.o \
	${OBJECTDIR}/src/World.o \
	${OBJECTDIR}/src/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/prclient

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/prclient: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/prclient ${OBJECTFILES} ${LDLIBSOPTIONS} -lSDL2main -lSDL2 -lSDL2_net -pthread

${OBJECTDIR}/_ext/7daaf93a/byteConverter.o: ../Common/byteConverter.c
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/byteConverter.o ../Common/byteConverter.c

${OBJECTDIR}/_ext/7daaf93a/networkInterface.o: ../Common/networkInterface.c
	${MKDIR} -p ${OBJECTDIR}/_ext/7daaf93a
	${RM} "$@.d"
	$(COMPILE.c) -g -DDEBUG -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7daaf93a/networkInterface.o ../Common/networkInterface.c

${OBJECTDIR}/src/Game.o: src/Game.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -DDEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Game.o src/Game.cpp

${OBJECTDIR}/src/Icon.o: src/Icon.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -DDEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Icon.o src/Icon.cpp

${OBJECTDIR}/src/Map.o: src/Map.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -DDEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Map.o src/Map.cpp

${OBJECTDIR}/src/Network.o: src/Network.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -DDEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Network.o src/Network.cpp

${OBJECTDIR}/src/Player.o: src/Player.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -DDEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Player.o src/Player.cpp

${OBJECTDIR}/src/Renderer.o: src/Renderer.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -DDEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Renderer.o src/Renderer.cpp

${OBJECTDIR}/src/Sprite.o: src/Sprite.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -DDEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Sprite.o src/Sprite.cpp

${OBJECTDIR}/src/Texture.o: src/Texture.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -DDEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Texture.o src/Texture.cpp

${OBJECTDIR}/src/Window.o: src/Window.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -DDEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Window.o src/Window.cpp

${OBJECTDIR}/src/World.o: src/World.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -DDEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/World.o src/World.cpp

${OBJECTDIR}/src/main.o: src/main.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -s -DDEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
