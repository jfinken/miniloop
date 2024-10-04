#include <miniLoop/Loop.h>

#include <iostream>
#include <memory>

#include <signal.h>
#include <string.h>

#define TIMEOUT_HOURLY 3600 * 1000

using namespace loop;

static void
graceful_exit(int signal_id) {
    std::cout << "Graceful exit after receiving '"<< strsignal(signal_id) << "' signal" << std::endl;
    Loop::singleton().exit();
}

static int fd = 3;
static void
on_fd_event(int rw) {
    std::cout << "Event on FD " << fd << ": " << rw << "\n";
}
static void
on_sigusr1_event(int sig_id) {
    std::cout << "SIGUSR " << sig_id << "\n";
}

int main(/*int argc, char* argv[]*/) {

    // Subscribe to signals for example
    auto sigintEvt{ Loop::UNIX_SIGNAL(SIGINT) };
    sigintEvt.onEvent([](int) { graceful_exit(SIGINT); });

    auto sigtermEvt{ Loop::UNIX_SIGNAL(SIGTERM) };
    sigtermEvt.onEvent([](int) { graceful_exit(SIGTERM); });

    // register listening to FD 3 and echo events
    auto fd_event{ Loop::IO(fd) };
    fd_event.setRequestedEvents(Loop::IO::READ | Loop::IO::WRITE);
    fd_event.onEvent([](int) { on_fd_event(Loop::IO::READ | Loop::IO::WRITE); });
    /*
    
    $ exec 3> output.txt
    $ echo "Hello, World!" >&3
    $ exec 3>&- 

    no worky, may be hitting this?
    https://stackoverflow.com/a/5471740
    */

    // $ kill -USR1 <pid-of-this-program>
    auto sigusr1_event{ Loop::UNIX_SIGNAL(SIGUSR1) };
    sigusr1_event.onEvent([](int) { on_sigusr1_event(SIGUSR1); });

    // Start the loop
    Loop::singleton().run();

    return EXIT_SUCCESS;
}
