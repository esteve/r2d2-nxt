#include <mutex>

int main(void) {
    std::mutex m;
    m.lock();
    m.unlock();
}
