#pragma once

#include <cstdio>

#include <array>
#include <string>
#include <utility>
#include <vector>

struct Shape{
    int h, w;
    int n;
    std::vector<std::pair<int, int>> a;

    Shape(): h(0), w(0), n(0){}

    Shape(int h, int w, int n, std::vector<std::pair<int, int>> &&a)
            :h(h), w(w), n(n), a(a){
        sort();
    }

    Shape(const Shape &r)
        :h(r.h), w(r.w), n(r.n), a(r.a){}

    bool operator ==(const Shape &r) const{
        return h == r.h && w == r.w && n == r.n && a == r.a;
    }
    
    bool operator !=(const Shape &r) const{
        return !(*this == r);
    }

    operator bool() const{
        return h > 0 && w > 0;;
    }

    void sort();

    bool contain(const Shape &r, int x = 0, int y = 0) const;
    std::vector<int> cover(const Shape &r, int x, int y) const;

    std::string to_string() const;

    static Shape read(FILE *fd);

    friend Shape rotate(const Shape &r);
};

struct Chip: public Shape{
    int id;
    int star, gp;

    Chip(){}

    Chip(int id, int star, int gp, Shape &&shape)
        : id(id), star(star), gp(gp), Shape(shape){}

    std::string to_string() const;
};

static int bias;

struct Piece{
    enum Color{
        Blue = 0,
        Red = 1
    };

    Color color;
    int star, n, gp;
    int id;
    using Attr = std::array<int, 4>;
    Attr val; // damage, dbk, accuracy, loading

    Piece(): star(), n(0){}

    Piece(int color, int clazz, int id, int (&tval)[4]);

    operator bool() const{
        return star > 0 && n > 0;
    }

    std::string to_string() const;

    static Piece read(FILE *fd);
};


inline Piece::Attr operator +(const Piece::Attr &l, const Piece::Attr &r){
    Piece::Attr res{l[0] + r[0], l[1] + r[1], l[2] + r[2], l[3] + r[3]};
    return res;
}

inline bool allgt(const Piece::Attr &l, const Piece::Attr &r){
    return l[0] >= r[0]
            && l[1] >= r[1]
            && l[2] >= r[2]
            && l[3] >= r[3];
}

struct Weapon: public Shape{
    int id;
    Piece::Color color;
    std::string name;

    Weapon(){}

    Weapon(int id, Piece::Color color, std::string name, Shape &&shape)
        : id(id), color(color), name(name), Shape(shape){}
};

int read_chips(FILE *fd, std::vector<Chip> &chips);
int read_chips(const char *file, std::vector<Chip> &chips);
int read_weapons(FILE *fd, std::vector<Weapon> &weapons);
int read_weapons(const char *file, std::vector<Weapon> &weapons);
int read_pieces(FILE *fd, std::vector<Piece> &pieces);
int read_pieces(const char *file, std::vector<Piece> &pieces);

int print_shapes(std::vector<Shape> &shapes);
