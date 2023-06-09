package mouse

import "sync"

type Ring[T any] struct {
	buffer      []T
	len         int
	read, write int
	lock        *sync.Mutex
}

func NewRing[T any](len int) *Ring[T] {
	return &Ring[T]{
		buffer: make([]T, len+1),
		len:    len + 1,
		lock:   &sync.Mutex{},
	}
}

func (r *Ring[T]) Clear() {
	r.lock.Lock()
	defer r.lock.Unlock()

	r.read = 0
	r.write = 0
}

func (r *Ring[T]) Add(data T) {
	r.lock.Lock()
	defer r.lock.Unlock()

	r.buffer[r.write] = data
	r.write = (r.write + 1) % r.len
	if r.write == r.read {
		r.read = (r.read + 1) % r.len
	}
}

func (r *Ring[T]) ForEach(f func(T)) {
	r.lock.Lock()
	defer r.lock.Unlock()

	if r.read <= r.write {
		for _, v := range r.buffer[r.read:r.write] {
			f(v)
		}
	} else {
		for _, v := range r.buffer[r.read:] {
			f(v)
		}
		for _, v := range r.buffer[:r.write] {
			f(v)
		}
	}
}
