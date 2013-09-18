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
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/730922523/mod_parse.o \
	${OBJECTDIR}/_ext/714316657/BF.o \
	${OBJECTDIR}/_ext/714316657/mod_var.o \
	${OBJECTDIR}/_ext/1325223799/BloomFilter.o \
	${OBJECTDIR}/_ext/1325223799/des.o \
	${OBJECTDIR}/_ext/1325223799/mod_killbots.o \
	${OBJECTDIR}/_ext/1325223799/mod_load.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmod_killbots.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmod_killbots.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmod_killbots.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/_ext/730922523/mod_parse.o: /home/ian/Desktop/mod_killbots/mod_parse/mod_parse.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/730922523
	${RM} $@.d
	$(COMPILE.c) -O2 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/730922523/mod_parse.o /home/ian/Desktop/mod_killbots/mod_parse/mod_parse.c

${OBJECTDIR}/_ext/714316657/BF.o: /home/ian/Desktop/mod_killbots/mod_var/BF.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/714316657
	${RM} $@.d
	$(COMPILE.c) -O2 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/714316657/BF.o /home/ian/Desktop/mod_killbots/mod_var/BF.c

${OBJECTDIR}/_ext/714316657/mod_var.o: /home/ian/Desktop/mod_killbots/mod_var/mod_var.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/714316657
	${RM} $@.d
	$(COMPILE.c) -O2 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/714316657/mod_var.o /home/ian/Desktop/mod_killbots/mod_var/mod_var.c

${OBJECTDIR}/_ext/1325223799/BloomFilter.o: /home/ian/Desktop/mod_killbots/src/BloomFilter.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1325223799
	${RM} $@.d
	$(COMPILE.c) -O2 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1325223799/BloomFilter.o /home/ian/Desktop/mod_killbots/src/BloomFilter.c

${OBJECTDIR}/_ext/1325223799/des.o: /home/ian/Desktop/mod_killbots/src/des.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1325223799
	${RM} $@.d
	$(COMPILE.c) -O2 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1325223799/des.o /home/ian/Desktop/mod_killbots/src/des.c

${OBJECTDIR}/_ext/1325223799/mod_killbots.o: /home/ian/Desktop/mod_killbots/src/mod_killbots.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1325223799
	${RM} $@.d
	$(COMPILE.c) -O2 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1325223799/mod_killbots.o /home/ian/Desktop/mod_killbots/src/mod_killbots.c

${OBJECTDIR}/_ext/1325223799/mod_load.o: /home/ian/Desktop/mod_killbots/src/mod_load.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1325223799
	${RM} $@.d
	$(COMPILE.c) -O2 -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1325223799/mod_load.o /home/ian/Desktop/mod_killbots/src/mod_load.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libmod_killbots.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
