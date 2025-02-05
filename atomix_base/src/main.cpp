#include <pybind11/pybind11.h>
#include <atomic>
#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
namespace py = pybind11;
using namespace pybind11::literals;

class AtomicFlag {
public:
    AtomicFlag() {}
    void clear() { value.clear(std::memory_order_release); }
    bool test_and_set() { return value.test_and_set(std::memory_order_acquire); }
    bool test() { return value.test(std::memory_order_acquire); }
    void wait(bool old) { value.wait(old, std::memory_order_acquire); }
    void notify_one() { value.notify_one(); }
    void notify_all() { value.notify_all(); }
    bool operator==(py::object other) const {
        return this->value.test(std::memory_order_acquire) == py::bool_(other);
    }
    bool operator!=(py::object other) const {
        return this->value.test(std::memory_order_acquire) != py::bool_(other);
    }
private:
    std::atomic_flag value = ATOMIC_FLAG_INIT;
};

class AtomicInt {
public:
    AtomicInt() : value(0) {}
    AtomicInt(int64_t value) : value(value) {}
    bool is_lock_free() { return value.is_lock_free(); }
    int64_t load() { return value.load(std::memory_order_acquire); }
    void store(int64_t new_val) { value.store(new_val, std::memory_order_release); }
    int64_t fetch_add(int64_t val) { return value.fetch_add(val, std::memory_order_acq_rel); }
    int64_t fetch_sub(int64_t val) { return value.fetch_sub(val, std::memory_order_acq_rel); }
    int64_t exchange(int64_t new_val) { return value.exchange(new_val, std::memory_order_acq_rel); }
    bool compare_exchange(int64_t expected_val, int64_t new_val) { 
        return value.compare_exchange_strong(expected_val, new_val);
        }
    bool compare_exchange_weak(int64_t expected_val, int64_t new_val) { 
        return value.compare_exchange_weak(expected_val, new_val);
        }
    bool operator==(int64_t other) const {
        return this->value == other;
    }
    bool operator!=(int64_t other) const {
        return this->value != other;
    }
    int64_t operator+(int64_t other) {
        return this->value.load(std::memory_order_acquire) + other;
    }
    AtomicInt* operator+=(int64_t other) {
        this->value += other;
        return this;
    }
    int64_t operator-(int64_t other) {
        return this->value.load(std::memory_order_acquire) + other;
    }
    AtomicInt* operator-=(int64_t other) {
        this->value -= other;
        return this;
    }
    int64_t rsub(int64_t other) {
        return other - this->value.load(std::memory_order_acquire);
    }
    int64_t operator*(int64_t other) {
        return this->value.load(std::memory_order_acquire) * other;
    }
    AtomicInt* operator*=(int64_t other) {
        this->value.exchange(this->value.load(std::memory_order_acquire) * other, std::memory_order_acq_rel);
        return this;
    }
    int64_t operator/(int64_t other) {
        return this->value.load(std::memory_order_acquire) / other;
    }
    AtomicInt* operator/=(int64_t other) {
        this->value.exchange(this->value.load(std::memory_order_acquire) / other, std::memory_order_acq_rel);
        return this;
    }
    int64_t rdiv(int64_t other) {
        return other / this->value.load(std::memory_order_acquire);
    }
    int64_t operator%(int64_t other) {
        return this->value.load(std::memory_order_acquire) % other;
    }
    AtomicInt* operator%=(int64_t other) {
        this->value.exchange(this->value.load(std::memory_order_acquire) % other, std::memory_order_acq_rel);
        return this;
    }
    int64_t rmod(int64_t other) {
        return other % this->value.load(std::memory_order_acquire);
    }
    int64_t operator&(int64_t other) {
        return this->value.load(std::memory_order_acquire) & other;
    }
    AtomicInt* operator&=(int64_t other) {
        this->value &= other;
        return this;
    }
    int64_t operator|(int64_t other) {
        return this->value.load(std::memory_order_acquire) | other;
    }
    AtomicInt* operator|=(int64_t other) {
        this->value |= other;
        return this;
    }
    int64_t operator^(int64_t other) {
        return this->value.load(std::memory_order_acquire) ^ other;
    }
    AtomicInt* operator^=(int64_t other) {
        this->value ^= other;
        return this;
    }
    bool lt(int64_t other) {
        return this->value.load(std::memory_order_acquire) < other;
    }
    bool le(int64_t other) {
        return this->value.load(std::memory_order_acquire) <= other;
    }
    bool gt(int64_t other) {
        return this->value.load(std::memory_order_acquire) > other;
    }
    bool ge(int64_t other) {
        return this->value.load(std::memory_order_acquire) >= other;
    }
    std::string str() {
        return std::to_string(this->value.load(std::memory_order_acquire));
    }

private:
    std::atomic<int64_t> value;
};

