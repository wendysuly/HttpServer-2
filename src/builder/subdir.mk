################################################################################
# HttpServer
# DB, 2014
################################################################################

SRC_DIR_BUILDER = ./src/builder
OBJ_DIR_BUILDER = ./$(BIN_DIR)/$(SRC_DIR_BUILDER)

# Add inputs and outputs 
CPP_SRCS += \
$(SRC_DIR_BUILDER)/PageBuilder.cpp \
$(SRC_DIR_BUILDER)/Templater.cpp \
$(SRC_DIR_BUILDER)/HTMLDecorator.cpp

OBJS += \
$(OBJ_DIR_BUILDER)/PageBuilder.o \
$(OBJ_DIR_BUILDER)/Templater.o \
$(OBJ_DIR_BUILDER)/HTMLDecorator.o

# Apply rule
$(OBJ_DIR_BUILDER)/%.o: $(SRC_DIR_BUILDER)/%.cpp
	$(dir_guard)
	$(CC) $(CFLAGS) -o "$@" "$<"
