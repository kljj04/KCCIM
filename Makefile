CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2
CPPFLAGS ?= -DKCCIM_EXPORTS -Iinclude -Iinternal
LDFLAGS ?= -shared
TARGET = kccim.dll
SRCS = src/automata.c \
       src/kccim.c \
       src/table.c

.PHONY: all clean test
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRCS)

test:
	dotnet run --project tests/Kccim.Tests/Kccim.Tests.csproj

clean:
	-del /Q $(TARGET)
