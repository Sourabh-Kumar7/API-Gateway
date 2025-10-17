#pragma once

#include <string>
#include <cstdint>

namespace ratelimiter {

// Abstract interface for a distributed sync backend for token buckets.
// Implementations can be local no-op (in-memory) or Redis-backed.
class RedisBucketSync {
public:
    virtual ~RedisBucketSync() = default;

    // Try to atomically consume `tokens` from the bucket identified by `key`.
    // Returns true if tokens were granted, false otherwise.
    virtual bool tryConsume(const std::string &key, uint64_t tokens) = 0;

    // Ensure the bucket exists with given capacity and refill rate. Optional for backends.
    virtual void ensureBucket(const std::string &key, uint64_t capacity, double refill_rate) = 0;
};

// A trivial no-op/local implementation used for unit tests and single-process usage.
class LocalBucketSync : public RedisBucketSync {
public:
    bool tryConsume(const std::string &key, uint64_t tokens) override;
    void ensureBucket(const std::string &key, uint64_t capacity, double refill_rate) override;
};

} // namespace ratelimiter
