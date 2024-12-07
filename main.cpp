#include <chrono>
#include <uv.h>

uv_loop_t *loop;
uv_timer_t gc_req;
uv_timer_t fake_job_req;

std::chrono::time_point<std::chrono::steady_clock> tp;
uint counter = 0;

void timer_cb(uv_timer_t* handle)
{
    ++counter;
    const auto now = std::chrono::steady_clock::now();
    printf("%u: %zu\n", counter, std::chrono::duration_cast<std::chrono::milliseconds>(now - tp).count());
}

void fake_timer_cb(uv_timer_t* handle)
{
    printf("stop\n");
}

int main() {
    printf("start\n");
    loop = uv_default_loop();
    uv_timer_init(loop, &gc_req);
    uv_unref((uv_handle_t*) &gc_req);
    uv_timer_start(&gc_req, timer_cb, 0, 100);
    tp = std::chrono::steady_clock::now();
    uv_timer_init(loop, &fake_job_req);
    uv_timer_start(&fake_job_req, fake_timer_cb, 10000, 0);
    return uv_run(loop, UV_RUN_DEFAULT);
}
