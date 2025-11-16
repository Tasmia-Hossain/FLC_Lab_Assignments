#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <unordered_set>

using namespace std;

bool isSeparator(char c) {
    return c == ';' || c == ',' || c == '\'';
}

bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '>' || c == '<' || c == '!' || c == '=';
}

bool isParenthesis(char c) {
    return c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']';
}

bool isSeparatorStr(const string& s) {
    return s.length() == 1 && isSeparator(s[0]);
}

bool isKeyword(const string& str) {
    static unordered_set<string> keywords = {
        "int", "float", "char", "double", "bool", "if", "else", "for", "while", "do", "switch",
        "case", "break", "continue", "return", "void", "static", "struct", "class", "public",
        "private", "protected", "include", "define", "using", "namespace", "true", "false"
    };
    return keywords.find(str) != keywords.end();
}

bool isOperatorStr(const string& s) {
    if (s.length() == 1) return isOperator(s[0]);
    if (s.length() == 2) return isOperator(s[0]) && s[1] == '=';
    return false;
}

bool isParenthesisStr(const string& s) {
    return s.length() == 1 && isParenthesis(s[0]);
}

bool isValidNumberDFA(const string& str) {
    int len,i,state;
    i=0;
    if(isdigit(str[i])){
        state=1;
    }
    else if(str[i]=='.'){
        state=2;
    }
    else{
        state=0;
    }

    len=str.size();
    i++;

    if(state==1){
        for(;i<len;i++){
            if(isdigit(str[i])){
                state=1;
            }
            else if(str[i]=='.'){
                state=2;
                i++;
                break;
            }
            else{
                state=0;
                break;
            }
        }
    }

    if(state==2){
        if(isdigit(str[i])){
                state=3;
                i++;
        }
        else{
            state=0;
        }
    }

    if(state==3){
        for(;i<len;i++){
            if(isdigit(str[i])){
                state=3;
            }
            else{
                state=0;
                break;
            }
        }
    }
    return state;
}

bool isIdentifierDFA(const string& str) {
    int state=0;

    if(str.empty())
        return false;

    for(int i=0;i<str.length();i++)
    {
        char ch=str[i];

        switch(state)
        {
            case 0:
                if(isalpha(ch) || ch=='_')
                    state=1;
                else
                    return false;
                break;

                case 1:
                if(isalnum(ch) || ch=='_')
                    state=1;
                else
                    return false;
                break;
        }
    }
    return (state==1);
}

int main()
{
    cout << "Sample Input: input.c" << endl;
    ifstream fp("input.c");
    if (!fp.is_open()) {
        cout << "Cannot open file input.c" << endl;
        return 1;
    }

    string code((istreambuf_iterator<char>(fp)), istreambuf_iterator<char>());
    cout << code << endl << endl << endl;

    cout << "Sample Output: " << endl << endl;
    cout << "Step 1: " << endl;

    ofstream step1("step1.txt");
    ofstream step2("step2.txt");

    string formatted;
    for (size_t i = 0; i < code.size(); ++i) {
        char c = code[i];

        auto add_space_before = [&]() {
            if (!formatted.empty() && formatted.back() != ' ') {
                formatted += ' ';
            }
        };
        auto add_space_after = [&]() {
            if (i + 1 < code.size() && code[i + 1] != ' ' && code[i + 1] != '\n') {
                formatted += ' ';
            }
        };

        if (isSeparator(c)) {
            add_space_before();
            formatted += c;
            add_space_after();
        } else if (isOperator(c)) {
            add_space_before();
            formatted += c;
            if (i + 1 < code.size() && code[i + 1] == '=') {
                formatted += '=';
                ++i;
            }
            add_space_after();
        } else if (isParenthesis(c)) {
            add_space_before();
            formatted += c;
            add_space_after();
        } else if (isspace(c)) {
            if (!formatted.empty() && formatted.back() != ' ') {
                formatted += ' ';
            }
        } else {
            formatted += c;
        }
    }

    cout << formatted << "\n\n\nStep 2: " << endl;
    step1 << formatted << endl;

    stringstream ss(formatted);
    string token;
    while (ss >> token) {
        string output;
        if (isSeparatorStr(token)) output = "[sep " + token + "]";
        else if (isKeyword(token)) output = "[kw " + token + "]";
        else if (isIdentifierDFA(token)) output = "[id " + token + "]";
        else if (isOperatorStr(token)) output = "[op " + token + "]";
        else if (isParenthesisStr(token)) output = "[par " + token + "]";
        else if (isValidNumberDFA(token)) output = "[num " + token + "]";
        else output = "[unkn " + token + "]";

        cout << output;
        step2 << output;
    }

    cout << endl;
    step1.close();
    step2.close();
    return 0;
}
