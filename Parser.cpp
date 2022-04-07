//
// Created by 29962 on 2022/3/26.
//

#include "Parser.h"

string strip(string s) {
    if (!s.empty()) {
        s.erase(0, s.find_first_not_of(' '));
        s.erase(s.find_last_not_of(' ') + 1);
    }
    return s;
}

vector<string> split(const string & x, const string & y){
    // 使用y字符串分割x字符串
    vector<string> ans;
    ans.clear();
    string::size_type pos1 = 0, pos2 = x.find(y);
    while (string::npos != pos2){
        ans.push_back(x.substr(pos1, pos2 - pos1));
        pos1 = pos2 + y.size();
        pos2 = x.find(y, pos1);
    }
    if (pos1 != x.length())
        ans.push_back(x.substr(pos1));
    return ans;
}

bool add_firsts_step(
        const vector<Gram>& gram_list,   // 语法规则列表
        map<string, SymbolSet> &firsts,  // 需要进行添加的first集合
        const SymbolSet& non_term        // 非终结符列表
){
    bool is_add = false;
    for(const Gram& gram: gram_list){
        string l_exp = gram.first, r_exp = gram.second;

        // 添加空集合
        if(firsts.count(l_exp) == 0){
            firsts[l_exp] = SymbolSet();
        }

        // 如果是终结符，其first集合就是其本身
        if(non_term.count(l_exp) == 0){
            if(firsts[l_exp].count(l_exp) == 0){
                firsts[l_exp].insert(l_exp);
                is_add = true;
            }
            continue;
        }

        // 若X∈VN，且有产生式X→a…,则a∈First(X),特别地，若有X→ε，则ε∈First(X)
        string r_first = split(r_exp, " ")[0];
        if(non_term.count(r_first) == 0){
            if(firsts[l_exp].count(r_first) == 0){
                firsts[l_exp].insert(r_first);
                is_add = true;
            }
        }

            // 若X→Y…且有Y∈VN，则把First(Y)中的所有非ε元素加到First(X)中
        else{
            if(firsts.count(r_first) == 0) continue;
            for(const string& r_f: firsts[r_first]){
                if(r_f == EPSILON) continue;
                if(firsts[l_exp].count(r_f) == 0){
                    firsts[l_exp].insert(r_f);
                    is_add = true;
                }
            }
            /*  若X→Y1Y2…Yk是一个产生式，Y1…Yi-1都是非终结符，而且，对于任何j (1<=j<=i-1),
                都有ε∈First(Yj)，则把First(Yi)中的所有非ε元素加入First(X)中；
                特别地，若所有的First(Yj) 均含有ε (j=1,2,…k),则把ε加入First(X)中。*/
            // 求出第一个first集中不包含幺元的i
            vector<string> r_exp_list = split(r_exp, " ");
            int i = 0;
            for(; i < r_exp_list.size(); i ++){
                if(firsts[r_exp_list[i]].count(EPSILON) == 0) break;
            }

            // 若所有的First(Yj) 均含有ε (j=1,2,…k),则把ε加入First(X)中。
            if(i >= r_exp_list.size() && firsts[l_exp].count(EPSILON) == 0) {
                firsts[l_exp].insert(EPSILON);
                is_add = true;
            }

            // 把第前 i 个 first 中的非幺元加入当前 first
            for(int j = 1; j < i && j < r_exp_list.size(); j ++){
                r_first = r_exp_list[j];
                if(firsts.count(r_first) == 0) continue;
                for(const string& r_f: firsts[r_first]){
                    if(r_f == EPSILON) continue;
                    if(firsts[l_exp].count(r_f) == 0){
                        firsts[l_exp].insert(r_f);
                        is_add = true;
                    }
                }
            }
        }
    }
    return is_add;
}

bool is_derivation_null(
        const vector<Gram>& gram_list,          // 语法规则列表
        const string & non_term                 // 非终结符
){
    bool is_null = false;
    for(const Gram& item: gram_list){
        if(item.first != non_term) continue;
        if(item.second == EPSILON){
            is_null = true;
            break;
        }
    }
    return is_null;
}

bool is_null_in(const vector<string>& str_list){
    for(const auto& s: str_list){
        if(s == EPSILON)
            return true;
    }
    return false;
}

vector<string> get_first(
        map<string, SymbolSet> &firsts,     // 所有符号的firsts集合
        const string& r_exp                        // 获取右边表达式的first集合
){
    vector<string> ans;
    bool have_null = false;
    for(const string& r_e: split(r_exp, " ")){
        // 终结符
        if(firsts.count(r_e) == 0) {
            ans.push_back(r_e);
        }

        // 非终结符
        have_null = false;
        for(const auto& item: firsts[r_e]){
            if(item == EPSILON){
                have_null = true;
                continue;
            }
            ans.push_back(item);
        }
        if(!have_null) break;
    }
    if(have_null)
        ans.emplace_back(EPSILON);
    return ans;
}

