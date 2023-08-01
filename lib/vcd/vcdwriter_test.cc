#include "lib/vcd/vcdwriter.hh"

#include <gtest/gtest.h>

#include <memory>
#include <sstream>

TEST(VCDWriter, Empty) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);
  EXPECT_EQ(out->str(), "");
}

TEST(VCDWriter, Trivial) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);
  writer.Time(0);
  writer.Time(1);

  EXPECT_EQ(out->str(), R"($version mouse::vcd::VCDWriter $end
$timescale 1ns $end
$scope module top $end
$upscope $end
$enddefinitions $end
#0
#1
)");
}

TEST(VCDTest, SingleScopeAndVariable) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);
  writer.Time(0);
  writer.BeginScope("foo");
  writer.Value("bar", true);
  writer.EndScope();
  writer.Time(1);
  writer.BeginScope("foo");
  writer.Value("bar", false);
  writer.EndScope();

  EXPECT_EQ(out->str(), R"($version mouse::vcd::VCDWriter $end
$timescale 1ns $end
$scope module top $end
$scope module foo $end
$var wire 1 a foo.bar $end
$upscope $end
$upscope $end
$enddefinitions $end
#0
1a
#1
0a
)");
}
