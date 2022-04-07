/*
 * @ author 高世平
 * @ time 2021年10月9日
*/
#include <iostream>
#include <cstdio>
#include "Lexer.h"
#include "Parser.h"

using namespace std;


int main(int argc, char const *argv[]){
    auto lexer = Lexer();
    const char *file_path = argv[2];
    FILE *fpin = fopen(file_path,"r");
    if(fpin == nullptr){
        fprintf(stderr,"源程序代码不存在");
        exit(-1);
    }
    vector<token_node> input = Lexer::lexical_analyse(fpin, true);

    if(argc != 3) {
        cout << "Arguments should be <grammar file> <input string>\n";
        exit(-1);
    }

    // Parsing the grammar file
    fstream grammar_file;
    grammar_file.open(argv[1], ios::in);
    if(grammar_file.fail()) {
        cout<<"Error in opening grammar file\n";
        return 2;
    }

    cout<<"Grammar parsed from grammar file: \n";

    // 记录所有的语法规则
    vector<Gram> gram_list;
    int count = 0;
    while(!grammar_file.eof()) {
        string buffer;
        getline(grammar_file, buffer);

        // 分别记录表达式左端和表达法右端
        vector<string> split_res = split(buffer, "->");
        string lhs = split_res[0], rhs = split_res[1];
        for(const auto& rh: split(strip(rhs), "|")){
            gram_list.emplace_back(strip(lhs), strip(rh));
        }
        cout << count++ << ".  " << gram_list.back().first << " -> " << gram_list.back().second << "\n";
    }
    cout<<"\n";

    auto analysis_table = gram_list2analysis_table(gram_list, false);
    auto parser_tree = analyse("PROG", input, analysis_table);
    parser_tree.display();
    return 0;
}