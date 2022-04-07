//
// Created by 29962 on 2022/4/6.
//

#include "ParserTree.h"

using namespace std;

size_t ParserTree::size() const {
    return this->idx - 1;
}

void ParserTree::add(int a, ParserTree::ParserTreeNode b) {
    e[idx] = std::move(b), ne[idx] = h[a], h[a] = idx ++ ;
}

void ParserTree::display(int root, int ident) {
    if(ident > 0) {     // 打印树枝
        for(int i = 0; i < ident - 1; ++i) {
            printf(vec_left[i] ? "│   " : "    ");
        }
        printf(vec_left[ident - 1] ? "├── " : "└── ");
    }

    if(idx < root) {    // 如果树为空，直接打印(null)
        printf("(null)\n");
        return;
    }

    printf("%s\n", e[root].info.c_str()); // 打印当前节点
    if(h[root] == -1) { // 如果当前节点无子树，直接返回
        return;
    }
    //遍历节点所有子树，分别打印
    for(int i = h[root]; i != -1; i = ne[i]){
        if(ne[i] != -1){
            vec_left[ident] = 1;
        }
        else vec_left[ident] = 0;
        display(i, ident + 1);
    }
}
