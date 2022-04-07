//
// Created by shipingg on 2021/10/10.
//

#include "Lexer.h"
#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>

token_list Lexer::KEYWORD = {
        "Tensor", "int", "float", "bool", "stdout", "stdin", "stderr", "write", "read"
};
ch_list Lexer::OPERATOR = {
        '+', '-', '*', '/', '^', '='
};
ch_list Lexer::SEPARATOR = {
        '[', ']', '<', '>', ';', ',', '(', ')'
};
ch_list Lexer::WHITE_SPACE = {
        ' ', '\t', '\r', '\n'
};

unordered_map<string, int> Lexer::TOKEN_TYPE_TABLE = {};
unordered_map<string, int> Lexer::SYMBOL_TABLE = {};
int Lexer::symbol_index = 0;
int Lexer::type_index = 0;

Lexer::Lexer() {
    // 对于所有的符号列表进行排序，方便之后二分查找
    sort(Lexer::KEYWORD.begin(), Lexer::KEYWORD.end());
    sort(Lexer::OPERATOR.begin(), Lexer::OPERATOR.end());
    sort(Lexer::SEPARATOR.begin(), Lexer::SEPARATOR.end());
    sort(Lexer::WHITE_SPACE.begin(), Lexer::WHITE_SPACE.end());

    // 构建类型表
    type_index = 0;
    for(const auto& s: KEYWORD)
        TOKEN_TYPE_TABLE[s] = type_index ++;
    for(const auto& s: OPERATOR)
        TOKEN_TYPE_TABLE[string(1,s)] = type_index ++;
    for(const auto& s: SEPARATOR)
        TOKEN_TYPE_TABLE[string(1,s)] = type_index ++;

    TOKEN_TYPE_TABLE["const_tensor"] =  type_index ++;
    TOKEN_TYPE_TABLE["const_int"] =     type_index ++;
    TOKEN_TYPE_TABLE["const_float"] =   type_index ++;
    TOKEN_TYPE_TABLE["const_bool"] =    type_index ++;
    TOKEN_TYPE_TABLE["const_str"] =     type_index ++;
    TOKEN_TYPE_TABLE["const_char"] =    type_index ++;

    // 符号表的索引初始化
    symbol_index = 0;
}

bool Lexer::IsKeyword(const string& word) {
    return binary_search(KEYWORD.begin(), KEYWORD.end(), word);
}


/**判断是否为分隔符**/
bool Lexer::IsSeparater(char ch){
    return binary_search(SEPARATOR.begin(), SEPARATOR.end(), ch);
}

/**判断是否为运算符**/
bool Lexer::IsOperator(char ch){
    return binary_search(OPERATOR.begin(), OPERATOR.end(), ch);
}

/**判断是否为空白符号**/
bool Lexer::IsWhiteSpace(char ch){
    return binary_search(WHITE_SPACE.begin(), WHITE_SPACE.end(), ch);
}
/**判断是否为大写字母**/
bool Lexer::IsUpLetter(char ch){
    return ch>='A' && ch<='Z';
}
/**判断是否为小写字母**/
bool Lexer::IsLowLetter(char ch){
    return (ch>='a' && ch<='z') || ch == '_';
}

/**判断是否为小写字母**/
bool Lexer::IsLetter(char ch){
    return IsUpLetter(ch) || IsLowLetter(ch);
}

/**判断是否为数字**/
bool Lexer::IsDigit(char ch){
    return ch>='0' && ch<='9';
}

