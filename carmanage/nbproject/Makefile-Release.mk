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
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/base/Buffer.o \
	${OBJECTDIR}/base/Condition.o \
	${OBJECTDIR}/base/Epoll.o \
	${OBJECTDIR}/base/EventLoop.o \
	${OBJECTDIR}/base/EventLoopBucket.o \
	${OBJECTDIR}/base/Events.o \
	${OBJECTDIR}/base/Handle.o \
	${OBJECTDIR}/base/Log.o \
	${OBJECTDIR}/base/Mutex.o \
	${OBJECTDIR}/base/MutexLocker.o \
	${OBJECTDIR}/base/NetAddress.o \
	${OBJECTDIR}/base/Poll.o \
	${OBJECTDIR}/base/Poller.o \
	${OBJECTDIR}/base/PthreadSem.o \
	${OBJECTDIR}/base/Rwlock.o \
	${OBJECTDIR}/base/SigIgnore.o \
	${OBJECTDIR}/base/Socket.o \
	${OBJECTDIR}/base/TcpConnBucket.o \
	${OBJECTDIR}/base/TcpConnection.o \
	${OBJECTDIR}/base/TcpServer.o \
	${OBJECTDIR}/base/Thread.o \
	${OBJECTDIR}/base/ThreadPool.o \
	${OBJECTDIR}/base/Timer.o \
	${OBJECTDIR}/base/TimerHeap.o \
	${OBJECTDIR}/base/TimersScheduler.o \
	${OBJECTDIR}/base/Timestamp.o \
	${OBJECTDIR}/http/HttpCommand.o \
	${OBJECTDIR}/http/HttpCommon.o \
	${OBJECTDIR}/http/HttpReqHandle.o \
	${OBJECTDIR}/http/HttpRequest.o \
	${OBJECTDIR}/http/HttpResponse.o \
	${OBJECTDIR}/http/HttpServer.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/mdm/ConsistencyHash.o \
	${OBJECTDIR}/mdm/HashNode.o \
	${OBJECTDIR}/mdm/Md5.o \
	${OBJECTDIR}/mdm/MdmInfo.o \
	${OBJECTDIR}/mdm/MdmInfoLoader.o \
	${OBJECTDIR}/mdm/mcacheTask.o \
	${OBJECTDIR}/mdm/mcacheTaskHandle.o \
	${OBJECTDIR}/mdm/mcacheTaskServer.o \
	${OBJECTDIR}/mdm/memProtocol.o \
	${OBJECTDIR}/mysql/mysql_connection_pool.o \
	${OBJECTDIR}/mysql/mysqlwork.o \
	${OBJECTDIR}/utils/smswork.o \
	${OBJECTDIR}/utils/utilfile.o \
	${OBJECTDIR}/utils/utilstring.o


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
LDLIBSOPTIONS=-lglog -lpthread -ljsoncpp -lz -lm -ldl `pkg-config --libs libcurl` /www/server/mysql/lib/libmysqlclient.a  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/carmanage

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/carmanage: /www/server/mysql/lib/libmysqlclient.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/carmanage: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/carmanage ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/base/Buffer.o: base/Buffer.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Buffer.o base/Buffer.cpp

${OBJECTDIR}/base/Condition.o: base/Condition.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Condition.o base/Condition.cpp

${OBJECTDIR}/base/Epoll.o: base/Epoll.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Epoll.o base/Epoll.cpp

${OBJECTDIR}/base/EventLoop.o: base/EventLoop.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/EventLoop.o base/EventLoop.cpp

${OBJECTDIR}/base/EventLoopBucket.o: base/EventLoopBucket.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/EventLoopBucket.o base/EventLoopBucket.cpp

${OBJECTDIR}/base/Events.o: base/Events.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Events.o base/Events.cpp

${OBJECTDIR}/base/Handle.o: base/Handle.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Handle.o base/Handle.cpp

${OBJECTDIR}/base/Log.o: base/Log.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Log.o base/Log.cpp

${OBJECTDIR}/base/Mutex.o: base/Mutex.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Mutex.o base/Mutex.cpp

${OBJECTDIR}/base/MutexLocker.o: base/MutexLocker.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/MutexLocker.o base/MutexLocker.cpp

${OBJECTDIR}/base/NetAddress.o: base/NetAddress.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/NetAddress.o base/NetAddress.cpp

${OBJECTDIR}/base/Poll.o: base/Poll.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Poll.o base/Poll.cpp

${OBJECTDIR}/base/Poller.o: base/Poller.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Poller.o base/Poller.cpp

${OBJECTDIR}/base/PthreadSem.o: base/PthreadSem.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/PthreadSem.o base/PthreadSem.cpp

${OBJECTDIR}/base/Rwlock.o: base/Rwlock.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Rwlock.o base/Rwlock.cpp

${OBJECTDIR}/base/SigIgnore.o: base/SigIgnore.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/SigIgnore.o base/SigIgnore.cpp

${OBJECTDIR}/base/Socket.o: base/Socket.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Socket.o base/Socket.cpp

${OBJECTDIR}/base/TcpConnBucket.o: base/TcpConnBucket.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/TcpConnBucket.o base/TcpConnBucket.cpp

${OBJECTDIR}/base/TcpConnection.o: base/TcpConnection.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/TcpConnection.o base/TcpConnection.cpp

${OBJECTDIR}/base/TcpServer.o: base/TcpServer.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/TcpServer.o base/TcpServer.cpp

