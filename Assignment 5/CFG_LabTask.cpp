#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

using namespace std;

string str;
bool found;
int length, i;

void A();
void X();

// X -> bbX | bcX | E
void X() {
    if(i<length && str[i]=='b'){
        i++;
        if(i<length && (str[i]=='b' || str[i]=='c')){
            i++;
            X();
            return;
        }else{
            found=false;
            return;
        }
    }
    found=true;
}

// A -> aXd
void A() {
    if(i<length && str[i]=='a'){
        i++;
        X();
        if(!found)  return;

        if(i<length && str[i]=='d'){
            i++;
            found=true;
        }else{
            found=false;
        }
    }else{
        found=false;
    }
}

int main() {
    while(1){
        cout << "Enter a string(type 'exit' to quit):\n";
        cin>>str;
        if(str=="exit") break;

        i=0;
        length=str.size();
        found=true;

        A();

        if(found && i == length){
            cout << "Accepted" << endl;
        } else {
            cout << "Rejected" << endl;
        }
    }
    return 0;
}

