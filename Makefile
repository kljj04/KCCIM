CC = gcc
CFLAGS = -Wall -Wextra -O2 -shared -DKCCIM_EXPORTS
TARGET = kccim.dll

# 새로 쪼갠 폴더 안의 파일들까지 전부 컴파일 대상에 추가!
SRCS = src/automata.c \
       src/kccim.c \
       src/table.c \


all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	del /Q $(TARGET)