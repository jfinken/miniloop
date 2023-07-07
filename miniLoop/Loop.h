#ifndef SRC_MINILOOP_LOOP_H_
#define SRC_MINILOOP_LOOP_H_

#include <functional>

#include <memory>
#include <string>

#include <miniJSON/JSON.h>

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

class AlarmClock
{
public:
    static std::shared_ptr<AlarmClock> create(
      const JSON::Object& spec) noexcept;

    virtual ~AlarmClock() = default;

    virtual void onAlarm(std::function<void()> callback) noexcept;
    virtual void reset() noexcept = 0;

    class Alarm
    {
    public:
        enum Days
        {
            DAY_MONDAY = 1 << 0,
            DAY_TUESDAY = 1 << 1,
            DAY_WEDNESDAY = 1 << 2,
            DAY_THURSDAY = 1 << 3,
            DAY_FRIDAY = 1 << 4,
            DAY_SATURDAY = 1 << 5,
            DAY_SUNDAY = 1 << 6,
        };
        static const int DAY_EVERY = 0xFF;

        /** makes an invalid object (will never trig) */
        Alarm() = default;
        ~Alarm() = default;

        /**
         * { "hours": <int>, "minutes": <int>, "on": [ "mon", "tue", "wed",
         * "thu", "fri", "sat", "sun" ] } when "on" is not provided, occurs
         * everyday.
         */
        Alarm(const JSON::Object& alarmSpec);

             operator bool() const { return valid; }
        bool matches(int time, int day);

    private:
        bool valid{ false };
        int  days{ DAY_EVERY };
        int  time{ 0 };
    };

protected:
    std::function<void()> callback;
    bool                  triggered{ false };
};

/**
 * Utility class to trigger an action at a specified time and day of the week.
 */
class RecurrentAlarm : public AlarmClock
{
public:
    RecurrentAlarm() noexcept;
    RecurrentAlarm(const JSON::Object& params) noexcept;

    virtual ~RecurrentAlarm() noexcept override;

    virtual void reset() noexcept override;

    void set(Alarm a);

private:
    Alarm                        alarm;
    loop::Loop::RecurrentTimeout timeout{ 30000 };
};

/**
 * Utility class to trigger an action at a specified time after creation
 */
class OneShotAlarm : public AlarmClock
{
public:
    OneShotAlarm() noexcept;
    OneShotAlarm(const JSON::Object& params) noexcept;

    virtual ~OneShotAlarm() noexcept override;

    virtual void reset() noexcept override;

    void set(int after_ms) noexcept;

private:
    loop::Loop::Timeout timeout;
};

}

#endif /* SRC_MINILOOP_LOOP_H_ */
