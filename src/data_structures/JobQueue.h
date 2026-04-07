#pragma once

#include <algorithm>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <utility>
#include <vector>

// TODO should be moved to `src/raytracer`
namespace rt::raytracer {

  template <typename JobData> class JobQueue {
  private:
    std::vector<JobData> jobs;

    const int numberOfJobs;
    const int chunkSize;
    int       currentChunkStart;

    std::mutex      queueMutex;
    std::condition_variable threadBarrier;

    std::function<void(int)> const onThreadDone;

  public:
    JobQueue(int numberOfJobs, int chunkSize, std::function<void(int)> onThreadDone)
        : chunkSize(chunkSize), numberOfJobs(numberOfJobs), onThreadDone(onThreadDone), currentChunkStart(0) {
      jobs.reserve(numberOfJobs);
    }

    // Used to add jobs by the main thread.
    void addJobNoLock(JobData newJobData) { jobs.push_back(newJobData); }

    std::pair<typename std::vector<JobData>::iterator, typename std::vector<JobData>::iterator>
    getChunk(int threadIndex) {

      // Unlocks automatically on scope end
      std::unique_lock<std::mutex> lk{queueMutex};

      // All jobs consumed, wait untill the main thread refreshes jobs and notifies threads.
      // Or main thread wants to stop threads
      if (currentChunkStart > jobs.size() - 1) {
        onThreadDone(threadIndex);
        threadBarrier.wait(lk);
      }

      auto start  = jobs.begin() + currentChunkStart;
      auto offset = std::min((currentChunkStart + chunkSize), (int)jobs.size() - 1);
      auto end    = jobs.begin() + offset;

      currentChunkStart = getCurrentChunkStart() + chunkSize;

      return std::make_pair(start, end);
    }

    std::vector<JobData> &getJobsVector() { return jobs; }

    int getCurrentChunkStart() { return currentChunkStart; }

    int getChunkSize() const { return chunkSize; }

    void awakeAllWorkers() {
      currentChunkStart = 0;
      threadBarrier.notify_all();
    }

    void setCurrentChunkStart(int ccs) { currentChunkStart = ccs; }
  };
} // namespace rt
