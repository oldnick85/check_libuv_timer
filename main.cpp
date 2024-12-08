#include <chrono>
#include <uv.h>

constexpr uint64_t timeout_ms = 100;    // repeat timeout
constexpr uint64_t repeat_count = 101;    // repeat count

// =====================================================================
// == ACCURATE TIMER WITH CALLBACKS CALLS WITH EXACT (almost) TIMEOUT ==
// =====================================================================
uv_timer_t timer_accurate_req;
uint timer_accurate_counter = 0;
std::chrono::time_point<std::chrono::steady_clock> timer_accurate_start_tp;
std::chrono::time_point<std::chrono::steady_clock> timer_accurate_last_tp;

void timer_accurate_cb(uv_timer_t* handle)
{
    if (timer_accurate_counter == 0)
    {
        timer_accurate_start_tp = std::chrono::steady_clock::now();
        timer_accurate_last_tp = timer_accurate_start_tp;
    }
    const auto now = std::chrono::steady_clock::now();
    if (timer_accurate_counter % 10 == 0)
    {
        const auto tm_from_start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - timer_accurate_start_tp).count();
        printf("accurate %.3u: %zu\n", timer_accurate_counter, tm_from_start_ms);
    }
    ++timer_accurate_counter;
    usleep(1000);
    if (timer_accurate_counter >= repeat_count)
    {
        return;
    }

    timer_accurate_last_tp += std::chrono::milliseconds{timeout_ms};
    uint64_t delta = std::chrono::duration_cast<std::chrono::microseconds>(timer_accurate_last_tp - now).count();
    uv_timer_start(&timer_accurate_req, timer_accurate_cb, (delta + (1000 / 2)) / 1000, 0);
}

// =====================================================================
// ==           STANDARD LIBUV REPEATING TIMER CALLBACKS              ==
// =====================================================================
uv_timer_t timer_libuv_req;
uint timer_libuv_counter = 0;
std::chrono::time_point<std::chrono::steady_clock> timer_libuv_start_tp;

void timer_libuv_cb(uv_timer_t* handle)
{
    if (timer_libuv_counter == 0)
    {
        timer_libuv_start_tp = std::chrono::steady_clock::now();
    }
    const auto now = std::chrono::steady_clock::now();
    if (timer_libuv_counter % 10 == 0)
    {
        const auto tm_from_start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - timer_libuv_start_tp).count();
        printf("libuv %.3u: %zu\n", timer_libuv_counter, tm_from_start_ms);
    }
    ++timer_libuv_counter;
    if (timer_libuv_counter >= repeat_count)
    {
        uv_timer_stop(handle);
        return;
    }
    usleep(1000);
}

// =====================================================================
// ==                      JUST LIMITING TIMER                        ==
// =====================================================================
uv_loop_t *loop;
uv_timer_t fake_job_req;

void fake_timer_cb(uv_timer_t* handle)
{
    printf("stop\n");
}

// =====================================================================
// ==                          MAIN FUNCTION                          ==
// =====================================================================
int main() 
{
    // get uv loop
    loop = uv_default_loop();
    // make standard timer
    uv_timer_init(loop, &timer_libuv_req);
    uv_unref((uv_handle_t*) &timer_libuv_req);
    uv_timer_start(&timer_libuv_req, timer_libuv_cb, timeout_ms/2, timeout_ms);
    // make accurate timer
    uv_timer_init(loop, &timer_accurate_req);
    uv_unref((uv_handle_t*) &timer_accurate_req);
    uv_timer_start(&timer_accurate_req, timer_accurate_cb, timeout_ms, 0);
    // make limiting timer
    uv_timer_init(loop, &fake_job_req);
    uv_timer_start(&fake_job_req, fake_timer_cb, timeout_ms*(repeat_count+10), 0);
    // start loop
    printf("start\n");
    return uv_run(loop, UV_RUN_DEFAULT);
}