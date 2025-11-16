#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

using namespace std;

string str;
bool found;
int length, i;

void Exp();
void Term();
void Factor();

// <Factor> -> ( <Exp> ) | ID | NUM
void Factor() {
    if(i<length && str[i]=='('){
        i++;
        Exp();
        if(found && i<length && str[i]==')'){
            i++;
            found=true;
        }else{
            found=false;
        }
    }
    else if(i<length && str[i]>='a' && str[i]<='e'){
        i++;
        found=true;
    }
    else if(i<length && str[i]>='0' && str[i]<='9'){
        i++;
        found=true;
    }
    else{
        found=false;;
    }
}

// <Term> -> <Factor> * <Factor> | <Factor> / <Factor> | <Factor>
void Term() {
    Factor();
    if(!found)  return;

    if(i<length && (str[i]=='*' || str[i]=='/')){
        i++;
        Factor();
        if(!found)  return;
    }
    found=true;
}

// <Exp> -> <Term> + <Term> | <Term> - <Term> | <Term>
void Exp() {
    Term();
    if(!found)  return;

    if(i<length && (str[i]=='+' || str[i]=='-')){
        i++;
        Term();
        if(!found)  return;
    }
    found=true;
}



int main() {
    while(1){
        cout << "Enter a string(type 'exit' to quit):\n";
        cin>>str;
        if(str=="exit") break;

        i=0;
        length=str.size();
        found=true;

        Exp();

        if(found && i == length){
            cout << "Accepted" << endl;
        } else {
            cout << "Rejected" << endl;
        }
    }
    return 0;
}
