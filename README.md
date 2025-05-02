## pyatomix provides std::atomic_flag and std::atomic<int64_t> for Python

Compatible with Python 3.13 free-threaded.

There are Windows wheels available, but it can build from source automatically on Windows. Windows users will need to have Visual Studio (or VS Build Tools) installed to build this from source.

Linux users will need the Python dev package installed for their Python version to install atomix from source, for instance: python3.13-dev

## Installation

```
pip install -U pyatomix
-or-
git clone https://github.com/electroglyph/pyatomix.git
pip install -U ./pyatomix
```

## Usage

```python
from pyatomix import AtomicInt, AtomicFlag
x = AtomicInt(7)     # initial value of 7
x += 1               # now it's 8, this is atomic
y = AtomicFlag(True) # y == True
y = AtomicFlag()     # y == False
z = y.test_and_set() # z == False, y == True
y.clear()            # y == False
```
the API is exactly the same as std::atomic and std::atomic_flag.
all the math operators except **, >>, <<=, >>, >>= are overridden for AtomicInt.

atomic assignment operators: +=, -=, &=, |=, ^= (using these are an atomic operation)

other assignment operators like %=,/=,//=,*= first load the value, then store the modified value. those operators are *not* atomic.

## Performance

Depending on compiler and OS, AtomicInt increment is 4-6x slower than a standard increment, which is still pretty fast.
1 million atomic increments in a for loop takes me 160ms in Linux, while incrementing a regular int 1 million times takes 40ms.

## To run the tests

```python
from pyatomix import AtomicTest
x = AtomicTest()
x.run()
```

NOTE: the AtomicFlag test will hang unless you're running a free-threaded build of Python

## API list, all these are atomic

```python
AtomicInt.is_lock_free() -> bool   : returns True if it's lock free, should be True for most
AtomicInt.store(value) -> None     : assign value, doesn't return anything
AtomicInt.load() -> int            : read value
AtomicInt.fetch_add(value) -> int  : add to current value, return previous value
AtomicInt.fetch_sub(value) -> int  : subtract from current value, return previous value
AtomicInt.exchange(value) -> int   : assign value, return previous value
AtomicInt.compare_exchange(expected, value) -> bool : 
    if current value == expected, replace it with value. returns True on success.
AtomicInt.compare_exchange_weak(expected, value) -> bool : 
    same as compare_exchange, but may fail spuriously. faster on some platforms, use in a loop.

AtomicFlag.clear() -> None        : set value to False
AtomicFlag.test_and_set() -> bool : set value to True and return previous value
AtomicFlag.test() -> bool         : read value
AtomicFlag.wait(bool cmp) -> bool : if current value == cmp, wait until signaled
AtomicFlag.notify_one() -> None   : notify one or more threads that the value has changed
AtomicFlag.notify_all() -> None   : notify all threads that the value has changed
```

## AtomicFlag overloaded operators: 

`==,!=` (will work with "falsy" types too)

## AtomicInt overloaded operators:

`==,!=,-,~,+,+=,-=,*,*=,/,/=,//,//=,|,|=,%,%=,&,&=,^,^=,>,>=,<,<=`

(trying to use anything other than an int with these will fail)

