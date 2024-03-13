#include <chrono>
#include <iostream>
#include <cstdio>
#include <chrono>
#include <thread>
#include <execinfo.h>
#include <signal.h>
#include <cstdlib>
#include <unistd.h>

#include "httplib.h"
#include "spdlog/spdlog.h"

void handler(int sig) {
    // print out all the frames to stderr
    fprintf(stderr, "Exiting on: signal %d:\n", sig);
    #ifdef NDEBUG
    if(sig==SIGSEGV) {
        // get void*'s for all entries on the stack
        void *array[10];
        size_t size = backtrace(array, 10);

        backtrace_symbols_fd(array, size, STDERR_FILENO);
    }
    #endif
    exit(1);
}

using namespace httplib;

class AccessLogger {
public:
        AccessLogger(bool nolog, const Request& req, const Response& res)
                :nolog_(nolog),
                req_(req),
                res_(res),
                startTime_(std::chrono::steady_clock::now())    {}

        ~AccessLogger() {
                if(!nolog_) {
                    spdlog::info("{} {} {} {} {} {}",
                        req_.method,
                        req_.path,
                        res_.status == -1 ? 200 : res_.status,
                        res_.body.size(),
                        req_.remote_addr,
                        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime_).count());

                }                     
        }

private:
        const bool nolog_;
        const Request& req_;
        const Response& res_;
        std::chrono::steady_clock::time_point startTime_;
};

std::string env(const std::string &name, const std::string &defaultValue) {
    auto val = std::getenv(name.c_str());
    return val == NULL ? defaultValue : std::string(val);
}

std::string env(const std::string &name) {
    return env(name, std::string());
}

struct AtomicCounter {
public:
    AtomicCounter()
        :AtomicCounter(0) {
    }

    AtomicCounter(int startValue)
        :counter_(startValue) {
    }

    AtomicCounter(const AtomicCounter &) = default;
    AtomicCounter & operator =(const AtomicCounter &) = default;

    int getValue() const { return counter_; }

    int increment() {
        return increment(1);
    }

    int increment(int value) {
        return counter_ += value;
    }

    int decrement() {
        return decrement(1);
    }

    int decrement(int value) {
        return counter_ -= value;
    }

private:
    volatile int counter_;
};

int main() {
    auto die = secure_getenv("DIE_FAST");
    if(NULL != die) {
        auto dieAfter = atoi(die);
        std::cout<<"Will sleep for " << dieAfter << " seconds" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(dieAfter));
        std::cout<<"Dying as requested"<<std::endl;
        return -1;
    }

    signal(SIGSEGV, handler);   // install our handler
    signal(SIGKILL, handler);   // install our handler
    signal(SIGINT, handler);   // install our handler

    auto providedCores = env("CORES");
   
    auto cores = std::thread::hardware_concurrency();
    if( !providedCores.empty() ) {
        cores = std::atoi(providedCores.c_str());
    }

    std::cout<<"Cores: "<<cores<<std::endl;

    Server svr;

    svr.new_task_queue = [&] { return new ThreadPool(cores); };

    auto noLogStr = env("NOLOG");
    auto nolog = !noLogStr.empty();
    svr.set_tcp_nodelay(true);
    svr.Get("/", [nolog](const Request& req, Response& res) {
        AccessLogger logger(nolog, req, res);
        res.set_content("Hello World!", "text/plain");
    });

    svr.Get("/nolog", [](const Request& req, Response& res) {
        res.set_content("Hello World!", "text/plain");
    });

    auto maxHealthy = std::atoi(env("HEALTHCOUNT", "3").c_str());

    AtomicCounter healthCtr;

    svr.Get("/unhealthy", [nolog, &healthCtr, &maxHealthy](const Request& req, Response& res) {
        AccessLogger logger(nolog, req, res);
        if ( healthCtr.increment() > maxHealthy ) {
           res.status = 500; 
        }
        else {
            res.set_content("Hello World!", "text/plain");
        }
    });

    svr.set_mount_point("/files", "./testfiles");
    
    svr.listen("0.0.0.0", 8000);
}


