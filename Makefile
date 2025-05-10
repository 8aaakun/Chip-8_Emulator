# =========  Makefile  =========
# 기본 설정 ---------------------------------------------------------
CXX       := g++
CXXFLAGS  := -std=c++17 -Wall -Wextra -O2              \
             $(shell sdl2-config --cflags)             \
             -I$(PROJECT_ROOT)/src                     # 추가 헤더가 있다면 여기에

LDFLAGS   := $(shell sdl2-config --libs)

PROJECT_ROOT := $(CURDIR)
SRC_DIR      := $(PROJECT_ROOT)/src
BUILD_DIR    := $(PROJECT_ROOT)/build
BIN_DIR      := $(PROJECT_ROOT)/bin

TARGET := $(BIN_DIR)/chip8         # 실행 파일 이름
SRCS   := $(wildcard $(SRC_DIR)/*.cpp)
OBJS   := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# 기본 규칙 ----------------------------------------------------------
all: $(TARGET)

# 실행 파일 링크
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDFLAGS)

# 개별 객체 파일 컴파일
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 필요 디렉터리 자동 생성
$(BUILD_DIR) $(BIN_DIR):
	@mkdir -p $@

# 청소 --------------------------------------------------------------
clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean
# =========  end  =========