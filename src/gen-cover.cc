#include <cstdio>

#include <functional>
#include <map>
#include <vector>

#include "chip.h"
#include "dlx.h"
#include "ilog.h"

using namespace std;

int generate_covers(const vector<Shape> &chips, const Shape &weapon,
        vector<vector<int>> &sols){

    vector<vector<int>> prob;
    vector<int> ids;
    for (size_t i = 0; i < chips.size(); ++i){
        Shape chip = chips[i];

        do{ 
            int dh = weapon.h - chip.h, dw = weapon.w - chip.w;

            for (int x = 0; x <= dh; ++x)
                for (int y = 0; y <= dw; ++y){
                    auto locs = weapon.cover(chip, x, y);
                    if (locs.size() != chip.a.size())
                        continue;

                    prob.emplace_back(move(locs));
                    ids.emplace_back(i);
                }

            chip = rotate(chip);
        }
        while (chip != chips[i]);
    }

    sols.clear();
    dlx::solve_sparse(prob, sols);

    MDEBUG("%s", to_string(sols).c_str());

    for (auto &sol: sols)
        for (auto &x: sol)
            x = ids[x];

    MDEBUG("%s", to_string(sols).c_str());

    MINFO("has %zu solutions before unique", sols.size());

    return 0;
}

int write_solutions(const char *file, vector<vector<int>> &sols){
    FILE *fd = fopen(file, "w");
    if (file == nullptr){
        MERROR("failed open file");
        return 1;
    }

    fprintf(fd, "%zu\n", sols.size());
    for (auto &sol: sols){
        fprintf(fd, "%zu", sol.size());
        for (auto &e: sol)
            fprintf(fd, " %d", e);
        fputc('\n', fd);
    }
    return 0;
}

int generate_covers(const char *file, const vector<Chip> &chips, const Weapon &weapon){
    int ret;

    vector<Shape> shapes;
    for (auto &chip: chips)
        shapes.push_back((Shape)chip);

    vector<vector<int>> sols;
    ret = generate_covers(shapes, weapon, sols);
    if (ret != 0){
        MERROR("failed find solutions");
        return 1;
    }
    if (sols.size() == 0){
        MIMPT("no soluions");
        return 2;
    }

    MDEBUG("%s", to_string(sols).c_str());
    for (auto &sol: sols)
        sort(sol.begin(), sol.end());
    MDEBUG("%s", to_string(sols).c_str());
    sort(sols.begin(), sols.end());
    MDEBUG("%s", to_string(sols).c_str());
    sols.erase(unique(sols.begin(), sols.end()), sols.end());

    MINFO("has %zu solutions after unique", sols.size());

    for (auto &sol: sols)
        for (auto &id: sol)
            id = chips[id].id;

    ret = write_solutions(file, sols);
    if (ret != 0){
        MERROR("failed write solutions to %s", file);
        return 1;
    }

    return 0;
}

int main(){
    int ret;
    vector<Chip> chips;
    const char *file_chips = "./data/chips.txt";
    ret = read_chips(file_chips, chips);
    if (ret != 0){
        MDEBUG("failed read chips from %s", file_chips);
        return 1;
    }

    MINFO("chips.size() = %zu", chips.size());

    vector<Weapon> weapons;
    const char *file_weapons = "./data/weapons.txt";
    ret = read_weapons(file_weapons, weapons);
    if (ret != 0){
        MDEBUG("failed read weapons from %s", file_weapons);
        return 1;
    }

    MINFO("weapons.size() = %zu", weapons.size());
    
    map<int, Weapon> map_weapons;
    for (auto &x: weapons)
        map_weapons.emplace(x.id, x);

    vector<Chip> top_chips;
    for (auto &chip: chips)
        if (chip.gp == 0)
            top_chips.push_back(chip);

    for (int i = 0; i < 6; ++i){
        auto p = map_weapons.find(i);
        if (p == map_weapons.end())
            continue;

        Weapon &weapon = p->second;
        string str_file = format(string("./data/covers-"), weapon.name);
        MINFO("find covers of %s", weapon.name.c_str());
        int ret = generate_covers(str_file.c_str(), top_chips, weapon);
        if (ret == 2){
            int ret = generate_covers(str_file.c_str(), chips, weapon);
        }
    }

    return 0;
}
