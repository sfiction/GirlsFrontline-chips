#include <cstdio>

#include <algorithm>
#include <map>
#include <vector>

#include "chip.h"
#include "ilog.h"

using namespace std;

using Cover = pair<vector<int>, string>;

int read_covers(FILE *fd, vector<Cover> &covers){
    static char buf[1024];
    int n;
    fscanf(fd, "%d", &n);
    covers.clear();
    covers.reserve(n);
    for (int i = 0; i < n; ++i){
        int m;
        fscanf(fd, "%d", &m);
        covers.emplace_back();
        Cover& cover = covers.back();
        cover.first.resize(m);
        for (int j = 0; j < m; ++j)
            fscanf(fd, "%d", &cover.first[j]);
        fscanf(fd, "%s", buf);
        cover.second = buf;
        replace(cover.second.begin(), cover.second.end(), '_', ' ');
    }
    return 0;
}

vector<int> cover;
const vector<vector<Piece>>* pieces;

vector<pair<int, int>> cur;
Piece::Attr limit;
Piece::Attr max_ratio{13, 33, 20, 15};
vector<pair<vector<pair<int, int>>, Piece::Attr>>* sols;

void DFS(int d, int loc, Piece::Attr attr, int rem_grids){
    MALL("DFS %d %d %s %s", d, loc, to_string(cur).c_str(), to_string(attr).c_str());

    if (div_ceil_sum(limit - attr, max_ratio) > rem_grids) return;

    if (d == cover.size()){
        MALL("cur %s %s", to_string(cur).c_str(), to_string(attr).c_str());
        if (allgt(attr, limit)){
            MDEBUG("cur %s %s", to_string(cur).c_str(), to_string(attr).c_str());
            sols->emplace_back(cur, attr);
        }
        return;
    }

    if (d == 0 || cover[d] != cover[d - 1])
        loc = 0;
    int k = cover[d];
    auto &pieces = ::pieces->at(k);
    for (size_t i = loc; i < pieces.size(); ++i){
        cur.emplace_back(k, i);

        MALL("DFS %d %d %s %s", d, loc, to_string(cur).c_str(), to_string(attr).c_str());
        DFS(d + 1, i + 1, attr + pieces[i].val, rem_grids - pieces[i].n);
        cur.pop_back();
    }
}

void find_sols(const vector<int> &cover, int sum_grids, const vector<vector<Piece>> &pieces,
        vector<pair<vector<pair<int, int>>, Piece::Attr>> &sols){
    ::cover = cover;
    ::pieces = &pieces;
    ::sols = &sols;

    cur.clear();
    DFS(0, 0, Piece::Attr{0, 0, 0, 0}, sum_grids);

    MIMPT("find %zu solutions", sols.size());
}

int main(){
    constexpr int weapon_id = 6;

    int ret;

    vector<Piece> pieces;
    const char *file_pieces = "./user/chips-inner";
    ret = read_pieces(file_pieces, pieces);
    if (ret != 0){
        MDEBUG("failed read pieces from %s", file_pieces);
        return 1;
    }

    MINFO("pieces.size() = %zu", pieces.size());

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

    // select weapon
    auto p = map_weapons.find(weapon_id);
    if (p == map_weapons.end()){
        MERROR("cant find the weapon %d", weapon_id);
        return 1;
    }
    auto &weapon = p->second;
    string str_file = format(string("./data/covers-"), weapon.name);
    FILE *fd = fopen(str_file.c_str(), "r");
    if (fd == nullptr){
        MERROR("failed read covers from %s", str_file.c_str());
        return 1;
    }

    vector<Cover> covers;
    read_covers(fd, covers);
    MINFO("read %zu covers from %s", covers.size(), str_file.c_str());

    vector<vector<Piece>> cand_pieces(28);
    int cnt_cand = 0;
    for (auto &piece: pieces)
        if (piece.color == weapon.color && piece.gp <= 0){
            cnt_cand += 1;
            cand_pieces[piece.id].push_back(piece);
        }
    MINFO("num of candidates = %d", cnt_cand);

    ::limit = Piece::Attr{206, 60, 97, 148};
    ::limit = Piece::Attr{185, 328, 135, 43};
    ::limit = Piece::Attr{122, 143, 132, 240};  // 5
    ::limit = Piece::Attr{162, 260, 172, 68};  // 4
    ::limit = Piece::Attr{195, 263, 158 - 2, 73}; // 6
    // ::limit = Piece::Attr{189, 263, 176 - 2, 73}; // 6
    vector<pair<vector<pair<int, int>>, Piece::Attr>> sols;
    vector<int> cover_head;
    for (auto &cover: covers){
        cover_head.push_back(sols.size());
        MINFO("%s", to_string(cover).c_str());
        find_sols(cover.first, weapon.n, cand_pieces, sols);
    }
    cover_head.push_back(sols.size());

    fprintf(stdout, "find %zu solutions over limit %s\n", sols.size(), to_string(::limit).c_str());
    for (size_t j = 0, i = 0; i < sols.size(); ++i){
        for (; i == cover_head[j]; ++j);
        for (size_t h = 0; h < weapon.h; ++h)
            fprintf(stdout, "%s\n", covers[j - 1].second.substr(h * weapon.w, weapon.w).c_str());

        auto& x = sols[i];
        auto &ids = x.first;
        auto &attr = x.second;
        
        vector<Piece*> t;
        for (auto &id: ids)
            t.push_back(&cand_pieces[id.first][id.second]);

        fprintf(stdout, "%s\n", to_string(attr).c_str());
        for (auto &x: t)
            fprintf(stdout, "%d %s %s\n", x->id, to_string(x->val).c_str(), to_string(x->val0).c_str());
    }

    return 0;
}
