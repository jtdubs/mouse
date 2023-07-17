#pragma once

#include <stddef.h>
#include <util/atomic.h>

#include "assert.hh"

namespace dequeue {

typedef enum : uint8_t {
  EVENT_PUSH_FRONT,
  EVENT_POP_FRONT,
  EVENT_PUSH_BACK,
  EVENT_POP_BACK,
} event_type_t;

template <typename T, size_t CAPACITY>
class dequeue {
  typedef void (*callback_t)(event_type_t, T);

 private:
  T          buffer[CAPACITY];
  uint8_t    front;
  uint8_t    back;
  callback_t callback;

 public:
  dequeue();

  void register_callback(callback_t c);
  void clear();
  bool empty() const;
  bool full() const;

  T    peek_front() const;
  T    peek_back() const;
  T    pop_front();
  T    pop_back();
  void push_front(T val);
  void push_back(T val);
};

template <typename T, size_t CAPACITY>
dequeue<T, CAPACITY>::dequeue() : front(CAPACITY - 1), back(0) {}

template <typename T, size_t CAPACITY>
void dequeue<T, CAPACITY>::register_callback(callback_t c) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    callback = c;
  }
}

template <typename T, size_t CAPACITY>
void dequeue<T, CAPACITY>::clear() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    front = CAPACITY - 1;
    back  = 0;
  }
}

template <typename T, size_t CAPACITY>
bool dequeue<T, CAPACITY>::empty() const {
  bool result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    result = ((front + 1) == back) || (front == (CAPACITY - 1) && back == 0);
  }
  return result;
}

template <typename T, size_t CAPACITY>
bool dequeue<T, CAPACITY>::full() const {
  bool result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    result = back == front;
  }
  return result;
}

template <typename T, size_t CAPACITY>
T dequeue<T, CAPACITY>::peek_front() const {
  T result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    assert(assert::DEQUEUE + 0, !empty());
    if (front == (CAPACITY - 1)) {
      result = buffer[0];
    } else {
      result = buffer[front + 1];
    }
  }
  return result;
}

template <typename T, size_t CAPACITY>
T dequeue<T, CAPACITY>::peek_back() const {
  T result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    assert(assert::DEQUEUE + 1, !empty());
    if (back == 0) {
      result = buffer[CAPACITY - 1];
    } else {
      result = buffer[back - 1];
    }
  }
  return result;
}

template <typename T, size_t CAPACITY>
T dequeue<T, CAPACITY>::pop_front() {
  T result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    assert(assert::DEQUEUE + 2, !empty());
    if (front == (CAPACITY - 1)) {
      front = 0;
    } else {
      front++;
    }
    result = buffer[front];
  }
  if (callback) {
    callback(EVENT_POP_FRONT, result);
  }
  return result;
}

template <typename T, size_t CAPACITY>
T dequeue<T, CAPACITY>::pop_back() {
  T result;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    assert(assert::DEQUEUE + 3, !empty());
    if (back == 0) {
      back = CAPACITY - 1;
    } else {
      back--;
    }
    result = buffer[back];
  }
  if (callback) {
    callback(EVENT_POP_BACK, result);
  }
  return result;
}

template <typename T, size_t CAPACITY>
void dequeue<T, CAPACITY>::push_front(T val) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    assert(assert::DEQUEUE + 4, !full());
    buffer[front] = val;
    if (front == 0) {
      front = CAPACITY - 1;
    } else {
      front--;
    }
  }
  if (callback) {
    callback(EVENT_PUSH_FRONT, val);
  }
}

template <typename T, size_t CAPACITY>
void dequeue<T, CAPACITY>::push_back(T val) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    assert(assert::DEQUEUE + 5, !full());
    buffer[back] = val;
    if (back == (CAPACITY - 1)) {
      back = 0;
    } else {
      back++;
    }
  }
  if (callback) {
    callback(EVENT_PUSH_BACK, val);
  }
}

}  // namespace dequeue
