# tools definition
include Build/defines-$(HOST).mk

target: all

#step1: set up Qt environment
.PHONY: set-up-qt
set-up-qt:
	@$(QT_ENV_PATH)

#step1: set up environment
.PHONY: set-up
set-up:
	$(SETUP_VCVAR)
	$(MKDIR) -p $(OUTPUT_PATH)

#step2: qmake *.pro + make
all: set-up-qt set-up
	$(CDDIR) $(OUTPUT_PATH) & $(QMAKE) $(PRO_FILE) $(SPFlashToolV6.CCFLAGS)
	$(CDDIR) $(OUTPUT_PATH) & $(SETUP_VCVAR) & $(ENV_EXE) -u MAKE -u MAKEFLAGS $(JOM)
	$(CP) $(SPFlashToolV6.Dependency.Files) $(RELEASE_TOOL_PATH)
	$(CP) -R $(SPFlashToolV6.Dependency.Dirs) $(RELEASE_TOOL_PATH)
	@echo $(BUILD_TYPE) version build pass
	
.PHONY: clean
clean: 
	$(CDDIR) $(OUTPUT_PATH) & $(SETUP_VCVAR) & $(ENV_EXE) -u MAKE -u MAKEFLAGS $(JOM) clean
	$(CDDIR) $(RELEASE_TOOL_PATH) & $(RM) -rf $(SPFlashToolV6.Output.Files)
	$(CDDIR) $(..) & $(RM) -rf $(BASE_OUTPUT_PATH)