${OBJECTDIR}/base/Thread.o: base/Thread.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Thread.o base/Thread.cpp

${OBJECTDIR}/base/ThreadPool.o: base/ThreadPool.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/ThreadPool.o base/ThreadPool.cpp

${OBJECTDIR}/base/Timer.o: base/Timer.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Timer.o base/Timer.cpp

${OBJECTDIR}/base/TimerHeap.o: base/TimerHeap.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/TimerHeap.o base/TimerHeap.cpp

${OBJECTDIR}/base/TimersScheduler.o: base/TimersScheduler.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/TimersScheduler.o base/TimersScheduler.cpp

${OBJECTDIR}/base/Timestamp.o: base/Timestamp.cpp
	${MKDIR} -p ${OBJECTDIR}/base
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/base/Timestamp.o base/Timestamp.cpp

${OBJECTDIR}/http/HttpCommand.o: http/HttpCommand.cpp
	${MKDIR} -p ${OBJECTDIR}/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/http/HttpCommand.o http/HttpCommand.cpp

${OBJECTDIR}/http/HttpCommon.o: http/HttpCommon.cpp
	${MKDIR} -p ${OBJECTDIR}/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/http/HttpCommon.o http/HttpCommon.cpp

${OBJECTDIR}/http/HttpReqHandle.o: http/HttpReqHandle.cpp
	${MKDIR} -p ${OBJECTDIR}/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/http/HttpReqHandle.o http/HttpReqHandle.cpp

${OBJECTDIR}/http/HttpRequest.o: http/HttpRequest.cpp
	${MKDIR} -p ${OBJECTDIR}/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/http/HttpRequest.o http/HttpRequest.cpp

${OBJECTDIR}/http/HttpResponse.o: http/HttpResponse.cpp
	${MKDIR} -p ${OBJECTDIR}/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/http/HttpResponse.o http/HttpResponse.cpp

${OBJECTDIR}/http/HttpServer.o: http/HttpServer.cpp
	${MKDIR} -p ${OBJECTDIR}/http
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/http/HttpServer.o http/HttpServer.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/mdm/ConsistencyHash.o: mdm/ConsistencyHash.cpp
	${MKDIR} -p ${OBJECTDIR}/mdm
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mdm/ConsistencyHash.o mdm/ConsistencyHash.cpp

${OBJECTDIR}/mdm/HashNode.o: mdm/HashNode.cpp
	${MKDIR} -p ${OBJECTDIR}/mdm
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mdm/HashNode.o mdm/HashNode.cpp

${OBJECTDIR}/mdm/Md5.o: mdm/Md5.cpp
	${MKDIR} -p ${OBJECTDIR}/mdm
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mdm/Md5.o mdm/Md5.cpp

${OBJECTDIR}/mdm/MdmInfo.o: mdm/MdmInfo.cpp
	${MKDIR} -p ${OBJECTDIR}/mdm
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mdm/MdmInfo.o mdm/MdmInfo.cpp

${OBJECTDIR}/mdm/MdmInfoLoader.o: mdm/MdmInfoLoader.cpp
	${MKDIR} -p ${OBJECTDIR}/mdm
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mdm/MdmInfoLoader.o mdm/MdmInfoLoader.cpp

${OBJECTDIR}/mdm/mcacheTask.o: mdm/mcacheTask.cpp
	${MKDIR} -p ${OBJECTDIR}/mdm
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mdm/mcacheTask.o mdm/mcacheTask.cpp

${OBJECTDIR}/mdm/mcacheTaskHandle.o: mdm/mcacheTaskHandle.cpp
	${MKDIR} -p ${OBJECTDIR}/mdm
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mdm/mcacheTaskHandle.o mdm/mcacheTaskHandle.cpp

${OBJECTDIR}/mdm/mcacheTaskServer.o: mdm/mcacheTaskServer.cpp
	${MKDIR} -p ${OBJECTDIR}/mdm
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mdm/mcacheTaskServer.o mdm/mcacheTaskServer.cpp

${OBJECTDIR}/mdm/memProtocol.o: mdm/memProtocol.cpp
	${MKDIR} -p ${OBJECTDIR}/mdm
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mdm/memProtocol.o mdm/memProtocol.cpp

${OBJECTDIR}/mysql/mysql_connection_pool.o: mysql/mysql_connection_pool.cpp
	${MKDIR} -p ${OBJECTDIR}/mysql
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mysql/mysql_connection_pool.o mysql/mysql_connection_pool.cpp

${OBJECTDIR}/mysql/mysqlwork.o: mysql/mysqlwork.cpp
	${MKDIR} -p ${OBJECTDIR}/mysql
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/mysql/mysqlwork.o mysql/mysqlwork.cpp

${OBJECTDIR}/utils/smswork.o: utils/smswork.cpp
	${MKDIR} -p ${OBJECTDIR}/utils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils/smswork.o utils/smswork.cpp

${OBJECTDIR}/utils/utilfile.o: utils/utilfile.cpp
	${MKDIR} -p ${OBJECTDIR}/utils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils/utilfile.o utils/utilfile.cpp

${OBJECTDIR}/utils/utilstring.o: utils/utilstring.cpp
	${MKDIR} -p ${OBJECTDIR}/utils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Iutils -Ibase -Ihttp -Imdm -I/www/server/mysql/include -Imysql `pkg-config --cflags libcurl` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils/utilstring.o utils/utilstring.cpp

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
