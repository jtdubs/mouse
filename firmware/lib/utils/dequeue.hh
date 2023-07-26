#pragma once

#include <stddef.h>
#include <util/atomic.h>

#include "assert.hh"

#if not defined(__AVR__)
#include <ostream>
#endif

namespace dequeue {

enum class Event : uint8_t {
  PushFront,
  PopFront,
  PushBack,
  PopBack,
};

template <typename T, size_t CAPACITY>
class Dequeue {
  typedef void (*callback_t)(Event, T);

 private:
  T          buffer[CAPACITY];
  uint8_t    front;
  uint8_t    back;
  callback_t callback;

 public:
  Dequeue();

  void RegisterCallback(callback_t c);
  void Clear();
  bool Empty() const;
  bool Full() const;

  T    PeekFront() const;
  T    PeekBack() const;
  T    PopFront();
  T    PopBack();
  void PushFront(T val);
  void PushBack(T val);
};

template <typename T, size_t CAPACITY>
Dequeue<T, CAPACITY>::Dequeue() : front(CAPACITY - 1), back(0) {}

template <typename T, size_t CAPACITY>
void Dequeue<T, CAPACITY>::RegisterCallback(callback_t c) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    callback = c;
  }
}

template <typename T, size_t CAPACITY>
void Dequeue<T, CAPACITY>::Clear() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    front = CAPACITY - 1;
    back  = 0;
  }
}

template <typename T, size_t CAPACITY>
bool Dequeue<T, CAPACITY>::Empty() const {
  bool result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    result = ((front + 1) == back) || (front == (CAPACITY - 1) && back == 0);
  }
  return result;
}

template <typename T, size_t CAPACITY>
bool Dequeue<T, CAPACITY>::Full() const {
  bool result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    result = back == front;
  }
  return result;
}

template <typename T, size_t CAPACITY>
T Dequeue<T, CAPACITY>::PeekFront() const {
  T result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    assert(assert::DEQUEUE + 0, !Empty());
    if (front == (CAPACITY - 1)) {
      result = buffer[0];
    } else {
      result = buffer[front + 1];
    }
  }
  return result;
}

template <typename T, size_t CAPACITY>
T Dequeue<T, CAPACITY>::PeekBack() const {
  T result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    assert(assert::DEQUEUE + 1, !Empty());
    if (back == 0) {
      result = buffer[CAPACITY - 1];
    } else {
      result = buffer[back - 1];
    }
  }
  return result;
}

template <typename T, size_t CAPACITY>
T Dequeue<T, CAPACITY>::PopFront() {
  T result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    assert(assert::DEQUEUE + 2, !Empty());
    if (front == (CAPACITY - 1)) {
      front = 0;
    } else {
      front++;
    }
    result = buffer[front];
  }
  if (callback) {
    callback(Event::PopFront, result);
  }
  return result;
}

template <typename T, size_t CAPACITY>
T Dequeue<T, CAPACITY>::PopBack() {
  T result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    assert(assert::DEQUEUE + 3, !Empty());
    if (back == 0) {
      back = CAPACITY - 1;
    } else {
      back--;
    }
    result = buffer[back];
  }
  if (callback) {
    callback(Event::PopBack, result);
  }
  return result;
}

template <typename T, size_t CAPACITY>
void Dequeue<T, CAPACITY>::PushFront(T val) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    assert(assert::DEQUEUE + 4, !Full());
    buffer[front] = val;
    if (front == 0) {
      front = CAPACITY - 1;
    } else {
      front--;
    }
  }
  if (callback) {
    callback(Event::PushFront, val);
  }
}

template <typename T, size_t CAPACITY>
void Dequeue<T, CAPACITY>::PushBack(T val) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    assert(assert::DEQUEUE + 5, !Full());
    buffer[back] = val;
    if (back == (CAPACITY - 1)) {
      back = 0;
    } else {
      back++;
    }
  }
  if (callback) {
    callback(Event::PushBack, val);
  }
}

#if not defined(__AVR__)
std::ostream &operator<<(std::ostream &o, const Event event) {
  switch (event) {
    case Event::PushFront:
      o << "Event::PushFront";
      break;
    case Event::PopFront:
      o << "Event::PopFront";
      break;
    case Event::PushBack:
      o << "Event::PushBack";
      break;
    case Event::PopBack:
      o << "Event::PopBack";
      break;
  }
}
#endif

}  // namespace dequeue