bool add_follows_step(
        const vector< Gram >& gram_list,     // 语法规则列表
        map<string, SymbolSet> &firsts,  // 已经完成的firsts集合
        map<string, SymbolSet> &follows, // 需要进行添加的follows集合
        const SymbolSet& non_term          // 非终结符列表
){
    bool is_add = false;
    for(const Gram& gram: gram_list){
        string l_exp = gram.first, r_exp = gram.second;

        // 初始化空集合
        if(follows.count(l_exp) == 0){
            follows[l_exp] = SymbolSet();
        }

        // 若A→αBβ∈P,则把First(β)加入Follow(B)中，但ε除外；
        vector<string> r_exp_list = split(r_exp, " ");
        for(int i = 0, j = 1; j < r_exp_list.size(); i ++, j ++){
            string x = r_exp_list[i], y = r_exp_list[j];
            if(non_term.count(x) != 0){
                if(non_term.count(y) == 0){
                    if(follows[x].count(y) == 0){
                        follows[x].insert(y);
                        is_add = true;
                    }
                }
                else for(const string& s: firsts[y]){
                        if(s == EPSILON) continue;
                        if(follows[x].count(s) == 0){
                            follows[x].insert(s);
                            is_add = true;
                        }
                    }
            }
        }

        // 若A→αB∈P或A→αBβ∈P且β=>* ε,则把Follow(A)加入 Follow(B)中；
        for(int i = 0; i < r_exp_list.size(); i ++){
            if(non_term.count(r_exp_list[i]) == 0) continue;
            if(i + 1 >= r_exp_list.size() || is_derivation_null(gram_list, r_exp_list[i + 1])){
                string x = r_exp_list[i];
                for(const string& s: follows[l_exp]){
                    if(follows[x].count(s) == 0){
                        follows[x].insert(s);
                        is_add = true;
                    }
                }
            }
        }
    }
    return is_add;
}


map<string, map<string, Gram>> get_analysis_table(
        const vector< Gram >& gram_list,     // 语法规则列表
        map<string, SymbolSet> &firsts,  // 已经完成的firsts集合
        map<string, SymbolSet> &follows, // 已经完成的follows集合
        const SymbolSet& term,             // 非终结符列表
        const SymbolSet& non_term          // 非终结符列表
){
    map<string, map<string, pair<string, string>>> analysis_table;

    // 对于文法G的每个产生式A→α执行：
    for(const Gram& gram: gram_list){
        string l_exp = gram.first, r_exp = gram.second;
        string r_first= split(r_exp, " ")[0];
        if(term.count(r_first) != 0){
            if(r_first == EPSILON){
                for(const string & r_fl: follows[l_exp]){
                    if(analysis_table[l_exp].count(r_fl) == 0)
                        analysis_table[l_exp][r_fl] = gram;
                    else{
                        fprintf(stderr, "文法非ll(1)文法");
                    }
                }
            }
            else if(analysis_table[l_exp].count(r_first) == 0)
                analysis_table[l_exp][r_first] = gram;
            else fprintf(stderr, "文法非ll(1)文法");
        }
        else {
            for (const string &r_f: get_first(firsts, r_exp)) {
                //若ε∈First(α),则对于任何b∈Follow(A),置 M[A,b]为A→α；
                if (r_f == EPSILON) {
                    for (const string &r_fl: follows[l_exp]) {
                        if (analysis_table[l_exp].count(r_fl) == 0)
                            analysis_table[l_exp][r_fl] = gram;
                        else fprintf(stderr, "文法非ll(1)文法");
                    }
                }

                // 对每个终结符a∈First(α), 置M[A,a]为A→α；
                else if (analysis_table[l_exp].count(r_f) == 0)
                    analysis_table[l_exp][r_f] = gram;
                else fprintf(stderr, "文法非ll(1)文法");
            }
        }
    }
    return analysis_table;
}

void print_analysis_table(map<string, map<string, Gram>> analysis_table){
    // 打印表格的列名
    set<string> columns, rows;
    for(const auto& i: analysis_table) {
        rows.insert(i.first);
        for (const auto& j: i.second)
            columns.insert(j.first);
    }
    cout << "\t\t";
    for(const string& c: columns) cout << c << "\t\t";
    putchar('\n');

    // 一个单元一个单元地打印
    for(const string& r: rows){
        cout << r << "\t\t";
        for(const string& c: columns){
            if(analysis_table[r].count(c) == 0) printf("\t\t");
            else{
                pair<string, string> cell = analysis_table[r][c];
                string cell_str = cell.first + "->" + cell.second;
                cout << cell_str << "\t";
                if(cell_str.size() < 8) cout << '\t';
            }
        }
        putchar('\n');
    }
}

