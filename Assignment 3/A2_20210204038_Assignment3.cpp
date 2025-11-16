#include <bits/stdc++.h>
using namespace std;

char a[10000], b[10000], temp[10000], c[1000][100];
int inde[10000];
int cur = 0;

struct data {
    char* name;
    char* idType;
    char* dataType;
    char* scope;
    char* value;
} v[100];

// 1) insert(): insert a name in the symbol table
void insert(int ind, char* _name, char* _id, char* _dat, char* _scope, char* _value) {
    v[ind].name = _name;
    v[ind].idType = _id;
    v[ind].dataType = _dat;
    v[ind].scope = _scope;
    v[ind].value = _value;
}

// 2) display(): display the whole symbol table
void display() {
    printf("\nSl. No.\t\tName\t\tId Type\t\tData Type\tScope\t\tValue\n");
    for (int i = 0; i < cur; i++) {
        printf("%d\t\t%s\t\t%s\t\t%s\t\t%s\t\t%s\n",
               i + 1, v[i].name, v[i].idType, v[i].dataType, v[i].scope, v[i].value);
    }
}

int search(char* _name, char* _id, char* _scope) {
    for (int i = 0; i < cur; i++) {
        if (strcmp(v[i].name, _name) == 0 &&
            strcmp(v[i].idType, _id) == 0 &&
            strcmp(v[i].scope, _scope) == 0) {
            return i;
        }
    }
    return -1;
}

char* modify(char* x) {
    if (strlen(x) == 0)
        return x;
    if (x[0] >= '0' && x[0] <= '9')
        return x;
    return (char*)""; // safe empty string
}

void update(int idx, char* val) {
    v[idx].value = val;
}

// 3) set_attribute(): associate an attribute (here, value) with an existing entry
void set_attribute(int idx, char* val) {
    update(idx, val);
}

// 4) free(): remove all entries and free storage of symbol table
void free() {
    for (int i = 0; i < cur; i++) {
        v[i].name = (char*)"";
        v[i].idType = (char*)"";
        v[i].dataType = (char*)"";
        v[i].scope = (char*)"";
        v[i].value = (char*)"";
    }
    cur = 0;
}

// 5) lookup(): search for a name and return pointer to its entry
data* lookup(char* _name, char* _id, char* _scope) {
    int idx = search(_name, _id, _scope);
    if (idx == -1) return nullptr;
    return &v[idx];
}

