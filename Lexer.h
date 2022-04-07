//
// Created by shipingg on 2021/10/10.
//

#ifndef GSP_PROGRAM_LANGUAGE_LEXER_H
#define GSP_PROGRAM_LANGUAGE_LEXER_H

#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
using namespace std;
typedef vector<string> token_list;
typedef vector<char> ch_list;

typedef struct token_node{
    int index;
    string value;
    string type;
}token_node;

class Lexer {
private:
    // 类别表
    static unordered_map<string, int> TOKEN_TYPE_TABLE;

    // 关键字列表
    static token_list KEYWORD;

    // 分隔符列表
    static ch_list SEPARATOR;

    // 运算符列表
    static ch_list OPERATOR;

    // 空白符号列表
    static ch_list WHITE_SPACE;


    /**判断是否为关键字**/
    static bool IsKeyword(const string& word);

    /**判断是否为分隔符**/
    static bool IsSeparater(char ch);

    /**判断是否为运算符**/
    static bool IsOperator(char ch);

    /**判断是否为空白符号**/
    static bool IsWhiteSpace(char ch);

    /**判断是否为大写字母**/
    static bool IsUpLetter(char ch);

    /**判断是否为小写字母**/
    static bool IsLowLetter(char ch);

    /**判断是否为小写字母**/
    static bool IsLetter(char ch);

    /**判断是否为数字**/
    static bool IsDigit(char ch);

public:
    // 符号表
    static unordered_map<string, int> SYMBOL_TABLE;

    // 索引映射
    typedef struct{
        string type, value;
    }TYPE_VALUE;
    static unordered_map<int, TYPE_VALUE> ID2VALUE;

    static int symbol_index, type_index;
    static vector<token_node> lexical_analyse(FILE * fpin, bool if_print) ;
    Lexer();
};


#endif //GSP_PROGRAM_LANGUAGE_LEXER_H
