#pragma once

#include "assert.h"

typedef enum : uint8_t {
  DEQUEUE_EVENT_TYPE_PUSH_FRONT,
  DEQUEUE_EVENT_TYPE_POP_FRONT,
  DEQUEUE_EVENT_TYPE_PUSH_BACK,
  DEQUEUE_EVENT_TYPE_POP_BACK,
} dequeue_event_type_t;

// A simple dequeue implementation.
#define DEFINE_DEQUEUE(type, name, len)                                                \
  typedef void (*name##_dequeue_callback_t)(dequeue_event_type_t, type);               \
                                                                                       \
  typedef struct {                                                                     \
    type                      buffer[len];                                             \
    uint8_t                   front;                                                   \
    uint8_t                   back;                                                    \
    name##_dequeue_callback_t callback;                                                \
  } name##_dequeue_t;                                                                  \
                                                                                       \
  static name##_dequeue_t name##_dequeue;                                              \
                                                                                       \
  [[maybe_unused]] static void name##_init() {                                         \
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                                                \
      name##_dequeue.front = len - 1;                                                  \
      name##_dequeue.back  = 0;                                                        \
    }                                                                                  \
  }                                                                                    \
                                                                                       \
  [[maybe_unused]] static void name##_register_callback(name##_dequeue_callback_t c) { \
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                                                \
      name##_dequeue.callback = c;                                                     \
    }                                                                                  \
  }                                                                                    \
                                                                                       \
  [[maybe_unused]] static void name##_clear() {                                        \
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                                                \
      name##_dequeue.front = len - 1;                                                  \
      name##_dequeue.back  = 0;                                                        \
    }                                                                                  \
  }                                                                                    \
                                                                                       \
  [[maybe_unused]] static bool name##_empty() {                                        \
    bool result;                                                                       \
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                                                \
      result = ((name##_dequeue.front + 1) == name##_dequeue.back)                     \
            || (name##_dequeue.front == (len - 1) && name##_dequeue.back == 0);        \
    }                                                                                  \
    return result;                                                                     \
  }                                                                                    \
                                                                                       \
  [[maybe_unused]] static bool name##_full() {                                         \
    bool result;                                                                       \
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                                                \
      result = name##_dequeue.back == name##_dequeue.front;                            \
    }                                                                                  \
    return result;                                                                     \
  }                                                                                    \
                                                                                       \
  [[maybe_unused]] static type name##_peek_front() {                                   \
    type result;                                                                       \
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                                                \
      assert(ASSERT_DEQUEUE + 0, !name##_empty());                                     \
      if (name##_dequeue.front == (len - 1)) {                                         \
        result = name##_dequeue.buffer[0];                                             \
      } else {                                                                         \
        result = name##_dequeue.buffer[name##_dequeue.front + 1];                      \
      }                                                                                \
    }                                                                                  \
    return result;                                                                     \
  }                                                                                    \
                                                                                       \
  [[maybe_unused]] static type name##_peek_back() {                                    \
    type result;                                                                       \
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                                                \
      assert(ASSERT_DEQUEUE + 1, !name##_empty());                                     \
      if (name##_dequeue.back == 0) {                                                  \
        result = name##_dequeue.buffer[len - 1];                                       \
      } else {                                                                         \
        result = name##_dequeue.buffer[name##_dequeue.back - 1];                       \
      }                                                                                \
    }                                                                                  \
    return result;                                                                     \
  }                                                                                    \
                                                                                       \
  [[maybe_unused]] static type name##_pop_front() {                                    \
    type result;                                                                       \
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                                                \
      assert(ASSERT_DEQUEUE + 2, !name##_empty());                                     \
      if (name##_dequeue.front == (len - 1)) {                                         \
        name##_dequeue.front = 0;                                                      \
      } else {                                                                         \
        name##_dequeue.front++;                                                        \
      }                                                                                \
      result = name##_dequeue.buffer[name##_dequeue.front];                            \
    }                                                                                  \
    if (name##_dequeue.callback) {                                                     \
      name##_dequeue.callback(DEQUEUE_EVENT_TYPE_POP_FRONT, result);                   \
    }                                                                                  \
    return result;                                                                     \
  }                                                                                    \
                                                                                       \
  [[maybe_unused]] static type name##_pop_back() {                                     \
    type result;                                                                       \
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                                                \
      assert(ASSERT_DEQUEUE + 3, !name##_empty());                                     \
      if (name##_dequeue.back == 0) {                                                  \
        name##_dequeue.back = len - 1;                                                 \
      } else {                                                                         \
        name##_dequeue.back--;                                                         \
      }                                                                                \
      result = name##_dequeue.buffer[name##_dequeue.back];                             \
    }                                                                                  \
    if (name##_dequeue.callback) {                                                     \
      name##_dequeue.callback(DEQUEUE_EVENT_TYPE_POP_BACK, result);                    \
    }                                                                                  \
    return result;                                                                     \
  }                                                                                    \
                                                                                       \
  [[maybe_unused]] static void name##_push_front(type val) {                           \
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                                                \
      assert(ASSERT_DEQUEUE + 4, !name##_full());                                      \
      name##_dequeue.buffer[name##_dequeue.front] = val;                               \
      if (name##_dequeue.front == 0) {                                                 \
        name##_dequeue.front = len - 1;                                                \
      } else {                                                                         \
        name##_dequeue.front--;                                                        \
      }                                                                                \
    }                                                                                  \
    if (name##_dequeue.callback) {                                                     \
      name##_dequeue.callback(DEQUEUE_EVENT_TYPE_PUSH_FRONT, val);                     \
    }                                                                                  \
  }                                                                                    \
                                                                                       \
  [[maybe_unused]] static void name##_push_back(type val) {                            \
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {                                                \
      assert(ASSERT_DEQUEUE + 5, !name##_full());                                      \
      name##_dequeue.buffer[name##_dequeue.back] = val;                                \
      if (name##_dequeue.back == (len - 1)) {                                          \
        name##_dequeue.back = 0;                                                       \
      } else {                                                                         \
        name##_dequeue.back++;                                                         \
      }                                                                                \
    }                                                                                  \
    if (name##_dequeue.callback) {                                                     \
      name##_dequeue.callback(DEQUEUE_EVENT_TYPE_PUSH_BACK, val);                      \
    }                                                                                  \
  }