vector<token_node> Lexer::lexical_analyse(FILE *fpin, bool if_print) {
    vector<token_node> result;
    char ch;
    string arr;
    while((ch=(char)fgetc(fpin))!=EOF){
        arr="";

        // 注释处理
        if(ch == '/'){
            ch = (char) fgetc(fpin);

            // 不是注释，还原状态
            if (ch != '/' && ch != '*'){
                ch = '/';
                fseek(fpin,-1L, SEEK_CUR);
            }

            // 单行注释
            if (ch == '/'){
                do {ch = (char)fgetc(fpin);} while (ch != '\n');
            }

            // 多行注释
            else {
                do {ch = (char)fgetc(fpin);} while(ch != '*');  // 状态2
                do {ch = (char)fgetc(fpin);} while(ch == '*');  // 状态3

                // 回到状态2，即注释里面出现 / 或 *
                while(ch != '/'){
                    do {ch = (char)fgetc(fpin);} while(ch != '*');  // 状态2
                    do {ch = (char)fgetc(fpin);} while(ch == '*');  // 状态3
                }
                // 多行注释完成，以 / 结尾
                continue;
            }
        }

        // 如果是空白符号，则直接跳过
        if(IsWhiteSpace(ch))
            continue;

        // 判断字符常数
        else if(ch == '\''){
            ch = (char)fgetc(fpin);
            arr += ch;

            // 转义字符
            if (ch == '\\'){
                ch = (char)fgetc(fpin);
                arr += ch;
            }

            // 判断字符结束
            ch = (char)fgetc(fpin);
            if(ch == '\'')
                result.push_back({Lexer::TOKEN_TYPE_TABLE["const_char"], arr, "const_char"});
            else fprintf(stderr, "unknown string: %s\n", arr.c_str());
        }

        // 判断字符串
        else if(ch == '\"'){
            char last_char;
            do {last_char = ch; ch = (char)fgetc(fpin); arr += ch;} while (ch != '\"' || last_char == '\\');
            result.push_back({Lexer::TOKEN_TYPE_TABLE["const_str"], arr.substr(0, arr.length() - 1), "const_string"});
        }

        //判断是否为关键字或字
        else if(IsLetter(ch)){
            while(IsLetter(ch) || IsDigit(ch)) {
                arr += ch;
                ch = (char) fgetc(fpin);
            }
            fseek(fpin, -1L, SEEK_CUR);

            // 关键词
            if(IsKeyword(arr)){
                result.push_back({Lexer::TOKEN_TYPE_TABLE[arr], arr, "key_word"});
            }
            // 如果是标识符，则需要加入符号表
            else{
                if(!Lexer::SYMBOL_TABLE.count(arr)){
                    Lexer::SYMBOL_TABLE[arr] = symbol_index ++;
                }
                result.push_back({Lexer::type_index++, arr, "id"});
            }
        }

        //判断是否为数字
        else if(IsDigit(ch)){
            int point_count = 0;    // 记录这段字符串中小数点的数量
            while(IsDigit(ch) || ch == '.'){
                if(ch == '.') point_count ++;
                arr += ch;
                ch = (char)fgetc(fpin);
            }
            fseek(fpin, -1L, SEEK_CUR);

            // 小数点多于1个，或小数点是位于数字字符串最后一个位置，则不是数字
            if(point_count > 1 || arr[arr.length() - 1] == '.'){
                fprintf(stderr, "unknown string: %s\n", arr.c_str());
                continue;
            }

            // 判断为常数
            // 判断负数
            if(result.back().value == "-"){
                result.pop_back();
                result.push_back({TOKEN_TYPE_TABLE["const_int"], "-" + arr, "const_int"});
            }
            else{
                result.push_back({TOKEN_TYPE_TABLE["const_int"], arr, "const_int"});
            }
        }

        // 判断运算符或分隔符
        else if(IsOperator(ch) || IsSeparater(ch)){
            arr += ch;
            result.push_back({TOKEN_TYPE_TABLE[arr], arr, "separator"});
            continue;
        }

        else{
            arr += ch;
            fprintf(stderr, "unknown string: %s\n", arr.c_str());
        }
    }

    if(if_print){
        for(int i = 0; i < result.size(); i ++){
            string attr = to_string(result[i].index), v = result[i].value, type = result[i].type;
            printf("(%s\t%s\t%s)", attr.c_str(), v.c_str(), type.c_str());
            if (i % 3 == 0) putchar('\n');
            else putchar('\t');
        }
    }
    return result;
}
