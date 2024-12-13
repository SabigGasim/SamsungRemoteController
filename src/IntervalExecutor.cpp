#include <functional>
#include <tuple>
#include <chrono>
#include <atomic>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

template <typename... Args>
class IntervalExecutor {
public:
    IntervalExecutor(int64_t interval, std::function<void(Args...)> callback)
        : m_Interval(interval),
          m_LastTimeCalled(0),
          m_IsRunning(false),
          m_Callback(std::move(callback)),
          m_TaskHandle(nullptr) {}

    void Execute(Args... args) {
        m_Args = std::make_tuple(std::forward<Args>(args)...);
        m_LastTimeCalled = xTaskGetTickCount();

        if (m_IsRunning.load()) {
            return;
        }

        if (m_TaskHandle != nullptr) {
            vTaskDelete(m_TaskHandle);
        }

        m_IsRunning.store(true);
        xTaskCreate(TaskFunction, "IntervalTask", 2048, this, 1, &m_TaskHandle);
    }

    ~IntervalExecutor() {
        if (m_IsRunning.load() && m_TaskHandle != nullptr) {
            vTaskDelete(m_TaskHandle);
        }
    }

private:
    std::tuple<Args...> m_Args;
    int64_t m_Interval;
    TickType_t m_LastTimeCalled;
    std::atomic<bool> m_IsRunning;
    std::function<void(Args...)> m_Callback;
    TaskHandle_t m_TaskHandle;

    static void TaskFunction(void* arg) {
        IntervalExecutor* self = static_cast<IntervalExecutor*>(arg);

        while (true) {
            vTaskDelay(pdMS_TO_TICKS(self->m_Interval));

            TickType_t now = xTaskGetTickCount();
            if ((now - self->m_LastTimeCalled) >= pdMS_TO_TICKS(self->m_Interval)) {
                std::apply(self->m_Callback, self->m_Args);
                break;
            }
        }

        self->m_IsRunning.store(false);
        self->m_TaskHandle = nullptr;
        vTaskDelete(nullptr);
    }
};
