ARGS:=
ifeq ($(VERBOSE), 1)
    ARGS:=$(ARGS) -v
endif
ifneq ($(JOBS), )
    ARGS:=$(ARGS) -j $(JOBS)
endif

.PHONY: debug-docker
debug-docker: ## Build all debug distributables using docker - Default
	./tools/buildscripts/build-in-docker.sh $(ARGS) -d

.PHONY: release-docker
release-docker: ## Build all release distributables using docker
	./tools/buildscripts/build-in-docker.sh $(ARGS)

.PHONY: debug
debug: ## Build all debug distributables
	./tools/buildscripts/build.sh $(ARGS) -d

.PHONY: release
release: ## Build all release distributables
	./tools/buildscripts/build.sh $(ARGS)

.PHONY: clean
clean: ## Cleanup all previous build artifacts and distributables
	./tools/buildscripts/build.sh -C

.PHONY: runtests-docker
runtests-docker: ## Run unit tests in docker build environment
	./tools/tests/unit-in-docker.sh $(ARGS)

.PHONY: build-oci-images
build-containers: ## Build the runtime containers
	./tools/buildscripts/build-containers.sh

# Show help menu
.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
