#include <cstdio>
#include <vector>

#include "dlx.h"

using namespace std;

int main(){
    vector<vector<int>> prob = {
    };
    prob.push_back({1, 2});
    prob.push_back({0, 2});
    prob.push_back({1, 3});
    prob.push_back({0, 3});
    prob.push_back({0, 3});

    vector<vector<int>> sols;
    dlx::solve_sparse(prob, sols);
    for (auto &sol: sols)
        for (size_t i = 0; i < sol.size(); ++i)
            printf("%d%c", sol[i], "\n "[i + 1 < sol.size()]);
    return 0;
}
