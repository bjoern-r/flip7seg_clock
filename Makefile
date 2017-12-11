include user.cfg
-include esp82xx/common.mf
-include esp82xx/main.mf

WEB_PORT     ?= 80
COM_PORT     ?= 7777
BACKEND_PORT ?= 7878
PAGE_OFFSET  ?= 65536     # 1048576
#ESPTOOL_PY = $(ESP_ROOT)/esptool/esptool.py
ESPTOOL_PY = esptool.py

% :
	$(warning This is the empty rule. Something went wrong.)
	@true

ifndef TARGET
$(info Modules were not checked out... use git clone --recursive in the future. Pulling now.)
$(shell git submodule update --init --recursive 2>/dev/null)
endif

# Example for a custom rule.
# Most of the build is handled in main.mf
.PHONY : showvars
showvars:
	$(foreach v, $(.VARIABLES), $(info $(v) = $($(v))))
	true

