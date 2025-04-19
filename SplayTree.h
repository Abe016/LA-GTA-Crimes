#ifndef SPLAYTREE_H
#define SPLAYTREE_H
#include <iostream>
#include <stack>

using namespace std;

template <typename K, typename V>
class SplayTree {
private:
    struct Node {
        K key;
        V value;
        Node* left;
        Node* right;
        Node(K k, V v) {
            key = k;
            value = v;
            left = nullptr;
            right = nullptr;
        }
    };
    Node* root;

    // bst insert
    Node* bstInsert(Node* node, K key, V value) {
        if (!node) {
            return new Node(key, value);
        }
        if (key < node->key) {
            node->left  = bstInsert(node->left,  key, value);
        } else {
            node->right = bstInsert(node->right, key, value);
        }
        return node;
    }

    Node *rightRotate(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        y->right = x;
        return y;
    }
    Node *leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        y->left = x;
        return y;
    }
    Node* splay(Node* root, K key) {
        if (root == nullptr || root->key == key) {
            return root;
        }
        if (key < root->key) {
            if (root->left == nullptr) {
                return root;
            }
            if (key < root->left->key) {
                root->left->left = splay(root->left->left, key);
                root = rightRotate(root);
            } else if (key > root->left->key) {
                root->left->right = splay(root->left->right, key);
                if (root->left->right != nullptr) {
                    root->left = leftRotate(root->left);
                }
            }
            if (root->left == nullptr) {
                return root;
            } else {
                return rightRotate(root);
            }
        } else {
            if (root->right == nullptr) {
                return root;
            }
            if (key > root->right->key) {
                root->right->right = splay(root->right->right, key);
                root = leftRotate(root);
            } else if (key < root->right->key) {
                root->right->left = splay(root->right->left, key);
                if (root->right->left != nullptr) {
                    root->right = rightRotate(root->right);
                }
            }
            if (root->right == nullptr) {
                return root;
            } else {
                return leftRotate(root);
            }
        }
    }

public:
    SplayTree() {
        root = nullptr;
    }

    // initial build for balanced tree
    void rawInsert(K key, V value) {
        root = bstInsert(root, key, value);
    }

    void insert(K key, V value) {
        if (root == nullptr) {
            root = new Node(key, value);
            return;
        }
        root = splay(root, key);
        if (root->key == key) {
            return;
        }
        Node* newNode = new Node(key, value);
        if (key < root->key) {
            newNode->right = root;
            newNode->left = root->left;
            root->left = nullptr;
        } else {
            newNode->left = root;
            newNode->right = root->right;
            root->right = nullptr;
        }
        root = newNode;
    }

    // finds key and splays the node to the root
    V* find(K key)
    {
        if (root == nullptr) {
            return nullptr;
        }

        // move accessed node to root
        root = splay(root, key);

        // if found, pointer to stored value
        if (root->key == key) {
            return &root->value;
        } else {
            return nullptr;
        }
    }

    // iterative inorder traversal
    template <typename Func>
    void forEach(Func f)
    {
        stack<Node*> stack;
        Node* curr = root;

        while (!stack.empty() || curr)
        {
            // go as left as possible
            while (curr)
            {
                stack.push(curr);
                curr = curr->left;
            }
            // visit
            curr = stack.top(); stack.pop();
            f(curr->key, curr->value);
            // then right subtree
            curr = curr->right;
        }
    }
};

#endif //SPLAYTREE_H
