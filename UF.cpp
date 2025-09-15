#include "UF.h"

void Union_Find::initialize_node_array(){
    this->parent_array = new ygm::container::array<size_t>(*this->world, num_nodes);
    this->parent_array->for_all([](const size_t& index, size_t& value){
        value = index;
    });
}
Union_Find::Union_Find(size_t num_nodes, ygm::comm& world): num_nodes{num_nodes}, world{&world}{
    this->initialize_node_array();
    this->this_ygm_ptr = ygm::ygm_ptr<Union_Find>(this);
}
bool Union_Find::merge(size_t node_1, size_t node_2){ //true for merged, false for already in the same set
    size_t current_worst_root = std::max(node_1, node_2);
    size_t best_root = std::min(node_1, node_2);
    if(current_worst_root == best_root){
        return false;
    }
    
    bool merged = false;
    ygm::ygm_ptr<bool> merged_ptr(&merged); 
    this->parent_array->async_visit(current_worst_root, [](const size_t& current_worst_root, size_t& value, const size_t best_root, const ygm::ygm_ptr<bool>& merged_ptr, const ygm::ygm_ptr<Union_Find>& this_ygm_ptr){
        size_t new_current_worst_root = std::max(current_worst_root, value);
        if(current_worst_root == new_current_worst_root){ //reached a root
            //we need to merge into best root
            value = best_root;
            *merged_ptr = true;
        }
        else{
            *merged_ptr = false;
        }
    }, best_root, merged_ptr, this_ygm_ptr);
    return *merged_ptr;
}

ygm::container::array<size_t> Union_Find::data(){
  return *(this->parent_array);
}
size_t Union_Find::num_disjoint_sets(){
  size_t num_sets = 0;
  this->parent_array->for_all([&num_sets](const size_t& index, size_t& value){
    if(index == value){
        num_sets++;
    }
  });
  return num_sets;
}
Union_Find::~Union_Find(){
    if(this->parent_array != nullptr){
        delete this->parent_array;
    }
}
