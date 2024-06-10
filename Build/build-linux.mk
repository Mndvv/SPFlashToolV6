# tools definition
include Build/defines-$(HOST).mk

target: all

#step1: set up environment
.PHONY: set-up
set-up:
	$(MKDIR) -p $(OUTPUT_PATH)

#step2: qmake *.pro + make
all: set-up
	$(CDDIR) $(OUTPUT_PATH);$(PWD);$(QMAKE) $(SPFlashToolV6.SRC.Path) $(SPFlashToolV6.CCFLAGS);$(MAKE);
	$(CP) $(SPFlashToolV6.Dependency.Files) $(OUTPUT_PATH)
	$(CP) -R $(SPFlashToolV6.Dependency.Dirs) $(OUTPUT_PATH)
	@echo $(BUILD_TYPE) version build pass

.PHONY: clean
clean: 
	$(CDDIR) $(OUTPUT_PATH);$(PWD);$(MAKE) clean
	$(CDDIR) $(OUTPUT_PATH);$(PWD);$(RM) -rf $(SPFlashToolV6.Output.Files)
	$(CDDIR) $(..);$(PWD);$(RM) -rf $(BASE_OUTPUT_PATH)
