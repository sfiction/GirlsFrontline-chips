#include <cstdio>
#include <vector>

#include "dlx.h"
#include "ilog.h"

using std::max;
using std::vector;

namespace dlx{
    struct node{
        node *l, *r, *u, *d, *c;
        int row;

        node(): l(this), r(this), u(this), d(this), c(nullptr){
        }

        node(node *l, node *r, node *u, node *d, node *c = nullptr)
            : l(l), r(r), u(u), d(u), c(c){
        }

        void pushl(node *x){
            x->l = l, l->r = x;
            x->r = this, l = x;
        }

        void pushr(node *x){
            x->r = r, r->l = x;
            x->l = this, r = x;
        }

        void pushu(node *x){
            x->u = u, u->d = x;
            x->d = this, u = x;
        }

        void pushd(node *x){
            x->d = d, d->u = x;
            x->u = this, d = x;
        }
    };

    inline void linklr(node *l, node *r){
        l->r = r, r->l = l;
    }

    inline void linkud(node *u, node *d){
        u->d = d, d->u = u;
    }

    inline void coverlr(node *x){
        x->l->r = x->r;
        x->r->l = x->l;
    }

    inline void coverud(node *x){
        x->u->d = x->d;
        x->d->u = x->u;
    }

    inline void uncoverlr(node *x){
        x->l->r = x, x->r->l = x;
    }

    inline void uncoverud(node *x){
        x->u->d = x, x->d->u = x;
    }

    inline void covercol(node *c){
        coverlr(c);
        for (node *r = c->d; r != c; r = r->d){
            for (node *x = r->r; x != r; x = x->r){
                coverud(x);
            }
        }
    }

    inline void uncovercol(node *c){
        for (node *r = c->d; r != c; r = r->d)
            for (node *x = r->r; x != r; x = x->r)
                uncoverud(x);
        uncoverlr(c);
    }

    void find_all_exact(node *rt, vector<int> &cur, vector<vector<int>> &sols){
        if (rt->r == rt){
            sols.emplace_back(cur);
            return;
        }

        node *c = rt->r;
        covercol(c);
        for (node *r = c->d; r != c; r = r->d){ 
            for (node *x = r->r; x != r; x = x->r)
                covercol(x->c);
            cur.push_back(r->row);
            find_all_exact(rt, cur, sols);
            cur.pop_back();
            for (node *x = r->r; x != r; x = x->r)
                uncovercol(x->c);
        }
        uncovercol(c);
    }

    int solve_sparse(const vector<vector<int>> &rows, vector<vector<int>> &sols){
        sols.clear();
        MINFO("matrix has %zu rows", rows.size());

        node rt;
        vector<node*> dummy_cols;
        vector<node*> pool;
        for (size_t i = 0; i < rows.size(); ++i){
            auto &row = rows[i];
            node *t = &rt;
            for (auto &e: row){
                while (e >= dummy_cols.size())
                    dummy_cols.push_back(new node);

                pool.push_back(new node);
                node *x = pool.back();
                x->c = dummy_cols[e];
                x->row = i;

                dummy_cols[e]->pushu(x);
                linklr(t, x);
                t = x;
            }
            linklr(t, rt.r);
        }
        
        if (dummy_cols.size() == 0){
            MINFO("no column in prob");
            return 0;
        }

        for (size_t i = 1; i < dummy_cols.size(); ++i)
            linklr(dummy_cols[i - 1], dummy_cols[i]);
        linklr(&rt, dummy_cols[0]);
        linklr(dummy_cols.back(), &rt);

        vector<int> cur;
        find_all_exact(&rt, cur, sols);

        for (auto x: dummy_cols)
            delete x;
        for (auto x: pool)
            delete x;
        return 0;
    }
}
