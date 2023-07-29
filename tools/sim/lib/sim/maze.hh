#pragma once

#include <memory>
#include <string>
#include <vector>

namespace mouse::app::sim {

struct Post {
  bool north, east, south, west;
};

class Maze {
 public:
  static std::vector<std::string> List();
  static std::unique_ptr<Maze>    Load(std::string name);

 public:
  Maze(std::vector<std::vector<Post>> posts);

  void        GetSize(int &width, int &height) const;
  const Post &operator()(int x, int y) const;

 private:
  std::vector<std::vector<Post>> posts_;
};

}  // namespace mouse::app::sim
