#include <cstdio>
#include "chip.h"

using namespace std;

int main(){
    vector<Shape> shapes;
    read_s(stdin, shapes);
    print_shapes(shapes);
    return 0;
}
