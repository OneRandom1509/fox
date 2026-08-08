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

# ============================================================
# Debug Build
# ============================================================

debug:
	@echo -e "$(COLOR_YELLOW)▶ Configuring Debug Build...$(COLOR_RESET)"
	@$(CMAKE) -S . -B $(BUILD_DBG_DIR) -DCMAKE_BUILD_TYPE=Debug $(EXTRA_CMAKE_FLAGS)
	@echo -e "$(COLOR_YELLOW)▶ Building Debug Binary...$(COLOR_RESET)"
	@$(CMAKE) --build $(BUILD_DBG_DIR)
	@echo -e "$(COLOR_GREEN)✔ Debug build complete: $(BUILD_DBG_DIR)/$(FOX_BINARY)$(COLOR_RESET)"

clean:
	rm -rf $(BUILD_DIR) $(BUILD_DBG_DIR)
	rm -f $(AST_GEN)

rebuild: clean build

# ============================================================
# AST Generator
# ============================================================

AST_GEN := tools/generate_ast
AST_OUTPUT_EXPR := include/ast/Expr.hpp
AST_OUTPUT_STMT := include/ast/Stmt.hpp

$(AST_GEN): tools/generate_ast.cpp
	@echo -e "$(COLOR_BLUE)▶ Building AST generator...$(COLOR_RESET)"
	@g++ -std=c++20 tools/generate_ast.cpp -o $(AST_GEN)

ast-expr: $(AST_GEN)
	@echo -e "$(COLOR_BLUE)▶ Generating Expr AST headers...$(COLOR_RESET)"
	@./$(AST_GEN) include/ast 0
	@$(CLANG_FORMAT) -i $(AST_OUTPUT_EXPR)
	@echo -e "$(COLOR_GREEN)✔ Expr AST generated and formatted$(COLOR_RESET)"

ast-stmt: $(AST_GEN)
	@echo -e "$(COLOR_BLUE)▶ Generating Stmt AST headers...$(COLOR_RESET)"
	@./$(AST_GEN) include/ast 1
	@$(CLANG_FORMAT) -i $(AST_OUTPUT_STMT)
	@echo -e "$(COLOR_GREEN)✔ Stmt AST generated and formatted$(COLOR_RESET)"

