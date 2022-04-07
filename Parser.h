//
// Created by 29962 on 2022/3/26.
//
#ifndef BYYL_220305_3_PARSER_H
#define BYYL_220305_3_PARSER_H

#include "Lexer.h"
#include "ParserTree.h"
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_map>
#include <stack>

# define EPSILON "$"                        // 幺元的定义
using namespace std;

typedef pair<string, string> Gram;          // 语法产生式
typedef set<string> SymbolSet;              // 符号表
typedef unordered_map<string, int>
        TOKEN_TYPE_TABLE;                   // 词法分析中的符号表

string strip(string s);
/*
 * 剔除字符串s左右两边的空格
 * */

vector<string> split(
        const string & x,                   // 需要进行分割的字符串
        const string & y                    // 使用x字符串进行分割
        );
/*
 * 使用字符串y分割字符串x，并返回字符串向量
 * */

bool add_firsts_step(
        const vector<Gram>& gram_list,      // 语法规则列表
        map<string, SymbolSet> &firsts,     // 需要进行添加的first集合
        const SymbolSet& non_term           // 非终结符列表
        );
/*
 * 遍历语法规则一次，添加first集合，并返回本轮中是否向first集合中添加元素
 * */

bool is_derivation_null(
        const vector<Gram>& gram_list,      // 语法规则列表
        const string & non_term             // 非终结符
        );
/*
 * 检查语法规则中是否存在幺元
 * */

bool is_null_in(
        const vector<string>& str_list      // 需要查找的字符串列表
        );
/*
 * 检查指定字符串列表中是否有幺元
 * */

vector<string> get_first(
        map<string, SymbolSet> &firsts,     // 所有符号的firsts集合
        const string& r_exp                        // 获取右边表达式的first集合
        );
/*
 * 获取右边表达式的first集合
 * */

bool add_follows_step(
        const vector< Gram >& gram_list,    // 语法规则列表
        map<string, SymbolSet> &firsts,     // 已经完成的firsts集合
        map<string, SymbolSet> &follows,    // 需要进行添加的follows集合
        const SymbolSet& non_term           // 非终结符列表
        );
/*
 * 遍历语法规则一次，添加follow集合，并返回本轮中是否向follow集合中添加元素
 * */

map<string, map<string, Gram>> get_analysis_table(
        const vector< Gram >& gram_list,    // 语法规则列表
        map<string, SymbolSet> &firsts,     // 已经完成的firsts集合
        map<string, SymbolSet> &follows,    // 已经完成的follows集合
        const SymbolSet& term,              // 终结符列表
        const SymbolSet& non_term           // 非终结符列表
        );
/*
 * 根据语法规则表、first集合、follow集合、非终结符列表、终结符列表获取分析表
 * */

void print_analysis_table(
        map<string,
        map<string, Gram>> analysis_table   // 分析表
        );
/*
 * 打印分析表
 * */

map<string, map<string, Gram>> gram_list2analysis_table(
        vector<Gram> gram_list,             // 语法规则列表
        bool print_mid = false              // 是否打印中间结果
                );
/*
 * 集成了前面的函数，从语法规则列表到分析表
 * */

string token_node2sym(
        const token_node& x                 // 词法分析表中的词法单元
        );
/*
 * 将词法单元转换为语法规则中的终结符
 * */

ParserTree analyse(
        const string& start_sym,            // 语法表中的开始非终结符
        vector<token_node> input,           // 词法分析器分析源代码的结果
        map<string,
        map<string, Gram>> analysis_table   // 分析表
        );

#endif //BYYL_220305_3_PARSER_H
