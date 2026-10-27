#include <bits/stdc++.h>
using namespace std;

struct Node {
    int id;
    char val;
    Node *left, *right;
    Node(int i, char v, Node* l = nullptr, Node* r = nullptr) 
        : id(i), val(v), left(l), right(r) {}
};

int prec(char c) {
    if (c == '*' || c == '/') return 2;
    if (c == '+' || c == '-') return 1;
    return 0;
}

string toPostfix(const string& infix) {
    string post = "";
    stack<char> st;
    for (char c : infix) {
        if (isalnum(c)) {
            post += c;
        } else if (c == '(') {
            st.push(c);
        } else if (c == ')') {
            while (!st.empty() && st.top() != '(') {
                post += st.top();
                st.pop();
            }
            if (!st.empty()) st.pop();
        } else {
            while (!st.empty() && prec(st.top()) >= prec(c)) {
                post += st.top();
                st.pop();
            }
            st.push(c);
        }
    }
    while (!st.empty()) {
        post += st.top();
        st.pop();
    }
    return post;
}

Node* buildAST(const string& post, vector<Node*>& nodes) {
    stack<Node*> st;
    int id = 1;
    for (char c : post) {
        if (isalnum(c)) {
            Node* n = new Node(id++, c);
            nodes.push_back(n);
            st.push(n);
        } else {
            Node* r = st.top(); st.pop();
            Node* l = st.top(); st.pop();
            Node* n = new Node(id++, c, l, r);
            nodes.push_back(n);
            st.push(n);
        }
    }
    return st.top();
}

Node* buildDAG(const string& post, vector<Node*>& dagNodes) {
    stack<Node*> st;
    int id = 1;
    for (char c : post) {
        Node *l = nullptr, *r = nullptr;
        if (!isalnum(c)) {
            r = st.top(); st.pop();
            l = st.top(); st.pop();
        }
        Node* existing = nullptr;
        for (Node* n : dagNodes) {
            if (n->val == c && n->left == l && n->right == r) {
                existing = n;
                break;
            }
        }
        if (existing) {
            st.push(existing);
        } else {
            Node* n = new Node(id++, c, l, r);
            dagNodes.push_back(n);
            st.push(n);
        }
    }
    return st.top();
}

void printTable(const vector<Node*>& nodes) {
    cout << left << setw(8) << "ID" << setw(10) << "SYMBOL" << setw(12) << "LEFT_ID" << "RIGHT_ID" << endl;
    cout << string(40, '-') << endl;
    for (Node* n : nodes) {
        string l = n->left ? to_string(n->left->id) + " (" + n->left->val + ")" : "-";
        string r = n->right ? to_string(n->right->id) + " (" + n->right->val + ")" : "-";
        cout << left << setw(8) << n->id << setw(10) << n->val << setw(12) << l << r << endl;
    }
}

void printTree(Node* root, int space = 0) {
    if (!root) return;
    space += 6;
    printTree(root->right, space);
    cout << endl << string(space - 6, ' ') << root->val << "[" << root->id << "]";
    printTree(root->left, space);
}

int main() {
    string input;
    cin >> input;

    string post = toPostfix(input);
    vector<Node*> astNodes, dagNodes;

    Node* astRoot = buildAST(post, astNodes);
    Node* dagRoot = buildDAG(post, dagNodes);

    cout << "\n[ ABSTRACT SYNTAX TREE (AST) - NODE TABLE ]\n";
    printTable(astNodes);
    cout << "\n[ AST STRUCTURE (Rotated 90 Deg Left) ]\n";
    printTree(astRoot);

    cout << "\n\n[ DIRECTED ACYCLIC GRAPH (DAG) - NODE TABLE ]\n";
    printTable(dagNodes);
    cout << "\n[ DAG STRUCTURE (Shared Node IDs) ]\n";
    printTree(dagRoot);
    cout << endl;

    return 0;
}