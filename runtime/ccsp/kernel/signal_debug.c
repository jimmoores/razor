/* Temporary debug: catch signals and print registers */
#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>

static struct sigaction old_segv, old_bus, old_trap;

static void debug_handler(int sig, siginfo_t *info, void *ucontext) {
    ucontext_t *ctx = (ucontext_t *)ucontext;
    char buf[512];
    int n = snprintf(buf, sizeof(buf),
        "\n*** SIGNAL %d at addr %p ***\n"
        "PC  = 0x%llx\n"
        "SP  = 0x%llx\n"
        "x0  = 0x%llx\n"
        "x1  = 0x%llx\n"
        "x25 = 0x%llx\n"
        "x28 = 0x%llx\n"
        "x30 = 0x%llx\n",
        sig, info->si_addr,
        (unsigned long long)ctx->uc_mcontext.pc,
        (unsigned long long)ctx->uc_mcontext.sp,
        (unsigned long long)ctx->uc_mcontext.regs[0],
        (unsigned long long)ctx->uc_mcontext.regs[1],
        (unsigned long long)ctx->uc_mcontext.regs[25],
        (unsigned long long)ctx->uc_mcontext.regs[28],
        (unsigned long long)ctx->uc_mcontext.regs[30]);
    write(2, buf, n);
    _exit(128 + sig);
}

/* Install AFTER ccsp's signal handlers */
__attribute__((constructor(65535)))
void install_debug_signals(void) {
    struct sigaction sa;
    sa.sa_sigaction = debug_handler;
    sa.sa_flags = SA_SIGINFO | SA_RESETHAND;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, &old_segv);
    sigaction(SIGBUS, &sa, &old_bus);
    sigaction(SIGTRAP, &sa, &old_trap);
}
