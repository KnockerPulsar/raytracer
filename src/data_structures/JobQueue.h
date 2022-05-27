#include <algorithm>
#include <iterator>
#include <mutex>
#include <tuple>
#include <utility>
#include <vector>

namespace rt {
  template <typename JobData> class JobQueue {
  private:
    std::vector<JobData> jobs;
    int                  currentChunkStart = 0;
    std::mutex           queueMutex;

    const int chunkSize;

  public:
    int numberOfJobs;

    JobQueue(int numberOfJobs, int chunkSize) : chunkSize(chunkSize), numberOfJobs(numberOfJobs) {
      jobs.reserve(numberOfJobs);
    }

    // Used to add jobs by the main thread.
    void addJobNoLock(JobData &newJobData) { jobs.push_back(newJobData); }

    std::tuple<typename std::vector<JobData>::iterator, typename std::vector<JobData>::iterator, bool> getChunk() {

      if (currentChunkStart > jobs.size() - 1)
        return std::make_tuple(jobs.end(), jobs.end(), false);
      

      queueMutex.lock();
      auto start  = jobs.begin() + currentChunkStart;
      auto offset = std::min((currentChunkStart + chunkSize), (int)jobs.size() - 1);
      auto end    = jobs.begin() + offset;
      currentChunkStart += chunkSize;
      queueMutex.unlock();

      return std::make_tuple(start, end, true);
    }

    std::vector<JobData> &getJobsVector() { return jobs; }
  };
} // namespace rt