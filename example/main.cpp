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

int main(/*int argc, char* argv[]*/) {

    // Subscribe to signals for example
    auto sigintEvt{ Loop::UNIX_SIGNAL(SIGINT) };
    sigintEvt.onEvent([](int) { graceful_exit(SIGINT); });

    auto sigtermEvt{ Loop::UNIX_SIGNAL(SIGTERM) };
    sigtermEvt.onEvent([](int) { graceful_exit(SIGTERM); });

    // Remember to take a break from screen every hour !
    auto takeBreak{ std::make_unique<Loop::RecurrentTimeout>(TIMEOUT_HOURLY) };
    takeBreak->onTimeout([](){
        std::cout << "Take a break !" << std::endl;
    });

    // Start the loop
    Loop::singleton().run();

    return EXIT_SUCCESS;
}