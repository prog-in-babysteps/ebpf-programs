#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include <bpf/libbpf.h>
#include "minimal.skel.h"

// Callback function to handle printing errors from libbpf
static int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args) {
    return vfprintf(stderr, format, args);
}

int main(int argc, char **argv) {
    struct minimal_bpf *skel;
    int err;

    // 1. Set up libbpf logging callback
    libbpf_set_print(libbpf_print_fn);

    // 2. Open and load the eBPF application skeleton
    skel = minimal_bpf__open_and_load();
    if (!skel) {
        fprintf(stderr, "Failed to open and load BPF skeleton\n");
        return 1;
    }

    // 3. Attach the eBPF programs to their configured tracepoints
    err = minimal_bpf__attach(skel);
    if (err) {
        fprintf(stderr, "Failed to attach BPF skeleton\n");
        goto cleanup;
    }

    printf("Successfully started! eBPF program attached to sys_enter_execve.\n");
    printf("Run 'sudo cat /sys/kernel/debug/tracing/trace_pipe' in another terminal to watch events.\n");
    printf("Press Ctrl+C to exit...\n");

    // 4. Keep the userspace program running so the eBPF program stays loaded
    while (1) {
        sleep(1);
    }

cleanup:
    // 5. Clean up and detach when terminating
    minimal_bpf__destroy(skel);
    return err < 0 ? -err : 0;
}

