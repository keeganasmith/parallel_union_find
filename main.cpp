#include "UF.h"

int main(int argc, char** argv){
    ygm::comm world(&argc, &argv);
    Union_Find uf(100, world);
    
}