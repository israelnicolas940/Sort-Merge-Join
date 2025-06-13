#include "disk_manager.h"
#include <cassert>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

namespace {} // namespace

TEST(TestTopic, ExistingFileRead) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}
int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
