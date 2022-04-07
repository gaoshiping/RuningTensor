//
// Created by 29962 on 2022/4/6.
//

#ifndef BYYL_220305_3_PARSERTREE_H
#define BYYL_220305_3_PARSERTREE_H

#include <iostream>
#include <cstdio>
#include <utility>
#include <cstring>
#define MAX_SIZE 5000
using namespace std;


class ParserTree{
private:
    typedef struct {
        string info;
    }ParserTreeNode;
    int h[MAX_SIZE]{}, ne[MAX_SIZE]{}, idx{};
    int vec_left[100]{};
    ParserTreeNode e[MAX_SIZE];

public:
    size_t size() const;                        // 获取语法树的节点数目
    void add(int a, ParserTreeNode b);          // 将b节点添加至编号为a的节点的子节点中
    void display(int root = 0, int ident = 0);  // 打印语法树，默认参数是打印整棵语法树
    ParserTree(){
        memset(vec_left, 0, sizeof vec_left);
        memset(h, -1, sizeof h);
    }
};

#endif //BYYL_220305_3_PARSERTREE_H
