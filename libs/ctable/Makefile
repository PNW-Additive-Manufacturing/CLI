BUILD_DIR   := ./bin
SRC_DIRS    := ./src

CFLAGS      := -MMD -MP -DDEBUG -std=c99 -Wall -Wextra -Werror -pedantic -g3 -O0
LDFLAGS     := 

# Use a platform check to handle the `find` command differently
ifeq ($(OS),Windows_NT)
    # Windows `find` workaround (PowerShell or cmd approach)
	FIND_SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
	FIND_INC_DIRS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)))
else
    # Unix/Linux find commands
	FIND_SRCS = $(shell find $(SRC_DIRS) -name *.c)
	FIND_INC_DIRS = $(shell find $(SRC_DIRS) -type d)
endif

SRCS := $(FIND_SRCS)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(FIND_INC_DIRS))

# Default target now just builds object files
all: $(OBJS)
	@echo "Object files are built in $(BUILD_DIR)"

# Rule to build object files
$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	@$(CC) $(INC_FLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)