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
		if u.Push() {
			e.PathStack = append(e.PathStack, u.XY)
		} else {
			e.PathStack = e.PathStack[:len(e.PathStack)-1]
		}

	case StackIDNext:
		if u.Push() {
			e.NextStack = append(e.NextStack, u.XY)
		} else {
			e.NextStack = e.NextStack[:len(e.NextStack)-1]
		}
	}
}
