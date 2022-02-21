#include <stdio.h>
#include <time.h>

#include <signal.h>
#include <unistd.h> /* alarm() */

int lines = 3; /* Store how many lines we have. */

/* Print current time every second at first line.. */
void prnt_clock() {
    time_t tt;
    struct tm *tm;
    char buf[80];

    time(&tt);
    tm = localtime(&tt);
    strftime(buf, sizeof(buf), "%H:%M:%S", tm);
    fprintf(stdout,
            "\033[s"   /* Save cursor position. */
            "\033[%dA" /* Move cursor up d lines. */
            "\r"       /* Moves cursor to beginning of line. */
            "%s"       /* String to print. */
            "\033[u"   /* Restore cursor position. */
            ,
            lines,
            buf);
    fflush(stdout);

    alarm(1);
}

/* Print last command entered at third line. */
void prnt_cmd(char *cmd) {
    fprintf(stdout,
            "\033[s\033[%dA\r\033[KLast cmd: %s\033[u",
            lines - 2, cmd);
    fflush(stdout);
}

/* Empty the stdin line */
void process_input(char c) {
    char buf[32];
    int i = 0;

    ++lines;
    while (c != '\n' && i < 31) {
        buf[i++] = c;
        c = getchar();
    }
    buf[i] = 0x00;
    if (c != '\n')
        while (getchar() != '\n')
            ;
    prnt_cmd(buf);
}

/* Signal handler. */
void sig_alarm(int sig) {
    if (sig == SIGALRM) {
        signal(SIGALRM, SIG_IGN);
        prnt_clock();
        signal(SIGALRM, sig_alarm);
    }
}

int main(void /* int argc, char *argv[] */) {
    char prompt[16] = "\033[1;31m$\033[0m "; /* We want bold red $ */
    int c;

    signal(SIGALRM, sig_alarm);
    fprintf(stdout,
            "\n" /* line for clock  (line 1. )*/
            "---------------------------------------------------\n"
            "Enter q to quit.\n" /* line for status (line 3.) */
            "%s"                 /* prompt          (line 4+) */
            ,
            prompt);
    prnt_clock();

    while (1) {
        c = getchar();
        if (c == 'q')
            break;
        process_input(c);
        fprintf(stdout, "%s", prompt);
        fflush(stdout);
    }

    return 0;
}