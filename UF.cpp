#include "UF.h"

void Union_Find::initialize_node_array(){
    this->parent_array = new ygm::container::array<size_t>(*this->world, num_nodes);
    this->parent_array->for_all([](const size_t& index, size_t& value){
        value = index;
    });
}
Union_Find::Union_Find(size_t num_nodes, ygm::comm& world): num_nodes{num_nodes}, world{&world}{
    this->initialize_node_array();
}
bool Union_Find::merge(size_t node_1, size_t node_2){ //true for merged, false for already in the same set
    size_t first_root = find(node_1);
    size_t second_root = find(node_2);
    if(first_root == second_root){
        return false;
    }
    size_t largest_root = std::max(first_root, second_root);
    size_t smallest_root = std::min(first_root, second_root);
    this->parent_array->async_set(smallest_root, largest_root); //parent_array[smallest_root] = largest_root now
    return true;
}
size_t Union_Find::retrieve_value(size_t index){
    size_t result = 0;
    this->parent_array->async_visit(index, [&result](size_t index, size_t value){
        result = value;
    });
    return result;
}
size_t Union_Find::find(size_t node){
    size_t parent = retrieve_value(node);
    if(parent != node){
        parent = this->find(parent);
        this->parent_array->async_set(node, parent); //path compression
    }
    return parent;
}
ygm::container::array<size_t> Union_Find::data(){
  return *(this->parent_array);
}
size_t Union_Find::num_disjoint_sets(){
  ygm::container::set<size_t> unique_sets(*this->world);
  ygm::ygm_ptr set_ptr= unique_sets.get_ygm_ptr();
  this->parent_array->for_all([set_ptr](const size_t& index, size_t& value){
    set_ptr->async_insert(value);
  });
  return unique_sets.size();
}
Union_Find::~Union_Find(){
    if(this->parent_array != nullptr){
        delete this->parent_array;
    }
}
