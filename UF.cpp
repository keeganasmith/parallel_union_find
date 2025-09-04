#include "UF.h"

Union_Find::initialize_node_array(){
    this->parent_array = new ygm::container::array<size_t>(*this->world, num_nodes);
    this->parent_array->for_all([](size_t& index, size_t& value){
        this->parent_array->async_set(index, index);
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
    size_t largest_root = max(first_root, second_root);
    size_t smallest_root = min(first_root, second_root);
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
        this->parent_array->async_set(node, parent) //path compression
    }
    return parent;
}
Union_Find::~Union_Find(){
    if(this->parent_array != nullptr){
        delete this->parent_array;
    }
}
