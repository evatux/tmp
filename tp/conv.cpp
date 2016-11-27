#include <stdlib.h>
#include <stdio.h>

#include <algorithm>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include <unistd.h>
#include <pthread.h>

#include "thread_pool.hpp"

struct convolution_t: public primitive_t {
    convolution_t(std::string name, int mb, int c = 100)
        : name_(name), mb_(mb), c_(c) { init_tasks(); }
    virtual std::vector<task_t *> tasks() override { return tasks_; }

private:
    std::string name_;
    int mb_, c_;
    std::vector<task_t *> tasks_;

    struct conv_task_t: public task_t {
        conv_task_t(convolution_t *conv, int mb_start, int mb_end)
            : conv_(conv), mb_start_(mb_start), mb_end_(mb_end) {}

        convolution_t *conv_;
        int mb_start_, mb_end_;

        virtual void operator()() {
            printf("[%lx] %s (%p) : %2d .. %2d\n", (long)pthread_self(),
                    conv_->name_.c_str(), conv_, mb_start_, mb_end_), fflush(0);
            usleep(conv_->c_*1000);
            conv_->event_.click();
        }
    };
    friend convolution_t::conv_task_t;

    void init_tasks() {
        int work_size = 2;
        int n_tasks = 0;
        for (int mb_cur = 0; mb_cur < mb_; mb_cur += work_size) {
            int mb_end = std::min(mb_cur + work_size, mb_);
            tasks_.push_back(new conv_task_t(this, mb_cur, mb_end));
            n_tasks++;
        }
        event_.reset(n_tasks);
    }
};

int main() {
    engine_t cpu;
    convolution_t c1("c1", 8, 2000);
    convolution_t c2("c2", 6, 2000);
    convolution_t c3("c3", 1, 200);
    convolution_t c4("c4", 1, 200);
    cpu.submit(&c1);
    cpu.submit(&c2, {&c1});
    cpu.submit(&c3, {&c1});
    cpu.submit(&c4, {&c3});
    cpu.wait();
    printf("exiting ... \n"), fflush(0);

    return 0;
}