int main() {
    int i, k, j;
    FILE* fp = fopen("input.c", "r");
    i = 0;
    while (fscanf(fp, "%c", &a[i]) != EOF) {
        i++;
    }
    a[i] = '\0';
    fclose(fp);

    printf("Input: \n");
    puts(a);

    // Step 1: Keep only identifiers from lexemes to prepare token stream for symbol table
    k = 0;
    for (i = 0; i < (int)strlen(a); i++) {
        if (a[i] == '[') {
            b[k++] = a[i++];
            j = 0;
            while (a[i] != ' ' && a[i] != ']') {
                temp[j++] = a[i++];
            }
            temp[j] = '\0';
            if (strcmp(temp, "id") == 0) {
                b[k++] = 'i';
                b[k++] = 'd';
                b[k++] = ' ';
            }
            if (a[i] == ' ') i++;
            while (a[i] != ']') {
                b[k++] = a[i++];
            }
            b[k++] = a[i];
        }
    }
    b[k] = '\0';

    printf("\nStep 1: \n");
    puts(b);
    fp = fopen("Step_01.txt", "w");
    fputs(b, fp);
    fclose(fp);

    // Step 2: Generate symbol table with Name, Id Type, Data Type, Scope, Value
    k = 0;
    for (i = 0; i < (int)strlen(b); i++) {
        if (b[i] == '[') {
            j = 0;
            i++;
            while (b[i] != ']' && b[i] != ' ') {
                c[k][j++] = b[i++];
            }
            c[k][j] = '\0';
            k++;
            if (b[i] == ' ') {
                j = 0;
                i++;
                while (b[i] != ']') {
                    c[k][j++] = b[i++];
                }
                c[k][j] = '\0';
                k++;
            }
        }
    }

    printf("\nStep 2: \n");
    int it = 0;
    char* scope = (char*)"global";
    for (i = 0; i < k; i++) {
        if (strcmp(c[i], "id") == 0) {
            if (strcmp(c[i + 2], "(") == 0) { // function
                if (strcmp(c[i - 1], "int") == 0 ||
                    strcmp(c[i - 1], "double") == 0 ||
                    strcmp(c[i - 1], "float") == 0 ||
                    strcmp(c[i - 1], "char") == 0) {
                    inde[it++] = cur;
                    insert(cur++, c[i + 1], (char*)"func", c[i - 1], scope, (char*)"");
                    scope = c[i + 1]; // enter function scope
                    i += 2;
                } else {
                    int pq = search(c[i + 1], (char*)"func", (char*)"global");
                    if (pq != -1) inde[it++] = pq;
                    else printf("\nID %s is not declared in global scope\n", c[i + 1]);
                    i += 2;
                }
            } else if (strcmp(c[i + 2], "=") == 0) { // assignment
                if (strcmp(c[i - 1], "int") == 0 ||
                    strcmp(c[i - 1], "double") == 0 ||
                    strcmp(c[i - 1], "float") == 0 ||
                    strcmp(c[i - 1], "char") == 0) {
                    if (search(c[i + 1], (char*)"var", scope) == -1) {
                        inde[it++] = cur;
                        insert(cur++, c[i + 1], (char*)"var", c[i - 1], scope, modify(c[i + 3]));
                    } else {
                        printf("\nID %s already declared in %s scope\n", c[i + 1], scope);
                    }
                } else {
                    int pq = search(c[i + 1], (char*)"var", scope);
                    if (pq == -1) {
                        printf("\nID %s is not declared in %s scope\n", c[i + 1], scope);
                    } else {
                        set_attribute(pq, modify(c[i + 3])); // use required API
                        inde[it++] = pq;
                    }
                }
                i += 2;
            } else if (strcmp(c[i + 2], ";") == 0 ||
                       strcmp(c[i + 2], ",") == 0 ||
                       strcmp(c[i + 2], ")") == 0) { // declaration
                if (strcmp(c[i - 1], "int") == 0 ||
                    strcmp(c[i - 1], "double") == 0 ||
                    strcmp(c[i - 1], "float") == 0 ||
                    strcmp(c[i - 1], "char") == 0) {
                    if (search(c[i + 1], (char*)"var", scope) == -1) {
                        inde[it++] = cur;
                        insert(cur++, c[i + 1], (char*)"var", c[i - 1], scope, (char*)"");
                    } else {
                        printf("\nID %s already declared in %s scope\n", c[i + 1], scope);
                    }
                    i += 2;
                } else {
                    int pq = search(c[i + 1], (char*)"var", scope);
                    if (pq == -1) {
                        printf("\nID %s is not declared in %s scope\n", c[i + 1], scope);
                    } else {
                        inde[it++] = pq;
                    }
                    i += 2;
                }
            }
        } else if (strcmp(c[i], "}") == 0) {
            scope = (char*)"global"; // reset scope
        }
    }

    display();

    // Step 4: Produce modified token stream annotated with symbol table indices for syntax analysis
    printf("\nStep 4: \n");
    fp = fopen("Step_04.txt", "w");
    int it2 = 0;
    for (i = 0; i < (int)strlen(b); i++) {
        if (b[i] == '[') {
            printf("%c", b[i]);
            fputc(b[i], fp);
            i++;
            while (b[i] != ']' && b[i] != ' ') {
                printf("%c", b[i]);
                fputc(b[i], fp);
                i++;
            }
            if (b[i] == ' ') {
                printf("%c", b[i]);
                fputc(b[i], fp);
                i++;
                while (b[i] != ']') {
                    i++;
                }
                fputc(inde[it2] + 1 + 48, fp);
                printf("%d", inde[it2++] + 1);
            }
            printf("%c", b[i]);
            fputc(b[i], fp);
        } else {
            printf("%c", b[i]);
        }
    }
    fclose(fp);

    //data* entry = lookup((char*)"x1", (char*)"var", (char*)"global");
    //if (entry) printf("\nFound: %s in scope %s\n", entry->name, entry->scope);
    free(); // clear symbol table

    return 0;
}
