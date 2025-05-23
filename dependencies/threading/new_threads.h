#include "../../supremacy.hpp"

#include <deque>
#include <mutex>
#include <thread>
#include <future>
#include <semaphore>
#include <algorithm>
#include <memory>
#include <functional>
#include <map>
#include <queue>
#include <any>

template <typename T>
class crypt_ptr {
    T* pointer;
public:
    __forceinline crypt_ptr(T* ptr = nullptr) {
        pointer = ptr;
    }

    __forceinline T* get() {
        return pointer;
    }

    __forceinline void set(T* other) {
        pointer = other;
    }

    __forceinline T* operator->() {
        return get();
    }

    __forceinline operator bool() {
        return pointer;
    }
};

enum class task_status_t {
    in_q,
    completed
};

class c_task
{
public:
    template <typename func_ret_type, typename ...fargs, typename ...func_types>
    c_task(func_ret_type(*func)(func_types...), fargs&&... args) :
        is_void{ std::is_void_v<func_ret_type> } {
        if constexpr (std::is_void_v<func_ret_type>) {
            void_func = std::bind(func, args...);
            any_func = []()->int { return 0; };
        }
        else {
            void_func = []()->void {};
            any_func = std::bind(func, args...);
        }
    }

    void operator() () {
        void_func();
        any_func_result = any_func();
    }

    bool has_result() {
        return !is_void;
    }

    std::any get_result() const {
#if 0
        assert(!is_void);
        assert(any_func_result.has_value());
#endif
        return any_func_result;
    }

private:
    std::function<void()> void_func;
    std::function<std::any()> any_func;
    std::any any_func_result;
    bool is_void;
};

struct task_info_t {
    task_status_t status = task_status_t::in_q;
    std::any result;
};

class c_thread_pool {
private:
    std::vector<std::thread> threads;

    std::queue<std::pair<c_task, uint64_t>> q;
    std::mutex q_mtx;
    std::condition_variable q_cv;

    std::unordered_map<uint64_t, task_info_t> tasks_info;
    std::condition_variable tasks_info_cv;
    std::mutex tasks_info_mtx;

    std::condition_variable wait_all_cv;

    std::atomic<bool> quite{ false };
    std::atomic<std::uint64_t> last_idx{ 0 };
    std::atomic<std::uint64_t> cnt_completed_tasks{ 0 };

    using allocate_thread_id_fn = std::int32_t(*)();
    using free_thread_id_fn = void(*)();

    allocate_thread_id_fn allocate_thread_id;
    free_thread_id_fn free_thread_id;

    void run() {
        allocate_thread_id();

        while (!quite) {
            std::unique_lock<std::mutex> lock(q_mtx);
            q_cv.wait(lock, [this]()->bool { return !q.empty() || quite; });

            if (!q.empty() && !quite) {
                std::pair<c_task, uint64_t> task = std::move(q.front());
                q.pop();
                lock.unlock();

                task.first();

                std::lock_guard<std::mutex> lock(tasks_info_mtx);
                if (task.first.has_result())
                    tasks_info[task.second].result = task.first.get_result();
                
                tasks_info[task.second].status = task_status_t::completed;
                ++cnt_completed_tasks;
            }
            wait_all_cv.notify_all();
            tasks_info_cv.notify_all();
        }

        free_thread_id();
    }

public:
    int get_threads_count() {
        auto ptr = *reinterpret_cast<bool**>(supremacy::g_context->addresses().m_thread_id_allocated);

        if (ptr) {
            int threads_allocated = 1;
            for (int i = 1; i < 128; ++i) {
                if (ptr[i])
                    threads_allocated++;
                else
                    break;
            }

            return std::clamp(32 - threads_allocated - 1, 3, 32);
        }

        unsigned int num_threads = std::thread::hardware_concurrency();
        num_threads = std::clamp<unsigned int>(num_threads, 2, 16);

        if (num_threads >= 8)
            num_threads -= 3;
        else
            num_threads -= 1;
                
        return num_threads;
    }

    void init() {
        const auto tier0 = GetModuleHandle(xorstr_("tier0.dll"));

        allocate_thread_id = (allocate_thread_id_fn)GetProcAddress(tier0, xorstr_("AllocateThreadID"));
        free_thread_id = (free_thread_id_fn)GetProcAddress(tier0, xorstr_("FreeThreadID"));

        const auto& num_threads = get_threads_count();

        threads.reserve(num_threads);
        for (int i = 0; i < num_threads; ++i)
            threads.emplace_back(&c_thread_pool::run, this);
    }

    void remove() {
        quite = true;
        q_cv.notify_all();
        for (int i = 0; i < threads.size(); ++i)
            threads[i].join();
    }

    ~c_thread_pool() {
        remove();
    }

    template <typename Func, typename ...fargs, typename ...func_types>
    std::uint64_t add_task(Func(*func)(func_types...), fargs&&... args) {
        const std::uint64_t task_id = last_idx++;

        std::unique_lock<std::mutex> lock(tasks_info_mtx);
        tasks_info[task_id] = task_info_t();
        lock.unlock();

        std::lock_guard<std::mutex> q_lock(q_mtx);
        q.emplace(c_task(func, std::forward<fargs>(args)...), task_id);
        q_cv.notify_one();
        return task_id;
    }

    void wait(const std::uint64_t task_id) {
        std::unique_lock<std::mutex> lock(tasks_info_mtx);
        tasks_info_cv.wait(lock, [this, task_id]()->bool {
                return task_id < last_idx&& tasks_info[task_id].status == task_status_t::completed;
            });
    }

    std::any wait_result(const std::uint64_t task_id) {
        std::unique_lock<std::mutex> lock(tasks_info_mtx);
        tasks_info_cv.wait(lock, [this, task_id]()->bool {
                return task_id < last_idx&& tasks_info[task_id].status == task_status_t::completed;
            });
        return tasks_info[task_id].result;
    }

    template<class T>
    void wait_result(const std::uint64_t task_id, T& value) {
        std::unique_lock<std::mutex> lock(tasks_info_mtx);
        tasks_info_cv.wait(lock, [this, task_id]()->bool {
                return task_id < last_idx&& tasks_info[task_id].status == task_status_t::completed;
            });
        value = std::any_cast<T>(tasks_info[task_id].result);
    }

    void wait_all() {
        std::unique_lock<std::mutex> lock(tasks_info_mtx);
        wait_all_cv.wait(lock, [this]()->bool { return cnt_completed_tasks == last_idx; });
    }

    bool calculated(const std::uint64_t task_id) {
        std::lock_guard<std::mutex> lock(tasks_info_mtx);
        return task_id < last_idx&& tasks_info[task_id].status == task_status_t::completed;
    }
};

extern crypt_ptr<c_thread_pool> thread_pool;