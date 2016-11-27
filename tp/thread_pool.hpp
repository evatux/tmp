#include <stdlib.h>
#include <stdio.h>

#include <atomic>
#include <deque>
#include <vector>
#include <thread>
#include <mutex>
#include <iostream>

struct event_t {
    event_t(int ntasks = 0): ntasks_(ntasks), clicks_(ntasks_) {}
    inline void click() { --clicks_; }
    inline bool ready() const { return clicks_ == 0; }
    void wait() const { while(!ready()) std::this_thread::yield(); }
    void reset() { clicks_.store(ntasks_); }
    void reset(int ntasks) { ntasks_ = ntasks; reset(); }
private:
    int ntasks_;
    std::atomic<int> clicks_;
};

struct task_t { virtual void operator()() = 0; };

struct primitive_t {
    virtual std::vector<task_t *> tasks() = 0;

    inline event_t &event() { return event_; }
    inline const event_t &event() const { return event_; }
protected:
    event_t event_;
};

struct thread_pool_t {
    thread_pool_t(int nthr, task_t *idle_callback = nullptr)
        : nthr_(nthr), idle_time_(20), stop_(false)
        , idle_callback_(idle_callback)
    {
        for (int ithr = 0; ithr < nthr_; ++ithr) {
            workers_.emplace_back(std::thread(&thread_pool_t::worker, this));
        }
    }
    ~thread_pool_t() { stop_ = true; for(auto &w: workers_) w.join(); }

    void submit(std::vector<task_t *> &tasks) {
        qmutex_.lock();
        work_queue_.insert(work_queue_.end(), tasks.begin(), tasks.end());
        qmutex_.unlock();
    }
    void wait() { while(!work_queue_.empty()) std::this_thread::yield(); }

    void set_idle_callback(task_t *_) { idle_callback_ = _; }

private:
    void worker() {
        int idle = 0;
        while(!stop_ || !work_queue_.empty()) {
            if (work_queue_.empty()) {
                std::this_thread::yield();
                if (++idle == idle_time_) {
                    idle = 0;
                    if (idle_callback_) (*idle_callback_)();
                };
                continue;
            }

            qmutex_.lock();
            if (!work_queue_.empty()) {
                task_t *task = work_queue_.at(0);
                work_queue_.pop_front();
                qmutex_.unlock();
                (*task)();
                idle = 0;
            }
            qmutex_.unlock();
        }
    }

    int nthr_;
    int idle_time_;
    volatile bool stop_;
    task_t *idle_callback_;

    std::vector<std::thread> workers_;
    std::deque<task_t *> work_queue_;
    std::mutex qmutex_;
};

struct scheduler_t: public task_t {
    scheduler_t(int nthr): thread_pool_(nthr, this) {}
    ~scheduler_t() { wait(); thread_pool_.set_idle_callback(nullptr); }

    typedef std::vector<primitive_t *> vector_deps_t;
    void submit(primitive_t *p, const vector_deps_t &deps) {
        m_.lock();
        processed_.push_back(false);
        net_p_.push_back(p);
        net_deps_.push_back(deps);
        process(true);
        m_.unlock();
    }

    void wait() {
        if (processed_.empty()) return;
        while (true) {
            bool done = true;
            for (bool b: processed_) done &= b;
            if (done) break;
            std::this_thread::yield();
        }
        thread_pool_.wait();
    }

private:
    bool ready_to_run(int idx) const {
        bool ready = true;
        for (auto dep: net_deps_[idx]) {
            ready &= dep->event().ready();
        }
        return ready;
    }

    void process(bool under_lock = false) {
        if (!under_lock) m_.lock();
        // check whether something can be put in thread_pool_
        for (size_t i = 0; i < net_p_.size(); ++i) {
            if (!processed_[i] && ready_to_run(i)) {
                std::vector<task_t *> t_vec = net_p_[i]->tasks();
                thread_pool_.submit(t_vec);
                processed_[i] = true;
            }
        }
        if (!under_lock) m_.unlock();
    }
    virtual void operator()() { process(false); }

    std::vector<bool> processed_;
    std::vector<primitive_t *> net_p_;
    std::vector<vector_deps_t> net_deps_;
    std::mutex m_;

    thread_pool_t thread_pool_;
};

struct engine_t {
    engine_t(int nthr = 4): scheduler_(nthr) {}

    using vector_deps_t = scheduler_t::vector_deps_t;
    void submit(primitive_t *p, const vector_deps_t &deps) {
        scheduler_.submit(p, deps);
    }
    void submit(primitive_t *p) { submit(p, vector_deps_t()); }

    void wait() { scheduler_.wait(); }

private:
    scheduler_t scheduler_;
};
