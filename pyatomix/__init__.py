from atomix_base import AtomicFlag, AtomicInt
import time
import threading
import pickle
__all__ = ["AtomicInt", "AtomicTest", "AtomicFlag"]

class AtomicTest():
    def __init__(self) -> None:
        self.base = AtomicInt(0)
        self.flag = AtomicFlag()
    def store_test(self) -> bool:
        self.base.store(1)
        return self.base.load() == 1
    def add_test(self) -> bool:
        self.base.store(7)
        if self.base.fetch_add(1) != 7:
            return False
        return self.base.load() == 8
    def sub_test(self) -> bool:
        self.base.store(7)
        if self.base.fetch_sub(1) != 7:
            return False
        return self.base.load() == 6
    def xchg_test(self) -> bool:
        self.base.store(1)
        if self.base.exchange(2) != 1:
            return False
        return self.base.load() == 2
    def cmp_xchg_test(self) -> bool:
        self.base.store(1)
        if self.base.compare_exchange(1, 2) != True:
            return False
        return self.base.load() == 2
    def cmp_xchg_weak_test(self) -> bool:
        self.base.store(1)
        if self.base.compare_exchange_weak(1, 2) != True:
            return False
        return self.base.load() == 2
    def operators(self) -> str:
        self.base.store(1)
        if self.base + 1 != 2:
            return "add fail"
        self.base += 1
        if self.base.load() != 2 or self.base != 2:
            return "iadd fail"
        if self.base - 1 != 1:
            return "sub fail"
        self.base -= 1
        if self.base.load() != 1 or self.base != 1:
            return "isub fail"
        self.base.store(2)
        if self.base * 2 != 4:
            return "mul fail"
        self.base *= 2
        if self.base.load() != 4 or self.base != 4:
            return "imul fail"
        if self.base // 2 != 2:
            return "div fail"
        self.base //= 2
        if self.base.load() != 2 or self.base != 2:
            return "idiv fail"
        if 2 // self.base != 1:
            return "rdiv fail"
        if 2 / self.base != 1:
            return "rdiv fail"
        if 2 * self.base != 4:
            return "rmul fail"
        if self.base % 2 != 0:
            return "mod fail"
        self.base %= 2
        if self.base.load() != 0 or self.base != 0:
            return "imod fail"
        self.base.store(2)
        if 2 % self.base != 0:
            return "rmod fail"
        if self.base & 2 != 2:
            return "and fail"
        if 2 & self.base != 2:
            return "rand fail"
        self.base &= 2
        if self.base.load() != 2 or self.base != 2:
            return "iand fail"
        if self.base | 2 != 2:
            return "or fail"
        if 2 | self.base != 2:
            return "ror fail"
        self.base |= 2
        if self.base.load() != 2 or self.base != 2:
            return "ior fail"
        if self.base ^ 2 != 0:
            return "xor fail"
        if 2 ^ self.base != 0:
            return "rxor fail"
        self.base ^= 2
        if self.base.load() != 0 or self.base != 0:
            return "ixor fail"
        self.base.store(2)
        if not self.base < 3:
            return "< fail"
        if not self.base <= 2:
            return "<= fail"
        if not self.base > 0:
            return "> fail"
        if not self.base >= 1:
            return ">= fail"
        return ""
    def pickle_test(self) -> bool:
        self.base = AtomicInt(888)
        x = pickle.dumps(self.base)
        y = pickle.loads(x)
        return y == 888
    def index_test(self) -> bool:
        self.base = AtomicInt(8)
        test_list = [0] * 9
        test_list[self.base] = 777
        return test_list[self.base] == test_list[8]
        
    def int_test(self):
        fail = 0
        result = "AtomicInt: "
        if not self.store_test():
            result += "Store test failed\n"
            fail += 1
        if not self.add_test():
            result += "Add test failed\n"
            fail += 1
        if not self.sub_test():
            result += "Subtract test failed\n"
            fail += 1
        if not self.xchg_test():
            result += "Exchange test failed\n"
            fail += 1
        if not self.cmp_xchg_test():
            result += "Compare exchange strong test failed\n"
            fail += 1
        if not self.cmp_xchg_weak_test():
            result += "Compare exchange weak test failed\n"
            fail += 1
        o = self.operators()
        if not o:
            result += "Operators test failed: {o}\n"
            fail += 1
        if not self.pickle_test():
            result += "Pickle test failed\n"
            fail += 1
        if not self.index_test():
            result += "Index test failed\n"
            fail += 1
        if fail == 0:
            result += "All tests passed"
        else:
            result += f"{fail} tests failed"
        return result
    
    def wait_test(self):
        start = time.time()
        self.flag.wait(True)
        print(f"waited for {time.time() - start:.2f} seconds.")
            
    def flag_test(self):
        if self.flag != False:
            return False
        if self.flag.test() != False:
            return False
        if self.flag.test_and_set() != False:
            return False
        if self.flag != True:
            return False
        self.flag = AtomicFlag(True)
        if self.flag != True:
            return False
        x = pickle.dumps(self.flag)
        y = pickle.loads(x)
        if y != True:
            return False
        self.flag.test_and_set()
        t = threading.Thread(target=self.wait_test)
        t.start()
        time.sleep(1)
        self.flag.clear()
        self.flag.notify_all()
        t.join()
        return True
            
    def run(self):
        print(f"{self.int_test()}")
        f = self.flag_test()
        print("AtomicFlag: All tests passed") if f else print("AtomicFlag: Tests failed")