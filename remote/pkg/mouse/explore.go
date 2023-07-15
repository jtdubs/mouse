package mouse

import (
	"sync"
)

type Explore struct {
	pathStack []uint8
	nextStack []uint8
	lock      *sync.Mutex
}

func NewExplore() *Explore {
	return &Explore{
		pathStack: make([]uint8, 0, 16),
		nextStack: make([]uint8, 0, 16),
		lock:      &sync.Mutex{},
	}
}

func (e *Explore) Read() (path, next []uint8) {
	e.lock.Lock()
	defer e.lock.Unlock()

	path = make([]uint8, len(e.pathStack))
	copy(path, e.pathStack)
	next = make([]uint8, len(e.nextStack))
	copy(next, e.nextStack)
	return
}

func (e *Explore) Update(u QueueUpdate) {
	e.lock.Lock()
	defer e.lock.Unlock()

	switch u.StackID() {
	case StackIDPath:
		switch u.Operation() {
		case PushFront:
			e.pathStack = append([]uint8{u.XY}, e.pathStack...)
		case PopFront:
			e.pathStack = e.pathStack[1:]
		case PushBack:
			e.pathStack = append(e.pathStack, u.XY)
		case PopBack:
			e.pathStack = e.pathStack[:len(e.pathStack)-1]
		}

	case StackIDNext:
		switch u.Operation() {
		case PushFront:
			e.nextStack = append([]uint8{u.XY}, e.nextStack...)
		case PopFront:
			e.nextStack = e.nextStack[1:]
		case PushBack:
			e.nextStack = append(e.nextStack, u.XY)
		case PopBack:
			e.nextStack = e.nextStack[:len(e.nextStack)-1]
		}
	}
}
