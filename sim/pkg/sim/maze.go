package sim

import (
	"fmt"
	"log"
	"os"

	"path/filepath"
	"strings"
)

type Post struct {
	North, South, East, West bool // Whether or not there are walls to the east and south of this post
}

type Maze struct {
	Posts [][]Post
	mazes []string
}

func newMaze() *Maze {
	m := &Maze{}
	m.Load(m.ListMazes()[0])
	return m
}

func (m *Maze) ListMazes() []string {
	if m.mazes == nil {
		files, err := filepath.Glob("pkg/sim/mazes/*.txt")
		if err != nil {
			log.Fatalf("Error listing mazes: %v", err)
		}

		for i, path := range files {
			files[i] = strings.TrimSuffix(filepath.Base(path), filepath.Ext(path))
		}
		m.mazes = files
	}
	return m.mazes
}

func (m *Maze) Load(name string) {
	bs, err := os.ReadFile(fmt.Sprintf("pkg/sim/mazes/%s.txt", name))
	if err != nil {
		log.Fatalf("Error loading maze: %v", err)
	}

	lines := strings.Split(string(bs), "\n")
	lines = lines[:len(lines)-1]
	height := len(lines)
	if height == 0 {
		log.Fatalf("Error loading maze: no lines")
	}

	for x, line := range lines {
		lines[x] = strings.TrimSpace(line)
	}

	width := len([]rune(lines[0]))
	if width == 0 {
		log.Fatalf("Error loading maze: no columns")
	}

	m.Posts = make([][]Post, height)
	for i := range m.Posts {
		m.Posts[i] = make([]Post, width)
	}

	log.Printf("Load(%s) = %v x %v", name, len(m.Posts[0]), len(m.Posts))

	for y, line := range lines {
		for x, post := range []rune(line) {
			p, ok := postLookup[post]
			if !ok {
				log.Fatalf("Error loading maze: unknown character %c", post)
			}
			m.Posts[len(lines)-y-1][x] = p
		}
	}
}

var postLookup = map[rune]Post{
	'╹': {North: true},
	'╺': {East: true},
	'╻': {South: true},
	'╸': {West: true},
	'┃': {North: true, South: true},
	'━': {East: true, West: true},
	'┏': {South: true, East: true},
	'┓': {South: true, West: true},
	'┛': {North: true, West: true},
	'┗': {North: true, East: true},
	'┳': {South: true, East: true, West: true},
	'┣': {North: true, South: true, East: true},
	'┻': {North: true, East: true, West: true},
	'┫': {North: true, South: true, West: true},
	'╋': {North: true, South: true, East: true, West: true},
	' ': {},
}
