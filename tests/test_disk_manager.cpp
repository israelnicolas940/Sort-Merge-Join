#include "gtest/gtest.h"

TEST(DiskManagerTest, ConstructorCreatesDataDirectory) {}
TEST(DiskManagerTest, GetTableFilenameReturnsCorrectPath) {}
TEST(DiskManagerTest, EnsureDataDirectoryCreatesDirectory) {}
TEST(DiskManagerTest, ReadPageReturnsEmptyPageIfFileNotExist) {}
TEST(DiskManagerTest, ReadPageReadsCorrectRows) {}
TEST(DiskManagerTest, WritePageCreatesAndWritesFile) {}
TEST(DiskManagerTest, WritePageOverwritesExistingPage) {}
TEST(DiskManagerTest, TableFileExistsReturnsTrueIfExists) {}
TEST(DiskManagerTest, TableFileExistsReturnsFalseIfNotExists) {}
TEST(DiskManagerTest, CreateTableFileCreatesFile) {}
TEST(DiskManagerTest, GetTotalPagesReturnsZeroIfFileNotExist) {}
TEST(DiskManagerTest, GetTotalPagesReturnsCorrectPageCount) {}
