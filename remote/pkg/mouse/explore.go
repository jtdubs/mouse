package mouse

type Explore struct {
	PathStack []uint8
	NextStack []uint8
}

func NewExplore() *Explore {
	return &Explore{
		PathStack: make([]uint8, 0, 16),
		NextStack: make([]uint8, 0, 16),
	}
}

func (e *Explore) Update(u QueueUpdate) {
	switch u.StackID() {
	case StackIDPath:
		switch u.Operation() {
		case PushFront:
			e.PathStack = append([]uint8{u.XY}, e.PathStack...)
		case PopFront:
			e.PathStack = e.PathStack[1:]
		case PushBack:
			e.PathStack = append(e.PathStack, u.XY)
		case PopBack:
			e.PathStack = e.PathStack[:len(e.PathStack)-1]
		}

	case StackIDNext:
		switch u.Operation() {
		case PushFront:
			e.NextStack = append([]uint8{u.XY}, e.NextStack...)
		case PopFront:
			e.NextStack = e.NextStack[1:]
		case PushBack:
			e.NextStack = append(e.NextStack, u.XY)
		case PopBack:
			e.NextStack = e.NextStack[:len(e.NextStack)-1]
		}
	}
}
