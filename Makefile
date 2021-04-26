SHELL = /bin/sh

.SUFFIXES: .tar.bz2

ROOT_PATH := .

#PACKAGE_NAME := $(basename $(notdir $(CURDIR)))
PACKAGE_NAME := ArduinoCore-samc
PACKAGE_VERSION := 1.0.0

# -----------------------------------------------------------------------------
# packaging specific
PACKAGE_FOLDER := .

ifeq (postpackaging,$(findstring $(MAKECMDGOALS),postpackaging))
  PACKAGE_FILENAME=$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.bz2
  PACKAGE_CHKSUM := $(firstword $(shell shasum -a 256 "package/$(PACKAGE_FILENAME)"))
  PACKAGE_SIZE := $(firstword $(shell wc -c "package/$(PACKAGE_FILENAME)"))
endif

# end of packaging specific
# -----------------------------------------------------------------------------

.PHONY: all clean print_info postpackaging

# Arduino module packaging:
#   - exclude version control system files, here git files and folders .git, .gitattributes and .gitignore
#   - exclude 'extras' folder
all: clean print_info
	@echo ----------------------------------------------------------
	@echo "Packaging module."
	mkdir -p package
	tar --exclude=./.gitattributes \
		--exclude=./.travis.yml \
		--exclude=extras \
		--exclude=microchipstudio \
		--exclude=testing \
		--exclude=.git \
		--exclude=package \
		--exclude=./Makefile \
		-cjf "package/$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.bz2" "$(PACKAGE_FOLDER)"
	$(MAKE) --no-builtin-rules postpackaging -C .
	@echo ----------------------------------------------------------

clean:
	@echo ----------------------------------------------------------
	@echo  Cleanup
	-$(RM) package/$(PACKAGE_NAME)-*.tar.bz2 package/package_$(PACKAGE_NAME)_*.json package/test_package_$(PACKAGE_NAME)_*.json
	@echo ----------------------------------------------------------

print_info:
	@echo ----------------------------------------------------------
	@echo Building $(PACKAGE_NAME) using
	@echo "CURDIR              = $(CURDIR)"
	@echo "OS                  = $(OS)"
	@echo "SHELL               = $(SHELL)"
	@echo "PACKAGE_VERSION     = $(PACKAGE_VERSION)"
	@echo "PACKAGE_NAME        = $(PACKAGE_NAME)"

postpackaging:
	@echo "PACKAGE_CHKSUM      = $(PACKAGE_CHKSUM)"
	@echo "PACKAGE_SIZE        = $(PACKAGE_SIZE)"
	@echo "PACKAGE_FILENAME    = $(PACKAGE_FILENAME)"
	mkdir -p package
	cat extras/package_index.json.template | sed s/%%VERSION%%/$(PACKAGE_VERSION)/ | sed s/%%FILENAME%%/$(PACKAGE_FILENAME)/ | sed s/%%CHECKSUM%%/$(PACKAGE_CHKSUM)/ | sed s/%%SIZE%%/$(PACKAGE_SIZE)/ > package/package_$(PACKAGE_NAME)_$(PACKAGE_VERSION)_index.json
	@echo "package_$(PACKAGE_NAME)_$(PACKAGE_VERSION)_index.json created"