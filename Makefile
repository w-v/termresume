EXEC := tres
SRC_DIR := src
OBJ_DIR := obj
H_DIR := include
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -lnotcurses-core -lnotcurses# -lm -lpthread
CPPFLAGS := -Wall -g -I./$(H_DIR)
#CXXFLAGS := 

$(EXEC): $(OBJ_FILES)
	   gcc -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	   gcc $(LIB_I) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(H_DIR)/%.h 
	   gcc $(LIB_I) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c -o $@ $<
clean: 
	rm $(OBJ_DIR)/* $(EXEC)
debug:
	@echo $(OBJ_FILES)
	@echo $(SRC_FILES)

deps:
	git clone https://github.com/recp/cglm
	cp -r cglm/include/cglm include
	rm -rf cglm
	git clone https://github.com/w-v/term3d_c include


