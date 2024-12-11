#include <functional>
#include <tuple>
#include <chrono>
#include <atomic>
#include <pthread.h>
#include <thread>

template <typename... Args>
class IntervalExecutor {
public:
    IntervalExecutor(int64_t interval, std::function<void(Args...)> callback)
        : m_Interval(std::chrono::milliseconds(interval)),
          m_LastTimeCalled(std::chrono::high_resolution_clock::now()),
          m_IsRunning(false),
          m_Callback(std::move(callback)),
          m_WorkerThread()
    {
        pthread_attr_init(&m_ThreadAttr);

        size_t stackSize = 8 * 1024;
        pthread_attr_setstacksize(&m_ThreadAttr, stackSize);
    }

    void Execute(Args... args) {
        m_Args = std::make_tuple(std::forward<Args>(args)...);
        m_LastTimeCalled = std::chrono::high_resolution_clock::now();

        if (m_IsRunning.load()) {
            return;
        }

        if (m_WorkerThread) {
            pthread_join(m_WorkerThread, nullptr);
        }

        pthread_create(&m_WorkerThread, &m_ThreadAttr, ThreadFunction, this);
    }

    ~IntervalExecutor() {
        if (m_IsRunning.load() && m_WorkerThread) {
            pthread_join(m_WorkerThread, nullptr);
        }
        pthread_attr_destroy(&m_ThreadAttr);
    }
    
private:
    typedef std::chrono::_V2::system_clock::time_point timepoint;

    std::tuple<Args...> m_Args;
    std::chrono::milliseconds m_Interval;
    timepoint m_LastTimeCalled;
    std::atomic<bool> m_IsRunning;
    std::function<void(Args...)> m_Callback;
    pthread_t m_WorkerThread;
    pthread_attr_t m_ThreadAttr;

    static void* ThreadFunction(void* arg) {
        IntervalExecutor* self = static_cast<IntervalExecutor*>(arg);
        self->m_IsRunning.store(true);

        while (true) {
            std::this_thread::sleep_for(self->m_Interval);
            auto now = std::chrono::high_resolution_clock::now();
            if (now - self->m_LastTimeCalled >= self->m_Interval) {
                std::apply(self->m_Callback, self->m_Args);
                break;
            }
        }

        self->m_IsRunning.store(false);
        return nullptr;
    }    
};
