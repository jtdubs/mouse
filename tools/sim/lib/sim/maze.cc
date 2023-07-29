#include "maze.hh"

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

namespace app::sim {

namespace {
const std::map<wchar_t, Post> kPostLookup = {
    {L'╹', {.north = true, .east = false, .south = false, .west = false}},
    {L'╺', {.north = false, .east = true, .south = false, .west = false}},
    {L'╻', {.north = false, .east = false, .south = true, .west = false}},
    {L'╸', {.north = false, .east = false, .south = false, .west = true}},
    {L'┃', {.north = true, .east = false, .south = true, .west = false}},
    {L'━', {.north = false, .east = true, .south = false, .west = true}},
    {L'┏', {.north = false, .east = true, .south = true, .west = false}},
    {L'┓', {.north = false, .east = false, .south = true, .west = true}},
    {L'┛', {.north = true, .east = false, .south = false, .west = true}},
    {L'┗', {.north = true, .east = true, .south = false, .west = false}},
    {L'┳', {.north = false, .east = true, .south = true, .west = true}},
    {L'┣', {.north = true, .east = true, .south = true, .west = false}},
    {L'┻', {.north = true, .east = true, .south = false, .west = true}},
    {L'┫', {.north = true, .east = false, .south = true, .west = true}},
    {L'╋', {.north = true, .east = true, .south = true, .west = true}},
    {L' ', {.north = false, .east = false, .south = false, .west = false}},
};
}

std::vector<std::string> Maze::List() {
  std::vector<std::string> result;

  std::filesystem::path folder = "data/mazes";
  if (!std::filesystem::exists(folder)) {
    return result;
  }

  for (const auto &entry : std::filesystem::directory_iterator(folder)) {
    if (!entry.is_regular_file()) {
      continue;
    }

    result.push_back(entry.path().stem());
  }

  return result;
}

std::unique_ptr<Maze> Maze::Load(std::string name) {
  auto file  = std::filesystem::path("data/mazes/");
  file      /= std::filesystem::path(name + ".txt");

  std::wifstream in(file);
  in.imbue(std::locale("en_US.UTF-8"));

  std::wstringstream buf;
  buf << in.rdbuf();

  std::wstring contents = buf.str();
  if (contents.empty()) {
    return nullptr;
  }

  std::vector<std::vector<Post>> maze;

  int start = 0;
  int end   = contents.find(L'\n');
  while (end != -1) {
    std::wstring line = contents.substr(start, end - start);

    std::vector<Post> posts;
    for (wchar_t c : line) {
      posts.push_back(kPostLookup.at(c));
    }
    maze.push_back(posts);

    start = end + 1;
    end   = contents.find(L'\n', start);
  }

  std::reverse(maze.begin(), maze.end());

  return make_unique<Maze>(maze);
}

void Maze::GetSize(int &width, int &height) const {
  height = posts_.size();
  width  = posts_[0].size();
  return;
}

const Post &Maze::operator()(int x, int y) const {
  return posts_[y][x];
}

Maze::Maze(std::vector<std::vector<Post>> posts) : posts_(posts) {}

}  // namespace app::sim
