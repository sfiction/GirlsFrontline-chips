#include <cstdio>

#include <algorithm>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "chip.h"
#include "ilog.h"

using namespace std;
    
void Shape::sort(){
    std::sort(a.begin(), a.end());
}
    
bool Shape::contain(const Shape &r, int x, int y) const{
    MALL("l: %s", this->to_string().c_str());
    MALL("r: %s", r.to_string().c_str());
    MALL("x: %d, y: %d", x, y);
    for (auto &loc: r.a){
        pair<int, int> t = make_pair(loc.first + x, loc.second + y);
        auto p = lower_bound(a.begin(), a.end(), t);
        if (p == a.end() || *p != t)
            return false;
    }
    return true;
}
vector<int> Shape::cover(const Shape &r, int x, int y) const{
    MALL("l: %s", this->to_string().c_str());
    MALL("r: %s", r.to_string().c_str());
    MALL("x: %d, y: %d", x, y);
    vector<int> res;
    for (auto &loc: r.a){
        pair<int, int> t = make_pair(loc.first + x, loc.second + y);
        auto p = lower_bound(a.begin(), a.end(), t);
        if (p == a.end() || *p != t)
            continue;
        res.push_back(p - a.begin());
    }
    return res;
}

string Shape::to_string() const{
    stringstream ss;

    ss << "(" << h << "," << w << "," << n << ",[";
    for (auto &x: a)
        ss << "(" << x.first << "," << x.second << "),";
    ss << "])";
    return ss.str();
}

Shape Shape::read(FILE *fd){
    int h, w, n;
    int ret = fscanf(fd, "%d%d%d", &h, &w, &n);
    if (ret != 3)
        return Shape();

    vector<pair<int, int>> a(n);
    for (int i = 0; i < n; ++i){
        int loc;
        int ret = fscanf(fd, "%d", &loc);
        if (ret != 1)
            return Shape();

        a[i].first = loc / w;
        a[i].second = loc % w;
    }

    Shape res(h, w, n, move(a));

    return res;
}

static int gp_weights[3] = {100, 92, 0};
static int attr_weights[4] = {44, 127, 71, 57};

Piece::Piece(int color, int clazz, int id, int (&tval)[4])
        : color((Color)color),
            star(clazz >> 8 & 0xf), n(clazz >> 4 & 0xf), gp(clazz & 0xf),
            id(id){
    for (size_t i = 0; i < 4; ++i){
        int gp = clazz & 0xf;
        int gp_weight = gp_weights[min(gp, 2)];
        int attr_weight = attr_weights[i];

        val[i] = (tval[i] * attr_weight * gp_weight + 999) / 1000;
        val0[i] = val[i];
        val[i] = (val[i] * 25 + 9) / 10;
    }
}

std::string Piece::to_string() const{
    stringstream ss;
    ss << "(" << color << "," << star << "," << n << "," << gp << "," << id
        << ",["
        << val[0] << "," << val[1] << "," << val[2] << "," << val[3]
        << "])";
    return ss.str();
}

Piece Piece::read(FILE *fd){
    int color, clazz, id;
    int val[4];
    int ret = fscanf(fd, "%d%d%d", &color, &clazz, &id);
    MALL("%d %d %d", color, clazz, id);
    if (ret != 3)
        return Piece();

    for (int i = 0; i < 4; ++i){
        ret = fscanf(fd, "%d", val + i);
        if (ret != 1)
            return Piece();
    }

    Piece res(color, clazz, id, val);

    return res;
}

Shape rotate(const Shape &r){
    Shape res(r);
    for (auto &x: res.a)
        x = make_pair(x.second, r.h - 1 - x.first);
    swap(res.h, res.w);
    res.sort();
    return res;
}

string Chip::to_string() const{
    stringstream ss;
    ss << "(" << id << "," << star << "," << gp << ",";
    ss << Shape::to_string() << ")";
    return ss.str();
}

int read_chips(FILE *fd, vector<Chip> &chips){
    chips.clear();
    
    int star, n, gp, id;
    while (fscanf(fd, "%d%d%d%d", &star, &n, &gp, &id) == 4){
        chips.emplace_back(id, star, gp, Shape::read(fd));
        MALL("%s", chips.back().to_string().c_str());
    }
    return 0;
}

int read_chips(const char *file, vector<Chip> &chips){
    FILE *fd = fopen(file, "r");
    if (fd == nullptr){
        MERROR("failed open file %s", file);
        return 1;
    }

    read_chips(fd, chips);
    fclose(fd);
    return 0;
}

int read_weapons(FILE *fd, vector<Weapon> &weapons){
    weapons.clear();

    int id, color;
    char buf[20];
    while (fscanf(fd, "%d%d", &id, &color) == 2){
        fscanf(fd, "%10s", buf);
        weapons.emplace_back(id, (Piece::Color)color, buf, Shape::read(fd));
        MALL("%s", weapons.back().to_string().c_str());
    }
    return 0;
}

int read_weapons(const char *file, vector<Weapon> &weapons){
    FILE *fd = fopen(file, "r");
    if (fd == nullptr){
        MERROR("failed open file %s", file);
        return 1;
    }

    read_weapons(fd, weapons);
    fclose(fd);
    return 0;
}
int read_pieces(FILE *fd, vector<Piece> &pieces){
    pieces.clear();

    Piece piece;
    while ((piece = Piece::read(fd))){
        pieces.push_back(piece);

        MALL("%s", piece.to_string().c_str());
    }
        
    return 0;
}

int read_pieces(const char *file, vector<Piece> &pieces){
    FILE *fd = fopen(file, "r");
    if (fd == nullptr){
        MERROR("failed open file %s", file);
        return 1;
    }

    read_pieces(fd, pieces);
    fclose(fd);
    return 0;
}

int print_shapes(vector<Shape> &shapes){
    for (auto &shape: shapes){
        printf("%d %d\n", shape.h, shape.w);

        set<pair<int, int>> lst(shape.a.begin(), shape.a.end());
        for (int i = 0; i < shape.h; ++i){
            for (int j = 0; j < shape.w; ++j)
                putchar(" *"[lst.find(make_pair(i, j)) != lst.end()]);
            puts("");
        }
    }
    return 0;
}
