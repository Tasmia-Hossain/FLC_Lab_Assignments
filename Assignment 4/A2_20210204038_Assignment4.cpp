#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <algorithm>
#include <cctype>
#include <sstream>

using namespace std;

// C keywords
static const unordered_set<string> KW = {
    "auto","break","case","char","const","continue","default","do","double",
    "else","enum","extern","float","for","goto","if","inline","int","long",
    "register","restrict","return","short","signed","sizeof","static","struct",
    "switch","typedef","union","unsigned","void","volatile","while","_Bool",
    "_Complex","_Imaginary"
};

// Data type keywords
static const unordered_set<string> TYPE_KW = {
    "void","char","short","int","long","float","double","signed","unsigned",
    "_Bool","_Complex","_Imaginary"
};

// Operators & Punctuation
static const unordered_set<string> PUNCT = {
    "{","}","(",")",";","/",",",".","[","]","+","-","*","/","%","=","==","<",">",
    "<=",">=","!=","&&","||","!","&","|","^","~","<<",">>","?:",":","?","++","--",
    "+=","-=","*=","/=","%=","&=","|=","^=","<<=",">>=","->"
};

struct Token {
    int line;
    string text;
    bool isString=false;
    bool isNumber=false;
    bool isIdent=false;
};

// Remove comments
string remove_comments_preserve_lines(string src) {
    string out;
    out.reserve(src.size());

    enum Mode {
        NORMAL, SLASH, LINE_CMT, BLOCK_CMT,
        STRING, CHARLIT, ESC_STRING, ESC_CHAR
    } mode = NORMAL;

    for (size_t i = 0; i < src.size(); i++) {
        char c = src[i];
        switch (mode) {
            case NORMAL:
                if (c == '/') mode = SLASH;
                else if (c == '"') { out.push_back(c); mode = STRING; }
                else if (c == '\'') { out.push_back(c); mode = CHARLIT; }
                else out.push_back(c);
                break;

            case SLASH:
                if (c == '/') { mode = LINE_CMT; }
                else if (c == '*') { mode = BLOCK_CMT; }
                else { out.push_back('/'); out.push_back(c); mode = NORMAL; }
                break;

            case LINE_CMT:
                if (c == '\n') { out.push_back('\n'); mode = NORMAL; }      // removes comments but keeps lines
                break;

            case BLOCK_CMT:
                if (c == '\n') out.push_back('\n');    // removes comments but keeps lines
                if (c == '*' && i + 1 < src.size() && src[i + 1] == '/') {
                    i++; mode = NORMAL;
                }
                break;

            case STRING:
                out.push_back(c);
                if (c == '\\') mode = ESC_STRING;
                else if (c == '"') mode = NORMAL;
                break;

            case CHARLIT:
                out.push_back(c);
                if (c == '\\') mode = ESC_CHAR;
                else if (c == '\'') mode = NORMAL;
                break;

            case ESC_STRING:
                out.push_back(c);
                mode = STRING;
                break;

            case ESC_CHAR:
                out.push_back(c);
                mode = CHARLIT;
                break;
        }
    }
    return out;
}

// Tokenization
bool isIdentStart(char c){  // first char of identifier
    return isalpha(c) || c=='_';    // a-z || A-Z || _
}
bool isIdentChar(char c){   // any char in identifier
    return isalnum(c) || c=='_';    // a-z || A-Z || 0-9 || _
}
bool isNumberStart(char c){     // first char of number
    return isdigit(c);  // 0-9
}
bool isOpChar(char c) {     // checks operators and punctuations
    static const string ops = "{}()[];,+-*/%=&|^!<>?:.~";
    for (char o : ops) {
        if (c == o) return true;
    }
    return false;
}

