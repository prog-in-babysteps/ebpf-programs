# Target binary name
TARGET := minimal

# Compilers
CLANG ?= clang
GCC   ?= gcc

# Get architecture of the host machine
ARCH := $(shell uname -m | sed 's/x86_64/x86/')

# Compilation flags
BPF_CFLAGS := -g -O2 -target bpf -D__TARGET_ARCH_$(ARCH) -Wno-missing-declarations
USER_CFLAGS := -g -O2 -Wall
USER_LIBS := -lbpf -lelf

.PHONY: all clean run

# Default target
all: $(TARGET)

# 1. Generate vmlinux.h dynamically from the running Arch kernel
vmlinux.h:
        @echo "=> Generating vmlinux.h from running kernel..."
        bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h

# 2. Compile kernel-space eBPF code into an ELF object
%.bpf.o: %.bpf.c vmlinux.h
        @echo "=> Compiling BPF object: $<"
        $(CLANG) $(BPF_CFLAGS) -c $< -o $@

# 3. Generate the BPF skeleton header file using bpftool
%.skel.h: %.bpf.o
        @echo "=> Generating BPF skeleton: $@"
        bpftool gen skeleton $< > $@

# 4. Compile the final userspace application
$(TARGET): $(TARGET).c $(TARGET).skel.h
        @echo "=> Compiling userspace binary: $@"
        $(GCC) $(USER_CFLAGS) $< $(USER_LIBS) -o $@

# Utility command to run the compiled application
run: $(TARGET)
        @echo "=> Running eBPF application (requires sudo)..."
        sudo ./$(TARGET)

# Clean build artifacts
clean:
        @echo "=> Cleaning up build files..."
        rm -f vmlinux.h *.o *.skel.h $(TARGET)

