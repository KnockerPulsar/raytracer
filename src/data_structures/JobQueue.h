#pragma once

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <iterator>
#include <mutex>
#include <ostream>
#include <tuple>
#include <utility>
#include <vector>

namespace rt {
  template <typename JobData> class JobQueue {
  private:
    std::vector<JobData> jobs;
    int                  currentChunkStart;
    std::mutex           queueMutex;

    std::condition_variable jobsRefreshed;

    const int chunkSize;

  public:
    int numberOfJobs;

    JobQueue(int numberOfJobs, int chunkSize) : chunkSize(chunkSize), numberOfJobs(numberOfJobs) {
      jobs.reserve(numberOfJobs);
    }

    // Used to add jobs by the main thread.
    void addJobNoLock(JobData newJobData) { jobs.push_back(newJobData); }

    std::pair<typename std::vector<JobData>::iterator, typename std::vector<JobData>::iterator> getChunk() {

      // Unlocks automatically on scope end
      std::unique_lock<std::mutex> lk{queueMutex};

      // All jobs consumed, wait untill the main thread refreshes jobs and notifies threads.
      if (currentChunkStart > jobs.size() - 1)
        jobsRefreshed.wait(lk);

      auto start  = jobs.begin() + currentChunkStart;
      auto offset = std::min((currentChunkStart + chunkSize), (int)jobs.size() - 1);
      auto end    = jobs.begin() + offset;


      // TODO: Updates to currentChunkStart don't seem to be sensed in other threads at the start
      currentChunkStart = getCurrentChunkStart() + chunkSize;

      return std::make_pair(start, end);
    }

    std::vector<JobData> &getJobsVector() { return jobs; }

    bool jobsDone() const { return currentChunkStart >= jobs.size(); }
    int  getCurrentChunkStart() { return currentChunkStart; }
    int  getChunkSize() const { return chunkSize; }

    void awakeAllWorkers() {
      currentChunkStart = 0;
      jobsRefreshed.notify_all();
    }
  };
} // namespace rt