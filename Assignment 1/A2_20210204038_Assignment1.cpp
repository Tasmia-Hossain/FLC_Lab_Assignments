#include <bits/stdc++.h>
using namespace std;

int main()
{
    FILE *p1, *p2;
    char c, next;
    int slc= 0, mlc= 0, whSpace= 0;

    p1 = fopen("Input.c", "r");
    p2 = fopen("output.txt", "w");

    if (!p1)
        cout << "File can't be opened!" << endl;
    else{
        cout << "Sample Input: Input.c"<<endl;
        while ((c = fgetc(p1)) != EOF) {
            cout << c;

            if (slc) {
                if (c == '\n') {
                    slc= 0;
                }
                continue;
            }

            if (mlc) {
                if (c == '*' && (next = fgetc(p1)) == '/') {
                    cout<<next;
                    mlc= 0;
                }
                continue;
            }

            if (c == '/') {
                next = fgetc(p1);
                cout<<next;

                if (next == '/') {
                    slc= 1;
                    continue;
                } else if (next == '*') {
                    mlc= 1;
                    continue;
                } else {
                    fputc(c, p2);
                    c = next;
                }
            }

            if (c == ' ' || c == '\t' || c == '\n') {
                if (!whSpace) {
                    fputc(' ', p2);
                    whSpace= 1;
                }
            } else {
                fputc(c, p2);
                whSpace= 0;
            }
        }
    }

    fclose(p1);
    fclose(p2);

    p2 = fopen("output.txt", "r");

    cout << "Sample Output: output.txt"<<endl;
    while ((c = fgetc(p2)) != EOF) {
        cout << c;
    }

    fclose(p2);
    return 0;
}
