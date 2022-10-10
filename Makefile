ARGS:=
ifeq ($(VERBOSE), 1)
    ARGS:=$(ARGS) -v
endif
ifneq ($(JOBS), )
    ARGS:=$(ARGS) -j $(JOBS)
endif

debug: ## Build all debug distributables - Default
	./tools/buildscripts/build.sh $(ARGS) -d

release: ## Build all release distributables
	./tools/buildscripts/build.sh $(ARGS)

clean: ## Cleanup all previous build artifacts and distributables
	./tools/buildscripts/build.sh -C

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
