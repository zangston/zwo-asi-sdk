#include <cstdio>
#include <cstdint>
#include <csignal>
#include <deque>
#include <thread>
#include <pthread.h>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <unistd.h>
#include <sys/syscall.h>
#include <err.h>
#include "Frame.h"
#include "agc.h"
#include "disk.h"
#include "preview.h"
#include "camera.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
// Globals accessed by all threads
///////////////////////////////////////////////////////////////////////////////////////////////////

// All threads should end gracefully when this is true
std::atomic_bool end_program = false;

// AGC enable state
std::atomic_bool agc_enabled = false;

// AGC outputs
std::atomic_int camera_gain = GAIN_MIN;
std::atomic_int camera_exposure_us = EXPOSURE_MIN_US;

// std::deque is not thread safe
std::mutex to_disk_deque_mutex;
std::mutex to_preview_deque_mutex;
std::mutex to_agc_deque_mutex;
std::mutex unused_deque_mutex;

std::condition_variable to_disk_deque_cv;
std::condition_variable to_preview_deque_cv;
std::condition_variable to_agc_deque_cv;
std::condition_variable unused_deque_cv;

// FIFOs holding pointers to frame objects
std::deque<Frame *> to_disk_deque;
std::deque<Frame *> to_preview_deque;
std::deque<Frame *> to_agc_deque;
std::deque<Frame *> unused_deque;

///////////////////////////////////////////////////////////////////////////////////////////////////
// End globals declaration section
///////////////////////////////////////////////////////////////////////////////////////////////////


static void set_thread_priority(pthread_t thread, int policy, int priority)
{
    sched_param sch_params;
    sch_params.sched_priority = priority;
    if (pthread_setschedparam(thread, policy, &sch_params))
    {
        err(1, "Failed to set thread priority");
    }
}


void sigint_handler(int signal)
{
    end_program = true;
    to_disk_deque_cv.notify_one();
    to_preview_deque_cv.notify_one();
    to_agc_deque_cv.notify_one();
    unused_deque_cv.notify_one();
}


int main(int argc, char *argv[])
{
    signal(SIGINT, sigint_handler);

    printf("main thread id: %ld\n", syscall(SYS_gettid));

    /*
     * Set real-time priority for the main thread. All threads created later, including by the ASI
     * library, will inherit this priority.
     */
    set_thread_priority(pthread_self(), SCHED_RR, 10);

    ASI_CAMERA_INFO CamInfo;
    init_camera(CamInfo);

    // Create pool of frame buffers
    Frame::IMAGE_SIZE_BYTES = CamInfo.MaxWidth * CamInfo.MaxHeight;
    constexpr size_t FRAME_POOL_SIZE = 64;
    static std::deque<Frame> frames;
    for(size_t i = 0; i < FRAME_POOL_SIZE; i++)
    {
        // Frame objects add themselves to unused_deque on construction
        frames.emplace_back();
    }

    // Start threads
    static std::thread write_to_disk_thread(
        write_to_disk,
        (argc > 1) ? (argv[1]) : (nullptr),
        CamInfo.MaxWidth,
        CamInfo.MaxHeight
    );
    static std::thread preview_thread(preview);
    static std::thread agc_thread(agc);

    // These threads do not need real-time priority
    set_thread_priority(preview_thread.native_handle(), SCHED_OTHER, 0);
    set_thread_priority(agc_thread.native_handle(), SCHED_OTHER, 0);

    // Get frames from camera and dispatch them to the other threads
    run_camera(CamInfo);

    printf("Main (camera) thread done, waiting for others to finish.\n");

    write_to_disk_thread.join();
    preview_thread.join();
    agc_thread.join();

    printf("Main thread ending.\n");

    return 0;
}
