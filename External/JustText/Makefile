# MARK: - Build and Test

name = JustText

JustText.headers = JustText.h Internal/*.h

# Standard build directory name
buildFolder = build
testFolder = build/tests

# Discover all library headers
CPPFLAGS += -I"." -I"Internal"

# Using a gnu extension or C++20 standard is important
CFLAGS += -std=gnu17
# Debug symbols and warnings
CFLAGS += -g
CFLAGS += -Wall -Wextra

.PHONY: test
# runs the primary test
test: test/test

.PHONY: test/%
# runs test of the user choosing (for now only one)
test/%: $(testFolder)/%
	$<

.PRECIOUS: $(testFolder)/%
$(testFolder)/%: Tests/%.c | $(testFolder)
	$(LINK.c) $< -o $@


# MARK: - Documentation

catalog = JustText.docc
symbolsFolder = $(buildFolder)/SymbolGraphs
symbolsFile = $(symbolsFolder)/JustText.symbols.json
archiveFile = $(buildFolder)/JustText.doccarchive

# flags to the compiler building the api symbols tree
SYMFLAGS += --product-name=$(name)
SYMFLAGS += -extract-api

# flags to the docc building the final documentation archive
DOCFLAGS += --additional-symbol-graph-dir $(symbolsFolder)

xcrun = $(shell which xcrun)
# docc is a documentation builder for swift and swift related languages
docc = $(if $(xcrun),$(shell xcrun --find docc),$(shell which docc))
# building of documentation is only supported with clang
clang = $(if $(xcrun),$(shell xcrun --find clang),$(shell which clang))

ifneq ($(docc),) # if docc variable is not empty
ifneq ($(clang),) # and if clang is found as well
# define new target, called docs for building documentation
.PHONY: docs
docs: $(archiveFile)

$(archiveFile): $(symbolsFile) $(catalog) | $(buildFolder)
	$(docc) convert $(catalog) $(DOCFLAGS) -o $@

$(symbolsFile): $(JustText.headers) | $(symbolsFolder)
	$(clang) $(CPPFLAGS) $(CFLAGS) $(SYMFLAGS) $^ -o $@

$(symbolsFolder): | $(buildFolder)
	mkdir $@

endif
endif


# MARK: - Maintenance and cleanup

$(testFolder):| $(buildFolder)
	mkdir $@

$(buildFolder):
	mkdir $@

.PHONY: clean
clean:
	$(RM) -r $(buildFolder)
