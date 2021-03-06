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
	${OBJECTDIR}/src/HTML.o \
	${OBJECTDIR}/src/STIL/STIL.o \
	${OBJECTDIR}/src/STIL/cJSON.o \
	${OBJECTDIR}/src/STIL/memory.o \
	${OBJECTDIR}/src/annotation.o \
	${OBJECTDIR}/src/attribute.o \
	${OBJECTDIR}/src/bigint.o \
	${OBJECTDIR}/src/css_parse.o \
	${OBJECTDIR}/src/css_property.o \
	${OBJECTDIR}/src/css_rule.o \
	${OBJECTDIR}/src/css_selector.o \
	${OBJECTDIR}/src/dom.o \
	${OBJECTDIR}/src/encoding.o \
	${OBJECTDIR}/src/error.o \
	${OBJECTDIR}/src/file_list.o \
	${OBJECTDIR}/src/formatter.o \
	${OBJECTDIR}/src/hashmap.o \
	${OBJECTDIR}/src/hashset.o \
	${OBJECTDIR}/src/jni.o \
	${OBJECTDIR}/src/log.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/master.o \
	${OBJECTDIR}/src/matrix.o \
	${OBJECTDIR}/src/matrix_queue.o \
	${OBJECTDIR}/src/memwatch.o \
	${OBJECTDIR}/src/node.o \
	${OBJECTDIR}/src/queue.o \
	${OBJECTDIR}/src/range.o \
	${OBJECTDIR}/src/range_array.o \
	${OBJECTDIR}/src/text_buf.o \
	${OBJECTDIR}/src/utils.o


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
LDLIBSOPTIONS=-lm /usr/lib/x86_64-linux-gnu/libicuio.so /usr/lib/x86_64-linux-gnu/libicuuc.so

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/formatter

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/formatter: /usr/lib/x86_64-linux-gnu/libicuio.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/formatter: /usr/lib/x86_64-linux-gnu/libicuuc.so

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/formatter: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/formatter ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/HTML.o: src/HTML.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/HTML.o src/HTML.c

${OBJECTDIR}/src/STIL/STIL.o: src/STIL/STIL.c 
	${MKDIR} -p ${OBJECTDIR}/src/STIL
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/STIL/STIL.o src/STIL/STIL.c

${OBJECTDIR}/src/STIL/cJSON.o: src/STIL/cJSON.c 
	${MKDIR} -p ${OBJECTDIR}/src/STIL
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/STIL/cJSON.o src/STIL/cJSON.c

${OBJECTDIR}/src/STIL/memory.o: src/STIL/memory.c 
	${MKDIR} -p ${OBJECTDIR}/src/STIL
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/STIL/memory.o src/STIL/memory.c

${OBJECTDIR}/src/annotation.o: src/annotation.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/annotation.o src/annotation.c

${OBJECTDIR}/src/attribute.o: src/attribute.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/attribute.o src/attribute.c

${OBJECTDIR}/src/bigint.o: src/bigint.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/bigint.o src/bigint.c

${OBJECTDIR}/src/css_parse.o: src/css_parse.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/css_parse.o src/css_parse.c

${OBJECTDIR}/src/css_property.o: src/css_property.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/css_property.o src/css_property.c

${OBJECTDIR}/src/css_rule.o: src/css_rule.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/css_rule.o src/css_rule.c

${OBJECTDIR}/src/css_selector.o: src/css_selector.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/css_selector.o src/css_selector.c

${OBJECTDIR}/src/dom.o: src/dom.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/dom.o src/dom.c

${OBJECTDIR}/src/encoding.o: src/encoding.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/encoding.o src/encoding.c

${OBJECTDIR}/src/error.o: src/error.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/error.o src/error.c

${OBJECTDIR}/src/file_list.o: src/file_list.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/file_list.o src/file_list.c

${OBJECTDIR}/src/formatter.o: src/formatter.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/formatter.o src/formatter.c

${OBJECTDIR}/src/hashmap.o: src/hashmap.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/hashmap.o src/hashmap.c

${OBJECTDIR}/src/hashset.o: src/hashset.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/hashset.o src/hashset.c

${OBJECTDIR}/src/jni.o: src/jni.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/jni.o src/jni.c

${OBJECTDIR}/src/log.o: src/log.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/log.o src/log.c

${OBJECTDIR}/src/main.o: src/main.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.c

${OBJECTDIR}/src/master.o: src/master.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/master.o src/master.c

${OBJECTDIR}/src/matrix.o: src/matrix.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/matrix.o src/matrix.c

${OBJECTDIR}/src/matrix_queue.o: src/matrix_queue.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/matrix_queue.o src/matrix_queue.c

${OBJECTDIR}/src/memwatch.o: src/memwatch.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memwatch.o src/memwatch.c

${OBJECTDIR}/src/node.o: src/node.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/node.o src/node.c

${OBJECTDIR}/src/queue.o: src/queue.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/queue.o src/queue.c

${OBJECTDIR}/src/range.o: src/range.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/range.o src/range.c

${OBJECTDIR}/src/range_array.o: src/range_array.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/range_array.o src/range_array.c

${OBJECTDIR}/src/text_buf.o: src/text_buf.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/text_buf.o src/text_buf.c

${OBJECTDIR}/src/utils.o: src/utils.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -DCOMMANDLINE -DMEMWATCH -Iinclude -Iinclude/STIL -I/usr/lib/jvm/java-8-openjdk-amd64/include -I/usr/lib/jvm/java-8-openjdk-amd64/include/linux -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/utils.o src/utils.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/formatter

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
