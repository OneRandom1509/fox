PROJECT_NAME := fox
FOX_BINARY := fox

SRC_DIRS := src include
BUILD_DIR := build
BUILD_DBG_DIR := build-dbg

# Tools
CLANG_FORMAT := clang-format
CLANG_TIDY   := clang-tidy
MAKE  := make
CMAKE := cmake
CCACHE := ccache

EXTRA_CMAKE_FLAGS ?=

COMPILE_COMMANDS := $(BUILD_DIR)/compile_commands.json

# Find sources
CPP_FILES := $(shell find $(SRC_DIRS) -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" \))
HDR_FILES := $(shell find $(SRC_DIRS) -type f \( -name "*.h" -o -name "*.hpp" \))

# ============================================================
# ANSI Colors
# ============================================================

COLOR_RESET := \033[0m
COLOR_BOLD  := \033[1m
COLOR_RED   := \033[31m
COLOR_GREEN := \033[32m
COLOR_YELLOW:= \033[33m
COLOR_BLUE  := \033[34m
COLOR_CYAN  := \033[36m


# ============================================================
# Release Build
# ============================================================

build:
	@echo -e "\033[34m▶ Configuring...\033[0m"
	@$(CMAKE) -S . -B $(BUILD_DIR)
	@echo -e "\033[34m▶ Building Release...\033[0m"
	@$(CMAKE) --build $(BUILD_DIR)
	@echo -e "\033[32m✔ Release build complete\033[0m"

clean:
	rm -rf $(BUILD_DIR)

# ============================================================
# AST Generator
# ============================================================

AST_GEN := tools/generate_ast
AST_OUTPUT := include/Expr.hpp

$(AST_GEN): tools/generate_ast.cpp
	@echo -e "$(COLOR_BLUE)▶ Building AST generator...$(COLOR_RESET)"
	@g++ -std=c++20 tools/generate_ast.cpp -o $(AST_GEN)

ast: $(AST_GEN)
	@echo -e "$(COLOR_BLUE)▶ Generating AST headers...$(COLOR_RESET)"
	@./$(AST_GEN) include
	@$(CLANG_FORMAT) -i $(AST_OUTPUT)
	@echo -e "$(COLOR_GREEN)✔ AST generated and formatted$(COLOR_RESET)"
