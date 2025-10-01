# API-Gateway

A high-performance **API Gateway** built in **C++20**, implementing a **token-bucket rate limiting algorithm** with:
- Drogon (C++ HTTP Framework)
- Redis (distributed sync)
- JWT (role-based authentication)
- JSON (hot-reloadable configs)
- Multithreading for concurrency

## 🚀 Features
- Per-user and per-API rate limiting
- Redis for cross-instance state synchronization
- JWT-based role authentication
- Hot-reloadable JSON configuration (no restarts needed)
- Handles **1M+ requests/day**

---