map<string, map<string, Gram>> gram_list2analysis_table(vector<Gram> gram_list, bool print_mid){
    // 获取所有的非终结符
    SymbolSet non_terms;
    for(const pair<string, string>& item : gram_list) {
        non_terms.insert(item.first);
    }
    if(print_mid){
        cout<<"非终结符有: ";
        for(const auto & non_term : non_terms) {
            cout<<non_term<<" ";
        }
        cout<<"\n";
    }

    // 获取所有的终结符
    SymbolSet terms;
    for(Gram & single_gram : gram_list) {
        for(string & symbol : split(single_gram.second, " ")) {
            if(non_terms.count(symbol) == 0) {
                terms.insert(symbol);
            }
        }
    }
    terms.insert(EPSILON);

    if(print_mid){
        cout<<"终结符有: ";
        for(const string& term : terms) {
            cout<<term<<" ";
        }
        cout<<"\n\n";
    }

    // 开始符号是第一个产生式的左侧的非终结符
    string start_sym = gram_list.begin()->first;

    // 扩展firsts集合：扩展first集合直至无法再次扩展
    map<string, SymbolSet> firsts;
    while(add_firsts_step(gram_list, firsts, non_terms)) continue;

    if(print_mid){
        cout<<"Firsts list: \n";
        for(auto & first : firsts) {
            cout<<first.first<<" : ";
            for(const auto & firsts_it : first.second) {
                cout<<firsts_it<<" ";
            }
            cout<<"\n";
        }
        cout<<"\n";
    }

    // 扩充follow集合 （对于开始符号S,置#于Follow(S)中）
    map<string, SymbolSet> follows;
    follows[start_sym] = SymbolSet();
    follows[start_sym].insert("#");
    while(add_follows_step(gram_list, firsts, follows, non_terms)) continue;

    if(print_mid){
        cout<<"Follows list: \n";
        for(auto & follow : follows) {
            cout << follow.first << " : ";
            for(const string& follows_it : follow.second) {
                cout << follows_it << " ";
            }
            cout << "\n";
        }
        cout << "\n";
    }

    auto result = get_analysis_table(gram_list, firsts, follows, terms, non_terms);
    if(print_mid) print_analysis_table(result);

    return result;
}

string token_node2sym(const token_node& x){
    if(x.type == "key_word" || x.type == "separator")
        return x.value;
    else if(x.type == "id" || x.type == "const_int" || x.type == "const_float" || x.type == "const_bool" || x.type == "const_string")
        return x.type;
    else if(x.index == -1)
        return "#";
    return "error";
}

ParserTree analyse(
        const string& start_sym,
        vector<token_node> input,
        map<string, map<string, Gram>> analysis_table
        ){

    // 初始状态栈底为 #E，输入串中加入#
    stack<pair<string, int>> sym_stack;
    ParserTree parser_tree;

    sym_stack.push({"#", -1});
    input.push_back((token_node){-1, "#"});

    parser_tree.add(-1, {start_sym});
    sym_stack.push({start_sym, parser_tree.size()});


    // 终止条件是输入串和符号栈都只有#
    while(true){
        // 错误条件判断和出口条件
        if(sym_stack.size() == 1 && input[0].index == -1){
            if(sym_stack.top().first == "#") return parser_tree;;
            fprintf(stderr, "输入串和语法规则不匹配。");
            return parser_tree;
        }
        if(sym_stack.top().first == "#"){
            if(input.size() == 1 && input[0].index == -1)
                return parser_tree;
            fprintf(stderr, "输入串和语法规则不匹配。");
            return parser_tree;
        }

        // 取符号栈和输入串的对应符号
        pair<string, int> x_f = sym_stack.top();
        token_node i_f = input.front();
        string x_s = x_f.first, x_i = token_node2sym(i_f);

        // 终结符
        if(analysis_table.count(x_s) == 0){
            // 匹配成功
            if(x_s == x_i){
                sym_stack.pop(); input.erase(input.begin());
                parser_tree.add(x_f.second, {i_f.value});
                continue;
            }
            else{
                fprintf(stderr, "输入串和语法规则不匹配。");
                return parser_tree;
            }
        }

        // 若X为非终结符，则查看分析表M[X, a]。若有关于X的产生式，则先令X出栈，再将该产生式右部倒序装入栈中
        if(analysis_table[x_s].count(x_i) != 0){
            Gram g = analysis_table[x_s][x_i];
            vector<string> r_exp_list = split(g.second, " ");
            sym_stack.pop();
            for(int i = (int)r_exp_list.size() - 1; i >= 0; i --){
                if(r_exp_list[i] == EPSILON) continue;
                parser_tree.add(x_f.second, {r_exp_list[i]});
                sym_stack.push({r_exp_list[i], parser_tree.size()});
            }
            continue;
        }

        // 无产生式则报错
        else{
            fprintf(stderr, "输入串和语法规则不匹配。");
            return parser_tree;
        }
    }
}