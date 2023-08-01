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

TEST(VCDTest, SingleBool) {
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

TEST(VCDTest, SingleUInt8) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);
  writer.Time(0);
  writer.BeginScope("foo");
  writer.Value("bar", uint8_t{10});
  writer.EndScope();
  writer.Time(1);
  writer.BeginScope("foo");
  writer.Value("bar", uint8_t{20});
  writer.EndScope();

  EXPECT_EQ(out->str(), R"($version mouse::vcd::VCDWriter $end
$timescale 1ns $end
$scope module top $end
$scope module foo $end
$var wire 8 a foo.bar[7:0] $end
$upscope $end
$upscope $end
$enddefinitions $end
#0
10a
#1
20a
)");
}

TEST(VCDTest, SingleUInt16) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);
  writer.Time(0);
  writer.BeginScope("foo");
  writer.Value("bar", uint16_t{10});
  writer.EndScope();
  writer.Time(1);
  writer.BeginScope("foo");
  writer.Value("bar", uint16_t{20});
  writer.EndScope();

  EXPECT_EQ(out->str(), R"($version mouse::vcd::VCDWriter $end
$timescale 1ns $end
$scope module top $end
$scope module foo $end
$var wire 16 a foo.bar[15:0] $end
$upscope $end
$upscope $end
$enddefinitions $end
#0
10a
#1
20a
)");
}

TEST(VCDTest, SingleUInt32) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);
  writer.Time(0);
  writer.BeginScope("foo");
  writer.Value("bar", uint32_t{10});
  writer.EndScope();
  writer.Time(1);
  writer.BeginScope("foo");
  writer.Value("bar", uint32_t{20});
  writer.EndScope();

  EXPECT_EQ(out->str(), R"($version mouse::vcd::VCDWriter $end
$timescale 1ns $end
$scope module top $end
$scope module foo $end
$var wire 32 a foo.bar[31:0] $end
$upscope $end
$upscope $end
$enddefinitions $end
#0
10a
#1
20a
)");
}

TEST(VCDTest, SingleInt8) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);
  writer.Time(0);
  writer.BeginScope("foo");
  writer.Value("bar", int8_t{10});
  writer.EndScope();
  writer.Time(1);
  writer.BeginScope("foo");
  writer.Value("bar", int8_t{-10});
  writer.EndScope();

  EXPECT_EQ(out->str(), R"($version mouse::vcd::VCDWriter $end
$timescale 1ns $end
$scope module top $end
$scope module foo $end
$var wire 8 a foo.bar[7:0] $end
$upscope $end
$upscope $end
$enddefinitions $end
#0
10a
#1
246a
)");
}

TEST(VCDTest, SingleInt16) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);
  writer.Time(0);
  writer.BeginScope("foo");
  writer.Value("bar", int16_t{10});
  writer.EndScope();
  writer.Time(1);
  writer.BeginScope("foo");
  writer.Value("bar", int16_t{-10});
  writer.EndScope();

  EXPECT_EQ(out->str(), R"($version mouse::vcd::VCDWriter $end
$timescale 1ns $end
$scope module top $end
$scope module foo $end
$var wire 16 a foo.bar[15:0] $end
$upscope $end
$upscope $end
$enddefinitions $end
#0
10a
#1
65526a
)");
}

TEST(VCDTest, SingleInt32) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);
  writer.Time(0);
  writer.BeginScope("foo");
  writer.Value("bar", int32_t{10});
  writer.EndScope();
  writer.Time(1);
  writer.BeginScope("foo");
  writer.Value("bar", int32_t{-10});
  writer.EndScope();

  EXPECT_EQ(out->str(), R"($version mouse::vcd::VCDWriter $end
$timescale 1ns $end
$scope module top $end
$scope module foo $end
$var wire 32 a foo.bar[31:0] $end
$upscope $end
$upscope $end
$enddefinitions $end
#0
10a
#1
4294967286a
)");
}

TEST(VCDTest, SingleFloat) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);
  writer.Time(0);
  writer.BeginScope("foo");
  writer.Value("bar", float{0.0});
  writer.EndScope();
  writer.Time(1);
  writer.BeginScope("foo");
  writer.Value("bar", float{1.0});
  writer.EndScope();

  EXPECT_EQ(out->str(), R"($version mouse::vcd::VCDWriter $end
$timescale 1ns $end
$scope module top $end
$scope module foo $end
$var wire 32 a foo.bar[31:0] $end
$upscope $end
$upscope $end
$enddefinitions $end
#0
0a
#1
1065353216a
)");
}

TEST(VCDTest, MoreScopes) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);

  writer.Time(0);
  writer.BeginScope("foo");
  writer.Value("apple", uint8_t{0});
  writer.BeginScope("bar");
  writer.Value("banana", uint8_t{0});
  writer.EndScope();
  writer.EndScope();
  writer.BeginScope("baz");
  writer.Value("orange", uint8_t{0});
  writer.EndScope();

  writer.Time(1);
  writer.BeginScope("foo");
  writer.Value("apple", uint8_t{1});
  writer.BeginScope("bar");
  writer.Value("banana", uint8_t{2});
  writer.EndScope();
  writer.EndScope();
  writer.BeginScope("baz");
  writer.Value("orange", uint8_t{3});
  writer.EndScope();

  EXPECT_EQ(out->str(), R"($version mouse::vcd::VCDWriter $end
$timescale 1ns $end
$scope module top $end
$scope module foo $end
$var wire 8 a foo.apple[7:0] $end
$scope module bar $end
$var wire 8 b foo.bar.banana[7:0] $end
$upscope $end
$upscope $end
$scope module baz $end
$var wire 8 c baz.orange[7:0] $end
$upscope $end
$upscope $end
$enddefinitions $end
#0
0a
0b
0c
#1
1a
2b
3c
)");
}

TEST(VCDTest, TimeDuringNonEmptyScope) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);

  writer.Time(0);
  writer.BeginScope("foo");
  EXPECT_DEATH({ writer.Time(1); }, "Assertion .* failed.");
}

TEST(VCDTest, TimeReversal) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);

  writer.Time(1);
  EXPECT_DEATH({ writer.Time(0); }, "Assertion .* failed.");
}

TEST(VCDTest, ScopeBeforeTime) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);

  EXPECT_DEATH({ writer.BeginScope("foo"); }, "Assertion .* failed.");
}

TEST(VCDTest, ExtraEndScope) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);

  writer.BeginScope("foo");
  writer.EndScope();
  EXPECT_DEATH({ writer.EndScope(); }, "Assertion .* failed.");
}

TEST(VCDTest, ValueBeforeTime) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);

  EXPECT_DEATH({ writer.Value("foo", uint8_t{0}); }, "Assertion .* failed.");
}

TEST(VCDTest, NewValueAfterHeaderClosed) {
  auto out = std::make_shared<std::ostringstream>();

  mouse::vcd::VCDWriter writer(out);
  writer.Time(0);
  writer.Value("foo", uint8_t{0});

  writer.Time(0);
  writer.Value("foo", uint8_t{1});
  EXPECT_DEATH({ writer.Value("bar", uint8_t{1}); }, "Assertion .* failed.");
}
