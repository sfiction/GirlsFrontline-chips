#include <cstdio>

#include <map>
#include <vector>

#include "chip.h"
#include "ilog.h"

using namespace std;

int read_covers(FILE *fd, vector<vector<int>> &covers){
    int n;
    fscanf(fd, "%d", &n);
    covers.clear();
    covers.reserve(n);
    for (int i = 0; i < n; ++i){
        int m;
        fscanf(fd, "%d", &m);
        covers.emplace_back(m);
        for (int j = 0; j < m; ++j)
            fscanf(fd, "%d", &covers.back()[j]);
    }
    return 0;
}

vector<int> cover;
const vector<vector<Piece>>* pieces;

vector<pair<int, int>> cur;
Piece::Attr limit;
vector<pair<vector<pair<int, int>>, Piece::Attr>>* sols;

void DFS(int d, int loc, Piece::Attr attr){
    MALL("DFS %d %d %s %s", d, loc, to_string(cur).c_str(), to_string(attr).c_str());
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
        DFS(d + 1, i + 1, attr + pieces[i].val);
        cur.pop_back();
    }
}

void find_sols(const vector<int> &cover, const vector<vector<Piece>> &pieces,
        vector<pair<vector<pair<int, int>>, Piece::Attr>> &sols){
    ::cover = cover;
    ::pieces = &pieces;
    ::sols = &sols;

    cur.clear();
    DFS(0, 0, Piece::Attr{0, 0, 0, 0});

    MIMPT("find %zu solutions", sols.size());
}

int main(){
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
    constexpr int weapon_id = 5;
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

    vector<vector<int>> covers;
    read_covers(fd, covers);
    MINFO("read %zu covers from %s", covers.size(), str_file.c_str());

    vector<vector<Piece>> cand_pieces(28);
    int cnt_cand = 0;
    for (auto &piece: pieces)
        if (piece.color == Piece::Red && piece.gp <= 0){
            cnt_cand += 1;
            cand_pieces[piece.id].push_back(piece);
        }
    MDEBUG("num of candidates = %d", cnt_cand);

    ::limit = Piece::Attr{206, 60, 97, 148};
    ::limit = Piece::Attr{122, 143, 134, 240};
    vector<pair<vector<pair<int, int>>, Piece::Attr>> sols;
    for (auto &cover: covers){
        MINFO("%s", to_string(cover).c_str());
        find_sols(cover, cand_pieces, sols);
    }

    fprintf(stdout, "find %zu solutions over limit %s\n", sols.size(), to_string(::limit).c_str());
    for (auto &x: sols){
        auto &ids = x.first;
        auto &attr = x.second;
        
        vector<Piece*> t;
        for (auto &id: ids)
            t.push_back(&cand_pieces[id.first][id.second]);

        fprintf(stdout, "%s\n", to_string(attr).c_str());
        for (auto &x: t)
            fprintf(stdout, "%d %s\n", x->id, to_string(x->val).c_str());
    }

    return 0;
}