PYBIND11_MODULE(atomix_base, m, py::mod_gil_not_used()) {
    py::class_<AtomicFlag>(m, "AtomicFlag")
        .def(py::init<>())
        .def("clear", &AtomicFlag::clear)
        .def("test_and_set", &AtomicFlag::test_and_set)
        .def("test", &AtomicFlag::test)
        .def("wait", &AtomicFlag::wait)
        .def("notify_one", &AtomicFlag::notify_one)
        .def("notify_all", &AtomicFlag::notify_all)
        .def("__eq__", &AtomicFlag::operator==)
        .def("__neq__", &AtomicFlag::operator!=);

    py::class_<AtomicInt>(m, "AtomicInt")
        .def(py::init<int64_t>())
        .def(py::init<>())
        .def("is_lock_free", &AtomicInt::is_lock_free)
        .def("load", &AtomicInt::load)
        .def("store", &AtomicInt::store)
        .def("fetch_add", &AtomicInt::fetch_add)
        .def("fetch_sub", &AtomicInt::fetch_sub)
        .def("exchange", &AtomicInt::exchange)
        .def("compare_exchange", &AtomicInt::compare_exchange)
        .def("compare_exchange_weak", &AtomicInt::compare_exchange_weak)
        .def("__eq__", &AtomicInt::operator==)
        .def("__neq__", &AtomicInt::operator!=)
        .def("__add__", &AtomicInt::operator+)
        .def("__iadd__", &AtomicInt::operator+=)
        .def("__sub__", &AtomicInt::operator-)
        .def("__isub__", &AtomicInt::operator-=)
        .def("__rsub__", &AtomicInt::rsub)
        .def("__mul__", &AtomicInt::operator*)
        .def("__imul__", &AtomicInt::operator*=)
        .def("__rmul__", &AtomicInt::operator*)
        .def("__truediv__", &AtomicInt::operator/)
        .def("__floordiv__", &AtomicInt::operator/)
        .def("__ifloordiv__", &AtomicInt::operator/=)
        .def("__itruediv__", &AtomicInt::operator/=)
        .def("__rfloordiv__", &AtomicInt::rdiv)
        .def("__rtruediv__", &AtomicInt::rdiv)
        .def("__mod__", &AtomicInt::operator%)
        .def("__imod__", &AtomicInt::operator%=)
        .def("__rmod__", &AtomicInt::rmod)
        .def("__and__", &AtomicInt::operator&)
        .def("__iand__", &AtomicInt::operator&=)
        .def("__rand__", &AtomicInt::operator&)
        .def("__or__", &AtomicInt::operator|)
        .def("__ior__", &AtomicInt::operator|=)
        .def("__ror__", &AtomicInt::operator|)
        .def("__xor__", &AtomicInt::operator^)
        .def("__ixor__", &AtomicInt::operator^=)
        .def("__rxor__", &AtomicInt::operator^)
        .def("__lt__", &AtomicInt::lt)
        .def("__le__", &AtomicInt::le)
        .def("__gt__", &AtomicInt::gt)
        .def("__ge__", &AtomicInt::ge)
        .def("__str__", &AtomicInt::str);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
