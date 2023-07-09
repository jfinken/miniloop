# miniLoop

**:star2: A lightweight C++ wrapper around libevent :star2:**

Nothing fancy, miniLoop is a quick and easy-to-use event-loop with basic functionalities :
- Listen on read/write on a file descriptor
- Set timeouts (oneshot and recurrent) 
- Register on unix signals
- ... that is pretty much it !

To use it, just start the loop and register your callbacks on the events of your choice.

It wraps [libevent](https://libevent.org/) and is therefore easily extendable to fit your needs.

## How to use

```
#include <miniLoop/Loop.h>
#include <signal.h>
#include <iostream>

#define TIMEOUT_HOURLY 3600 * 1000

int main(/*int argc, char* argv[]) {

    // Subscribe to SIGINT signal for example
    auto sigintEvt{ Loop::UNIX_SIGNAL(SIGINT) };
    sigintEvt.onEvent([](int) { /*graceful_exit(SIGINT);*/ });

    // Remember to take a break from screen every hour !
    auto takeBreak{ std::make_unique<Loop::RecurrentTimeout>(TIMEOUT_HOURLY) };
    takeBreak->onTimeout([](){
        std::cout << "Take a break !" << std::endl;
    }

    // Start the loop
    Loop::singleton().run();

    return EXIT_SUCCESS;
}
```


## How to install

- Install

```
cmake [-S ${path/to/src}] [-DCMAKE_NSTALL_PREFIX=${install/prefix}] .
make
make install
```

- Uninstall

```
make uninstall
```

## Dependencies

miniLoop depends on :
- [libevent](https://libevent.org/)
- [miniJSON](https://github.com/MericLuc/minijson), which is easy to install from [here](https://github.com/MericLuc/minijson).
