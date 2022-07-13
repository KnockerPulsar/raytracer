#pragma once

#include "AsyncRenderData.h"
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

using std::vector, std::pair;

namespace rt {

  class AsyncRenderData;

  template <typename JobData> class JobQueue {
  private:
    vector<JobData> jobs;
    int             currentChunkStart;
    std::mutex      queueMutex;

    std::condition_variable threadBarrier;

    const int chunkSize;

  public:
    int numberOfJobs;

    JobQueue(int numberOfJobs, int chunkSize) : chunkSize(chunkSize), numberOfJobs(numberOfJobs) {
      jobs.reserve(numberOfJobs);
    }

    // Used to add jobs by the main thread.
    void addJobNoLock(JobData newJobData) { jobs.push_back(newJobData); }

    pair<typename vector<JobData>::iterator, typename vector<JobData>::iterator>
    getChunk(AsyncRenderData &ard, int threadIndex) {

      // Unlocks automatically on scope end
      std::unique_lock<std::mutex> lk{queueMutex};

      // All jobs consumed, wait untill the main thread refreshes jobs and notifies threads.
      // Or main thread wants to stop threads
      if (currentChunkStart > jobs.size() - 1) {
        ard.finishedThreads[threadIndex] = true;
        threadBarrier.wait(lk);
      }

      auto start  = jobs.begin() + currentChunkStart;
      auto offset = std::min((currentChunkStart + chunkSize), (int)jobs.size() - 1);
      auto end    = jobs.begin() + offset;

      currentChunkStart = getCurrentChunkStart() + chunkSize;

      return std::make_pair(start, end);
    }

    vector<JobData> &getJobsVector() { return jobs; }

    int getCurrentChunkStart() { return currentChunkStart; }

    int getChunkSize() const { return chunkSize; }

    void awakeAllWorkers() {
      currentChunkStart = 0;
      threadBarrier.notify_all();
    }

    void setCurrentChunkStart(int ccs) { currentChunkStart = ccs; }
  };
} // namespace rt