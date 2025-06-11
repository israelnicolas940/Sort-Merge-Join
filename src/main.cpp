#include "data_structures.h"
#include "disk_manager.h"
#include <cassert>
#include <fstream>

void test_read_file(const std::string &filename) {
  // // Prepare a test CSV file
  // std::string test_filename = "test_data.csv";
  // std::ofstream outfile(test_filename);
  // outfile << "id,name,age\n";
  // outfile << "1,Alice,30\n";
  // outfile << "2,Bob,25\n";
  // outfile << "3,Charlie,22\n";
  // outfile << "4,David,28\n";
  // outfile.close();

  // std::vector<std::vector<std::string>> rows;
  // int total_rows = 0;
  // int result = read_file(test_filename, rows, total_rows);

  // assert(result == NO_ERR);
  // assert(total_rows == 4);
  // assert(rows.size() == 4);
  // assert(rows[0][1] == "Alice");
  // assert(rows[3][2] == "28");

  // std::remove(test_filename.c_str());
}

int main(void) {
  std::string filename = "example_table.csv";
  test_read_file(filename);

  return 0;
}
