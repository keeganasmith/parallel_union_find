#ifndef HEADERFILE_H
#define HEADERFILE_H
#include <bits/stdc++.h>
#include <ygm/comm.hpp>
#include <ygm/container/map.hpp>
#include <ygm/container/set.hpp>
#include <ygm/container/bag.hpp>
#include <ygm/container/array.hpp>
#include <ygm/container/disjoint_set.hpp>
#include <ygm/io/csv_parser.hpp>
#include <chrono>

class Union_Find{
    size_t num_nodes;
    ygm::container::array<size_t>* parent_array;
    ygm::comm* world;
    ygm::ygm_ptr<Union_Find> this_ygm_ptr;
    void initialize_node_array();
    size_t retrieve_value(size_t index);
    public:
    Union_Find(size_t num_nodes, ygm::comm& world);
    void merge(size_t node_1, size_t node_2);
    ygm::container::array<size_t> data();
    size_t num_disjoint_sets();
    ~Union_Find();
};
#endif
