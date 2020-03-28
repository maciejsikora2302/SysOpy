#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h> 
#include <string.h>
#include <sys/wait.h>

#define SU SIGUSR1

// Arithemtic exception
#define SIG_1 SIGFPE
// Sent when child process finishes execution
#define SIG_2 SIGCHLD
// Sent when IO operation occurs 
#define SIG_3 SIGSEGV


/*
ucontext
This is a pointer to a ucontext_t structure, cast to void *.
The structure pointed to by this field contains signal context information that was saved on the user-space stack by the kernel; for details, see sigreturn(2).  
Further information about the ucontext_t structure can be found in getcontext(3).  
Commonly, the handler function doesn't make any use of the third argument.
*/

// siginfo_t {
// int      si_signo;     /* Signal number */
// int      si_errno;     /* An errno value */
// int      si_code;      /* Signal code */
// int      si_trapno;    /* Trap number that caused
//                             hardware-generated signal
//                             (unused on most architectures) */
// pid_t    si_pid;       /* Sending process ID */
// uid_t    si_uid;       /* Real user ID of sending process */
// int      si_status;    /* Exit value or signal */
// clock_t  si_utime;     /* User time consumed */
// clock_t  si_stime;     /* System time consumed */
// sigval_t si_value;     /* Signal value */
// int      si_int;       /* POSIX.1b signal */
// void    *si_ptr;       /* POSIX.1b signal */
// int      si_overrun;   /* Timer overrun count;
//                             POSIX.1b timers */
// int      si_timerid;   /* Timer ID; POSIX.1b timers */
// void    *si_addr;      /* Memory location which caused fault */
// long     si_band;      /* Band event (was int in
//                             glibc 2.3.2 and earlier) */
// int      si_fd;        /* File descriptor */
// short    si_addr_lsb;  /* Least significant bit of address
//                             (since Linux 2.6.32) */
// void    *si_lower;     /* Lower bound when address violation
//                             occurred (since Linux 3.19) */
// void    *si_upper;     /* Upper bound when address violation
//                             occurred (since Linux 3.19) */
// int      si_pkey;      /* Protection key on PTE that caused
//                             fault (since Linux 4.6) */
// void    *si_call_addr; /* Address of system call instruction
//                             (since Linux 3.5) */
// int      si_syscall;   /* Number of attempted system call
//                             (since Linux 3.5) */
// unsigned int si_arch;  /* Architecture of attempted system call
//                             (since Linux 3.5) */
// }



void sig1_handler(int sig, siginfo_t* info, void* ucontext) {
    printf("I'm working with signal nr: %d -> SIGFPE\n", info->si_signo);
    printf("Error number (errno): %d\n", info->si_errno);
    printf("|-----------------------------------------------------------------------|\n|    o   \\ o /  _ o         __|    \\ /     |__        o _  \\ o /   o    |\n|   /|\\    |     /\\   ___\\o   \\o    |    o/    o/__   /\\     |    /|\\   |\n|   / \\   / \\   | \\  /)  |    ( \\  /o\\  / )    |  (\\  / |   / \\   / \\   |\n|-----------------------------------------------------------------------|\n\n");
    exit(1);
}

void sig2_handler(int sig, siginfo_t* info, void* ucontext) {
    printf("I'm working with signal nr: %d -> SIGCHLD\n", info->si_signo);
    printf("Exit status or value: %d\n", info->si_status);
    printf("(\\\n \\'\\\n  \\'\\     __________  \n / '|   ()_________)\n \\ '/    \\ ~~~~~~~~ \\\n   \\       \\ ~~~~~~   \\\n   ==).      \\__________\\\n  (__)       ()__________)\n");
}

void sig3_handler(int sig, siginfo_t* info, void* ucontext) {
    printf("I'm working with signal nr: %d -> SIGSEGV\n", info->si_signo);
    printf("Fault adress: %p\n", info->si_addr);
    printf("           __..._   _...__\n  _..-\"      `Y`      \"-._\n  \\ Once upon |           /\n  \\\\  a time..|          //\n  \\\\\\         |         ///\n   \\\\\\ _..---.|.---.._ ///\n    \\\\`_..---.Y.---.._`//\n     '`               `'\n");
    exit(1);
}

int main(int argc, char** argv){
    // We install arithmetic exception handler.
    struct sigaction act;
    act.sa_sigaction = sig1_handler;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaction(SIG_1, &act, NULL);

    // We install child signal handler
    struct sigaction child_act;
    child_act.sa_sigaction = sig2_handler;
    child_act.sa_flags = SA_SIGINFO;
    sigemptyset(&child_act.sa_mask);
    sigaction(SIG_2, &child_act, NULL);

    // We install handler for io operations
    struct sigaction seg_act;
    seg_act.sa_sigaction = sig3_handler;
    seg_act.sa_flags = SA_SIGINFO;
    sigemptyset(&seg_act.sa_mask);
    sigaction(SIG_3, &seg_act, NULL);

    // Here we're provoking various errors.
    int forked = fork();
    // sleep(1);
    if (forked == 0) {
        int zero = 0;
        int foo = 2 / zero;
        printf("I managed to divide by zero! \\o\\ /o/ \\o/ ==> %d\n", foo);
        // sleep(1);
    }else{
        // sleep(1);
        // Wait for child signal
        sleep(1);
        char* foo = NULL;
        foo[5] = 'c';
        printf("Escaped segFault %s...\n", foo);
        return 0;
    }    
}   