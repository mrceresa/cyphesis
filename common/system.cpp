// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

#include "log.h"
#include "debug.h"
#include "globals.h"

#include <iostream>

#if CYPHESIS_MD5_PASSWORDS
#  include <openssl/md5.h>
#endif

extern "C" {
    #include <sys/utsname.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <signal.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdio.h>
}

static const bool debug_flag = false;

const std::string get_hostname()
{
    struct utsname host_ident;
    if (uname(&host_ident) != 0) {
        return "UNKNOWN";
    }
    return std::string(host_ident.nodename);
}

extern "C" void signal_received(int signo)
{
    exit_flag = true;

#if defined(HAVE_SIGACTION)
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = SIG_IGN;
    sigaction(signo, &action, NULL);
#else
    signal(signo, SIG_IGN);
#endif
}

void interactive_signals()
{
#if defined(HAVE_SIGACTION)
    struct sigaction action;

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = signal_received;
    sigaction(SIGINT, &action, NULL);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = signal_received;
    sigaction(SIGTERM, &action, NULL);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = signal_received;
    sigaction(SIGQUIT, &action, NULL);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &action, NULL);
#else
    signal(SIGINT, signal_received);
    signal(SIGTERM, signal_received);
    signal(SIGQUIT, signal_received);
    signal(SIGPIPE, SIG_IGN);
#endif
}

void daemon_signals()
{
#if defined(HAVE_SIGACTION)
    struct sigaction action;

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = SIG_IGN;
    sigaction(SIGINT, &action, NULL);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = signal_received;
    sigaction(SIGTERM, &action, NULL);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = SIG_IGN;
    sigaction(SIGQUIT, &action, NULL);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &action, NULL);
#else
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, signal_received);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
#endif
}

int daemonise()
{
    int pid = fork();
    int new_stdio;
    int status = 0;
    int running = false;

    switch (pid) {
        case 0:
            // Child
            // Switch signal behavoir
            daemon_signals();
            // Get rid if stdio
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            // Get rid of controlling tty, and start new session
            setsid();
            // Open /dev/null on the stdio file descriptors to avoid problems
            new_stdio = open("/dev/null", O_RDWR);
            dup2(new_stdio, STDIN_FILENO);
            dup2(new_stdio, STDOUT_FILENO);
            dup2(new_stdio, STDERR_FILENO);
            break;
        case -1:
            // Error

            // We are not the daemon process
	    daemon_flag = false;

            log(ERROR, "Failed to fork() to go to the background.");

            break;
        default:
	    // Parent

            // We are not the daemon process
            daemon_flag = false;

	    // Install handler for SIGUSR1
#if defined(HAVE_SIGACTION)
            struct sigaction action;
            sigemptyset(&action.sa_mask);
            action.sa_flags = 0;
            action.sa_handler = signal_received;
            sigaction(SIGUSR1, &action, NULL);
#else
            signal(SIGUSR1, signal_usr1);
#endif

            if (wait4(pid, &status, 0, NULL) < 0) {
                running = true;
            } else {
                pid = -1;
            }

            if (running == true) {
                log(INFO, "Running");
            } else {
		int estatus = WEXITSTATUS(status);
		if (estatus == EXIT_SUCCESS) {
                    log(ERROR, "Cyphesis exited normally at initialisation.");
		} else if (estatus == EXIT_DATABASE_ERROR) {
                    log(ERROR, "Cyphesis was unable to connect it the database.");
		} else if (estatus == EXIT_SOCKET_ERROR) {
                    log(ERROR, "Cyphesis was unable to open a listen socket.");
		} else {
                    log(ERROR, "Cyphesis exited unexpectedly at initialisation.");
		}
                log(ERROR, "See syslog for details.");
            }

            break;
    }
    return pid;
}

void running()
{
    if (daemon_flag) {
        kill(getppid(), SIGUSR1);
    }
}

static const char hex_table[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

void encrypt_password(const std::string & pwd, std::string & hash)
{
#if CYPHESIS_MD5_PASSWORDS
    unsigned char buf[MD5_DIGEST_LENGTH + 1];
    MD5((const unsigned char *)pwd.c_str(), pwd.size(), buf);
    buf[16] = '\0';
    hash = "";
    for(int i = 0; i < 16; ++i) {
        hash.push_back(hex_table[buf[i] & 0xf]);
        hash.push_back(hex_table[(buf[i] & 0xf0) >> 4]);
    }
    return;
#else
    hash = pwd;
    return;
#endif
}
