#pragma once

#include <memory>
#include <string>
#include <vector>

namespace sim {

struct Post {
  bool north, south, east, west;
};

class Maze {
 public:
  static std::vector<std::string> List();
  static std::unique_ptr<Maze>    Load(std::string name);

 public:
  Maze(std::vector<std::vector<Post>> posts);

  const void  GetSize(int &width, int &height) const;
  const Post &operator()(int x, int y) const;

 private:
  std::vector<std::vector<Post>> posts_;
};

}  // namespace sim