vector<Token> tokenize_with_lines(string src){
    vector<Token> toks;
    int line=1;
    for(size_t i=0;i<src.size();){
        char c=src[i];
        if(c==' '||c=='\t'||c=='\r'){i++;continue;}     // If c is a space, tab, or carriage return, skip it
        if(c=='\n'){line++;i++;continue;}   // c is a newline \n, increment the line counter and skip it
        if(c=='"'){
            Token t{line,"",true,false,false};
            t.text.push_back(c);
            i++;
            bool esc=false;
            while(i<src.size()){
                char d=src[i++];
                t.text.push_back(d);
                if(d=='\n') line++;
                if(esc) esc=false;
                else if(d=='\\') esc=true;
                else if(d=='"') break;
            }
            toks.push_back(move(t));
            continue;
        }

        if(c=='\''){
            Token t{line,"",true,false,false};
            t.text.push_back(c); i++;
            bool esc=false;
            while(i<src.size()){
             char d=src[i++];
             t.text.push_back(d);
             if(d=='\n') line++;
             if(esc) esc=false;
             else if(d=='\\') esc=true;
             else if(d=='\'') break;
            }
            toks.push_back(move(t));
            continue;
        }
        if(isIdentStart(c)){
            size_t j=i+1;
            while(j<src.size() && isIdentChar(src[j])) j++;
            string w=src.substr(i,j-i);
            toks.push_back(Token{line,w,false,false,true});//string, number, identifier
            i=j; continue;
        }
        if(isNumberStart(c)){
            size_t j=i; bool dot=false,exp=false;
            while(j<src.size()){
                char d=src[j];
                if(isdigit(d)) j++;
                else if(d=='.'&&!dot){ dot=true;j++; }
                else if((d=='e'||d=='E')&&!exp){
                    exp=true;j++;
                if(j<src.size()&&(src[j]=='+'||src[j]=='-')) j++;
                }
                else break;
            }
            toks.push_back(Token{line,src.substr(i,j-i),false,true,false});
            i=j;
            continue;
        }
        if(isOpChar(c)){  //Detect operators or punctuation
            string two,one;
            if(i+1<src.size()) two=src.substr(i,2);     // match two-character operators first (==, !=, <=, ++ etc)
            one=src.substr(i,1);
            if(!two.empty() && PUNCT.count(two)){
                toks.push_back(Token{line,two});
                i+=2;
            }
            else if(PUNCT.count(one)){
                toks.push_back(Token{line,one}); i++;
            }
            else{
                toks.push_back(Token{line,one});i++;
            }
            continue;
        }
        toks.push_back(Token{line,string(1,c)}); i++;
    }
    return toks;
}

// Symbol Table
struct SymEntry {
    string name;      // identifier name
    string idType;    // var / func
    string dataType;  // int / float / double / void
    string scope;     // global / functionName
};
struct SymbolTable {
    vector<SymEntry> table;
    string currScope = "global";

    // exists -> check if variable exists in scope
    bool exists(string n, string sc) {
        for (auto &e : table) {
            if (e.name == n && e.scope == sc) return true;
        }
        if (sc != "global") {
            for (auto &e : table) {
                if (e.name == n && e.scope == "global") return true;
            }
        }
        return false;
    }

    bool existsExact(const string& n, const string& sc) {
        for (auto &e : table) {
            if (e.name == n && e.scope == sc) return true;
        }
        return false;
    }

    // insert -> add new identifier, checks duplicate declaration
    bool insert(const string& n, const string& iT, const string& dT,
                const string& sc, string &err) {
        if (existsExact(n, sc)) { //same scope e 2 ta identifier
            err = "Duplicate identifier declaration: " + n +
                  " (scope: " + sc + ")";
            return false;
        }
        table.push_back({n, iT, dT, sc});
        return true;
    }

    void enterScope(const string& sc) { currScope = sc; }
    void leaveScope() { currScope = "global"; }

    // display -> print table
    void display() const {
        cout << "\nSl\tName\tId TYPE\tData Type\tScope\n";
        cout << "-------------------------------------------------------\n";
        for (size_t i = 0; i < table.size(); i++) {
            cout << (i+1) << "\t|"
                 << table[i].name << "\t|"
                 << table[i].idType << "\t|"
                 << table[i].dataType << "\t\t|"
                 << table[i].scope << "\n";
        }
    }
};

// error handler
struct Err {
    int line;
    string msg;
};
struct Errs {
    vector<Err> v;
    void add(int l,const string&m){v.push_back({l,m});}     // add a new error
    void flush_sorted(){    // print all errors sorted by line
        sort(v.begin(),v.end(),[](auto&a,auto&b){
            return a.line<b.line||(a.line==b.line&&a.msg<b.msg);
        });
        for(auto&e:v) cout<<"Line "<<e.line<<": "<<e.msg<<"\n";
    }
    bool any()const{return !v.empty();}
};

