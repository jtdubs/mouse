package sim

type Post struct {
	East, South bool // Whether or not there are walls to the east and south of this post
}

type Maze struct {
	Posts [][]Post
}

func NewMaze(width, height int) *Maze {
	m := &Maze{
		Posts: make([][]Post, height),
	}
	for y := 0; y < height; y++ {
		m.Posts[y] = make([]Post, width)
	}
	return m
}
