#include "UF.h"
using std::string, std::vector, std::pair;
void verify_initialization(Union_Find& uf, size_t size, ygm::comm& world){
  bool correct = true;
  uf.data().for_all([&correct](const size_t& index, const size_t& value){
    if(index != value){
      correct = false;
    }
  });
  bool result = ygm::all_reduce(correct,[](const bool& one, const bool& two){
      return one && two;
  }, world);
  if(!result){
    world.cout0("failed initialization\n");
  } 
  else{
    world.cout0("passed initialization\n");
  }
}

void read_csv_file(const char*  csv_file, ygm::comm& world, ygm::container::bag<pair<size_t, size_t>>& edges){
  vector<string> filenames(1, csv_file);
  ygm::io::csv_parser parser(world, filenames);
  ygm::ygm_ptr edges_ptr = edges.get_ygm_ptr();
  parser.for_all([&edges_ptr](ygm::io::detail::csv_line line){
    long long vertex_one = line[0].as_integer();
    long long vertex_two = line[1].as_integer();
    edges_ptr->async_insert(pair<long long, long long>(vertex_one, vertex_two)); 
  });
  world.barrier();
}

void test(ygm::comm& world){
  size_t num_nodes = 100;
  Union_Find uf(num_nodes, world);
  verify_initialization(uf, num_nodes, world);
  world.barrier();
  int my_rank = ygm::wrank();
  int world_size = ygm::wsize();
  ygm::container::bag<pair<size_t, size_t>> edges(world);
  read_csv_file("./web-google.csv", world, edges);  
  world.barrier();
  
}
int main(int argc, char** argv){
    ygm::comm world(&argc, &argv);
    test(world); 
}
