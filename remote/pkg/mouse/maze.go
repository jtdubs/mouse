package mouse

type Maze struct {
	Cells [][]Cell
}

type Cell struct {
	North, East, South, West bool
	Distance                 uint8
}

func (m *Maze) Update(u MazeUpdate) {
	var w, h int
	w = len(m.Cells)
	if w > 0 {
		h = len(m.Cells[0])
	}

	x, y := u.X(), u.Y()
	if x >= w {
		m.Cells = append(m.Cells, make([][]Cell, (x-w)+1)...)
		for i := range m.Cells[w:] {
			m.Cells[w+i] = make([]Cell, h)
		}
	}

	if y >= h {
		for i := range m.Cells {
			m.Cells[i] = append(m.Cells[i], make([]Cell, (y-h)+1)...)
		}
	}

	n, e, s, we := u.Walls()
	m.Cells[x][y] = Cell{
		North:    n,
		East:     e,
		South:    s,
		West:     we,
		Distance: u.Distance(),
	}
}
