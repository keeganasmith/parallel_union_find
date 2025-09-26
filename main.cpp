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
  size_t max_val = INT32_MIN;
  parser.for_all([&edges_ptr, &max_val](ygm::io::detail::csv_line line){
    size_t vertex_one = line[0].as_integer();
    size_t vertex_two = line[1].as_integer();
    edges_ptr->async_insert(pair<long long, long long>(vertex_one, vertex_two)); 
    max_val = std::max(std::max(vertex_one, vertex_two), max_val);
  });
  max_val = ygm::max(max_val, world);
  world.cout0("max value: ", max_val);
}

void test(ygm::comm& world){
  size_t num_nodes = 100;
  Union_Find uf(num_nodes, world);
  verify_initialization(uf, num_nodes, world);
  world.barrier();
  int my_rank = ygm::wrank();
  int world_size = ygm::wsize();
  size_t num_nodes_to_merge = num_nodes / world_size;
  //if wrank = 4:
  //  rank zero merges every 0 (mod 4) and 1 (mod 4) nodes
  //  rank one merges every 1 (mod 4) and 2 (mod 4) nodes
  //  rank two merges every 2 (mod 4) and 3 (mod 4) nodes
  //  rank three doesn't do anything
  //in general:
  //  rank n merges every n (mod wsize) and n + 1 (mod wsize) nodes
  //this should result in ceil(num_nodes / wsize) disjoint sets
  if(my_rank != world_size - 1){
    for(size_t i = my_rank; i < num_nodes - 1; i += world_size){
      uf.merge(i, i + 1);
    }
  }
  world.barrier();
  size_t num_disjoint_sets = uf.num_disjoint_sets();
  if(my_rank == 0){
    if(num_disjoint_sets != std::ceil((double)num_nodes / (double)world_size)){
      std::cout << "incorrect number of disjoint sets. Expected " << num_nodes / world_size << " got " << num_disjoint_sets << "\n";
    }
    else{
      std::cout << "correct number of disjoint sets\n";
    }
  }
}

int main(int argc, char** argv){
    ygm::comm world(&argc, &argv);
    test(world); 
}
