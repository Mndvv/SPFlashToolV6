# tools
QMAKE                         := /opt/Qt5.14.2/5.14.2/gcc_64/bin/qmake
RM                            := rm
MKDIR                         := mkdir
CDDIR                         := cd
CP                            := cp
PWD                           := pwd

SPFlashToolV6.CCFLAGS           := -r -spec linux-g++-64 CONFIG+=$(BUILD_TYPE)

# path
SPFlashToolV6.SRC.Path          := $(shell $(PWD))

# Dependent files
SPFlashToolV6.Dependency.Files := \
 $(wildcard Libs/*.xml) \
 $(wildcard Libs/*.xsd) \
 $(wildcard Libs/*.ini) \
 $(wildcard Libs/*.so)  \
 $(wildcard Libs/*.bin) \
 $(wildcard Libs/*.sh) \
 $(wildcard Libs/Release.json) \
 $(wildcard Libs/99-ttyacms.rules)
 
 # Dependent directory
SPFlashToolV6.Dependency.Dirs := \
 $(wildcard Libs/QtLinux/plugins) \
 Libs/QtLinux/lib

SPFlashToolV6.Output.Files := *.xml *.xsd *.ini *.so *.bin *.sh *.qhc *.qch *.txt \
	99-ttyacms.rules Release.json \
	lib plugins
