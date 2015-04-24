# Microsoft Developer Studio Generated NMAKE File, Based on ImageSegmentation.dsp
!IF "$(CFG)" == ""
CFG=ImageSegmentation - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ImageSegmentation - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ImageSegmentation - Win32 Release" && "$(CFG)" != "ImageSegmentation - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ImageSegmentation.mak" CFG="ImageSegmentation - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ImageSegmentation - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ImageSegmentation - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ImageSegmentation - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\ImageSegmentation.exe" "$(OUTDIR)\ImageSegmentation.pch"


CLEAN :
	-@erase "$(INTDIR)\gradient.obj"
	-@erase "$(INTDIR)\ImageSegmentation.obj"
	-@erase "$(INTDIR)\ImageSegmentation.pch"
	-@erase "$(INTDIR)\ImageSegmentation.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\toboggan.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\watershed.obj"
	-@erase "$(OUTDIR)\ImageSegmentation.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ImageSegmentation.pch" /YX"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x404 /fo"$(INTDIR)\ImageSegmentation.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ImageSegmentation.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\ImageSegmentation.pdb" /machine:I386 /out:"$(OUTDIR)\ImageSegmentation.exe" 
LINK32_OBJS= \
	"$(INTDIR)\gradient.obj" \
	"$(INTDIR)\ImageSegmentation.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\toboggan.obj" \
	"$(INTDIR)\watershed.obj" \
	"$(INTDIR)\ImageSegmentation.res"

"$(OUTDIR)\ImageSegmentation.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ImageSegmentation - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\ImageSegmentation.exe" "$(OUTDIR)\ImageSegmentation.pch"


CLEAN :
	-@erase "$(INTDIR)\gradient.obj"
	-@erase "$(INTDIR)\ImageSegmentation.obj"
	-@erase "$(INTDIR)\ImageSegmentation.pch"
	-@erase "$(INTDIR)\ImageSegmentation.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\toboggan.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\watershed.obj"
	-@erase "$(OUTDIR)\ImageSegmentation.exe"
	-@erase "$(OUTDIR)\ImageSegmentation.ilk"
	-@erase "$(OUTDIR)\ImageSegmentation.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MLd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ImageSegmentation.pch" /YX"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x404 /fo"$(INTDIR)\ImageSegmentation.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ImageSegmentation.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\ImageSegmentation.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ImageSegmentation.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\gradient.obj" \
	"$(INTDIR)\ImageSegmentation.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\toboggan.obj" \
	"$(INTDIR)\watershed.obj" \
	"$(INTDIR)\ImageSegmentation.res"

"$(OUTDIR)\ImageSegmentation.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ImageSegmentation.dep")
!INCLUDE "ImageSegmentation.dep"
!ELSE 
!MESSAGE Warning: cannot find "ImageSegmentation.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ImageSegmentation - Win32 Release" || "$(CFG)" == "ImageSegmentation - Win32 Debug"
SOURCE=.\gradient.cpp

"$(INTDIR)\gradient.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ImageSegmentation.cpp

"$(INTDIR)\ImageSegmentation.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ImageSegmentation.rc

"$(INTDIR)\ImageSegmentation.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "ImageSegmentation - Win32 Release"

CPP_SWITCHES=/nologo /G6 /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ImageSegmentation.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\ImageSegmentation.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "ImageSegmentation - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MLd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ImageSegmentation.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\ImageSegmentation.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\toboggan.cpp

"$(INTDIR)\toboggan.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\watershed.cpp

"$(INTDIR)\watershed.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

