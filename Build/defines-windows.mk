QMAKE  := C:\Qt\Qt5.14.2\5.14.2\msvc2017\bin\qmake.exe
JOM    := "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Tools\MSVC\14.16.27023\bin\Hostx86\x86\nmake.exe"
ENV_EXE := "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\IDE\CommonExtensions\Microsoft\TeamFoundation\Team Explorer\Git\usr\bin\env.exe"
SETUP_VCVAR  := "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars32.bat"
QT_ENV_PATH  := "C:\Qt\Qt5.14.2\5.14.2\msvc2017\bin\qtenv2.bat"

RM                            := $(TOOLS_DIR)\rm.exe
MKDIR                         := $(TOOLS_DIR)\mkdir.exe
CP                            := $(TOOLS_DIR)\cp.exe
CDDIR                         := cd

SPFlashToolV6.CCFLAGS    := -r -spec win32-msvc "CONFIG+=$(BUILD_TYPE)"

RELEASE_TOOL_PATH     := $(OUTPUT_PATH)/$(BUILD_TYPE)

PRO_FILE := $(WORKING_DIR)\SPFlashToolV6.pro

# Dependent files
SPFlashToolV6.Dependency.Files := \
 $(wildcard Libs/*.xml) \
 $(wildcard Libs/*.xsd) \
 $(wildcard Libs/*.ini) \
 $(wildcard Libs/*.dll)  \
 $(wildcard Libs/*.bin) \
 $(wildcard Libs/*.exe) \
 $(wildcard Libs/*.json) \
 $(wildcard Libs/QtWin/$(BUILD_TYPE)/*.dll)
 
# Dependent directory
SPFlashToolV6.Dependency.Dirs := \
 Libs/QtWin/$(BUILD_TYPE)/plugins

SPFlashToolV6.Output.Files := *.xml *.xsd *.ini *.dll *.bin *.exe *.json \
    plugins