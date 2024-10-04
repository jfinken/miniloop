#ifndef SRC_MINILOOP_LOOP_H_
#define SRC_MINILOOP_LOOP_H_

#include <functional>

#include <memory>
#include <string>


struct event;
struct event_base;

namespace loop {

/*!
 * \brief The Loop class is the eventloop for miniloop
 */
class Loop
{
public:
    static Loop& singleton();

    /*!
     *  \brief Setup libevent to use linux's pthread library
     */
    static void usePthreads();

    Loop();
    virtual ~Loop();

    /*!
     * \brief run start the loop.
     * It will run untill exit() is called.
     */
    void run();

    /*!
     * \brief exit Stops the loop.
     * It has no effect if the loop is not running.
     */
    void exit();

public:
    /*!
     * \brief The UNIX_SIGNAL class allows to listen to specific UNIX
     *  signals and perform actions when they are emitted.
     */
    class UNIX_SIGNAL
    {
    public:
        UNIX_SIGNAL(int id, Loop& loop = Loop::singleton());
        virtual ~UNIX_SIGNAL();

        /*!
         * \brief onEvent Sets the callback when the requested signal
         * is emitted.
         * \param callback the callback.
         */
        void onEvent(std::function<void(int)> callback);

        /*!
         * \brief setRequestedSignal Change the signal of interest
         * \param id The id of the signal (\see signum-generic.h)
         */
        void setRequestedSignal(int id);

        /*!
         * \brief getRequestedSignal
         * \return The ID of the requested signal
         */
        int getRequestedSignal() const;
        int getFd() const;

        explicit operator bool() const { return nullptr != event; }

    private:
        Loop&                    loop;
        std::function<void(int)> callback{ nullptr };
        int                      requested_id;
        struct event*            event{ nullptr };

        static void trampoline(int fd, short events, void* data);
    };

    /*!
     * \brief The IO class allows to listen on a file descriptor and
     *  perform an action when an event (read|write) happens on it.
     */
    class IO
    {
    public:
        static const int READ;
        static const int WRITE;

        IO(int fd, Loop& loop = Loop::singleton());
        virtual ~IO();

        /*!
         * \brief onEvent Sets the callback
         * \param callback the callback
         */
        void onEvent(std::function<void(int)> callback);

        /*!
         * \brief getFd Retrieves the file descriptor
         * \return the file descriptor
         */
        int getFd() const;

        /*!
         * \brief setFd Changes the file descriptor
         */
        void setFd(int);

        /*!
         * \brief setRequestedEvents change the event of interest
         *  Possible values are : READ, WRITE or READ | WRITE
         * \param events The event of interest.
         */
        void setRequestedEvents(int events);
        int  getRequestedEvents() const;

    private:
        Loop&                    loop;
        std::function<void(int)> callback{ nullptr };
        int                      requested_events{ 0 };
        struct event*            event{ nullptr };

        static void trampoline(int fd, short events, void* data);
    };

    /*!
     * \brief The Timeout class allows to perform an action when a
     * timeout occurs.
     */
    class Timeout
    {
    public:
        Timeout(Loop& loop = Loop::singleton());
        virtual ~Timeout();

        /*!
         * \brief onTimeout sets the callback
         * \param callback the callback
         */
        void onTimeout(std::function<void()> callback);

        /*!
         * \brief set Change the value of the timeout
         * \param after_ms the new timeout value (in milliseconds)
         */
        void set(int after_ms);

        /*!
         * \brief cancel Cancels the timeout
         */
        void cancel();

    private:
        Loop&                 loop;
        std::function<void()> callback{ nullptr };
        struct event*         event{ nullptr };
        static void           trampoline(int fd, short events, void* data);
    };

    /*!
     * \brief The RecurrentTimeout class allows to perform an action
     * on a timeout that can be programmed for specific day(s)/hour(s)
     */
    class RecurrentTimeout
    {
    public:
        RecurrentTimeout(int period_ms, Loop& loop = Loop::singleton());
        virtual ~RecurrentTimeout();

        /*!
         * \brief onTimeout set the action to perform when timeout occurs
         * \param callback the callback
         */
        void onTimeout(std::function<void()> callback);

    private:
        int                   period_ms;
        Timeout               underlyingTimeout;
        std::function<void()> callback{ nullptr };
    };

private:
    struct event_base* event_base{ nullptr };
};


}

#endif /* SRC_MINILOOP_LOOP_H_ */