// Helpers
bool isTypeSpec(const string&w){return TYPE_KW.count(w);}
bool isKeyword(const string&w){return KW.count(w);}
bool isIdentToken(const Token&t){return t.isIdent && !isKeyword(t.text);}// true if identifier
int parseType(const vector<Token>&toks,int i,string&dtype, Errs& errs){
    int start=i;
    vector<string>parts;//store each type specifier ("unsigned", "long", "int")
    while(i<(int)toks.size() && toks[i].line==toks[start].line
         && isTypeSpec(toks[i].text)){
        if(!parts.empty() && toks[i].text==parts.back())
         errs.add(toks[i].line,"Duplicate type specifier: "+toks[i].text);
        else parts.push_back(toks[i].text);
        i++;
    }
    dtype.clear();
    for(size_t k=0;k<parts.size();k++){
        if(k) dtype.push_back(' ');
        dtype+=parts[k];
    }
    return i;
}

// Keeps track of current scope (global or function)
struct ScopeStack{
    vector<string> st{"global"};
    string curr()const{return st.back();}
    void push(const string&s){ st.push_back(s); }
    void pop(){ if(st.size()>1) st.pop_back(); }
};

int main(){
    ifstream fin("input.c");
    if (!fin) {
    cout << "Error: input.txt not found!\n";
    return 1;
    }
    string src, line;
    while (getline(fin, line)) {// read the file line by line
    src += line + "\n"; // add the line and a newline character
    }
   fin.close();


   string filtered=remove_comments_preserve_lines(src);

   cout << "Step 1: Scanned & Filtered Source\n";
    {
      stringstream ss(filtered);
      string line;
      int ln = 1;
      while(getline(ss, line))
        cout << ln++ << " " << line << "\n";
     }

    vector<Token> toks=tokenize_with_lines(filtered);
    cout<<"\nStep 2: Tokenized Lexemes\n";
    { int curr=toks.empty()?1:toks.front().line; cout<<curr<<" ";
    for(auto&t:toks){
        if(t.line!=curr){
                curr=t.line; cout<<"\n"<<curr<<" ";
        }
    cout<<"["<<t.text<<"] ";} cout<<"\n";
    }

    SymbolTable sym; Errs errs; ScopeStack scopes; string pendingFN; bool fnPending=false;
    stack<pair<string,int>> braceStack; stack<int> ifStack;
    stack<int> forParenDepth;

    string dummy;
    sym.insert("printf","func","int","global",dummy);
    sym.insert("scanf","func","int","global",dummy);

    size_t i = 0;
    while (i < toks.size()) {
        if (toks[i].text == "{") {
            if (fnPending) { scopes.push(pendingFN); fnPending = false; }
            else { scopes.push(scopes.curr()); }
        } else if (toks[i].text == "}") {
            scopes.pop();
        }

        // For-loop parentheses tracking
        if (toks[i].text == "for") forParenDepth.push(1);
        if (toks[i].text == "(" && !forParenDepth.empty()) forParenDepth.top()++;
        if (toks[i].text == ")" && !forParenDepth.empty()) {
            forParenDepth.top()--;
            if (forParenDepth.top() == 0) forParenDepth.pop();
        }

        // Type declaration
        if (isTypeSpec(toks[i].text)) {
            string dtype;
            int j = parseType(toks, (int)i, dtype, errs);
            while (j < (int)toks.size()) {
                while (j < (int)toks.size() && toks[j].text == "*") j++;
                if (j >= (int)toks.size()) break;
                if (!isIdentToken(toks[j])) break;
                string ident = toks[j].text;
                int line = toks[j].line;
                j++;
                if (j < (int)toks.size() && toks[j].text == "(") { // function
                    int depth = 1;
                    j++;
                    vector<pair<string, string>> params;
                    string pd;
                    while (j < (int)toks.size() && depth > 0) {
                        if (toks[j].text == "(") {
                            depth++;
                            j++;
                            continue;
                        }
                        if (toks[j].text == ")") {
                            depth--;
                            j++;
                            if (depth == 0) break;
                            continue;
                        }
                        int tj = parseType(toks, j, pd, errs);
                        if (tj != j) {
                            j = tj;
                            while (j < (int)toks.size() && toks[j].text == "*") j++;
                            if (j < (int)toks.size() && isIdentToken(toks[j])) {
                                params.push_back({pd, toks[j].text});
                                j++;
                            }
                            if (j < (int)toks.size() && toks[j].text == ",") j++;
                            continue;
                        } else {
                            while (j < (int)toks.size() && toks[j].text != "," && toks[j].text != ")") j++;
                            if (j < (int)toks.size() && toks[j].text == ",") j++;
                        }
                    }
                    string e;
                    if (!sym.insert(ident, "func", dtype, "global", e)) errs.add(line, e);
                    pendingFN = ident;
                    fnPending = true;
                    for (auto& p : params) {
                        string e;
                        if (!sym.insert(p.second, "var", p.first, ident, e)) errs.add(line, e);
                    }
                } else { // variable
                    string e;
                    if (!sym.insert(ident, "var", dtype, scopes.curr(), e)) errs.add(line, e);
                    if (j < (int)toks.size() && toks[j].text == "=") {
                        j++;
                        while (j < (int)toks.size() && toks[j].text != "," && toks[j].text != ";") j++;
                    }
                    if (j < (int)toks.size() && toks[j].text == ",") {
                        j++;
                        continue;
                    }
                }

                break;
            }
            i = j;
            continue;
        }

        // Undeclared identifiers
        if (isIdentToken(toks[i]) && !sym.exists(toks[i].text, scopes.curr()))
            errs.add(toks[i].line, "Undeclared identifier: " + toks[i].text);

        // Extra semicolon detection for non-declaration statements
        if (toks[i].text == ";") {
            bool inForHeader = false;
            if (!forParenDepth.empty()) {
                size_t k = i;
                while (k > 0 && toks[k-1].text != "{" && toks[k-1].text != "}") {
                    if (toks[k-1].text == "for") {
                        inForHeader = true;
                        break;
                    }
                    k--;
                }
            }

            // Only flag consecutive semicolons as warnings if not in declaration or for-loop header
            if (!inForHeader) {
                size_t j = i + 1;
                while (j < toks.size() && toks[j].text == ";") {
                    // Comment this out to ignore harmless extra semicolons
                    // errs.add(toks[j].line, "Extra semicolon");
                    j++;
                }
            }
        }

        // if-else check
        if (toks[i].text == "if") ifStack.push(toks[i].line);
        else if (toks[i].text == "else") {
            if (ifStack.empty()) errs.add(toks[i].line, "Unmatched 'else' (no previous 'if')");
            else ifStack.pop();
        }

        /*
        if (toks[i].text == "if") ifStack.push(toks[i].line);
        else if (toks[i].text == "else") {
            if (i + 1 < toks.size() && toks[i+1].text == "if") {
                if (ifStack.empty()) errs.add(toks[i].line, "Unmatched 'else if'");
                // 'else if' continues chain, don't pop
            } else {
                if (ifStack.empty()) errs.add(toks[i].line, "Unmatched 'else'");
                else ifStack.pop(); // close the chain
            }
        }
        */

        // braces check
        if (toks[i].text == "{" || toks[i].text == "(") braceStack.push({toks[i].text, toks[i].line});
        else if (toks[i].text == "}" || toks[i].text == ")") {
            if (braceStack.empty()) errs.add(toks[i].line, "Unbalanced '" + toks[i].text + "'");
            else {
                auto top = braceStack.top();
                if ((top.first == "{" && toks[i].text == "}") || (top.first == "(" && toks[i].text == ")")) braceStack.pop();
                else errs.add(toks[i].line, "Mismatched parentheses/braces");
            }
        }

        if (toks[i].text == "<<"||toks[i].text == ">>") {
            if (i > 0 && i < toks.size() - 1) {
                if (isIdentToken(toks[i-1]) && isIdentToken(toks[i+1])) {
                    errs.add(toks[i].line, "Invalid condition ");
                }
            }
        }
        i++;
    }

    while (!braceStack.empty()) {
        auto p = braceStack.top();
        braceStack.pop();
        errs.add(p.second, "Unbalanced '" + p.first + "'");
    }

    cout << "\nStep 3: Symbol Table\n";
    sym.display();
    cout << "\n\n\nStep 4: Syntax Errors\n\n";
    if (!errs.any()) cout << "No error.\n";
    else errs.flush_sorted();
}
