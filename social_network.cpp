#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <algorithm>

using namespace std;

struct Post {
    int postId;
    string content;
    int timestamp;
};

struct AVLNode {
    Post post;
    AVLNode* left;
    AVLNode* right;
    int height;
    
    AVLNode(const Post& p) : post(p), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
private:
    AVLNode* root;

    int height(AVLNode* N) const {
        return N ? N->height : 0;
    }

    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        
        x->right = y;
        y->left = T2;
        
        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;
        
        return x;
    }

    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        
        y->left = x;
        x->right = T2;
        
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;
        
        return y;
    }

    int getBalance(AVLNode* N) const {
        return N ? height(N->left) - height(N->right) : 0;
    }

    AVLNode* insertNode(AVLNode* node, const Post& post) {
        if (!node) return new AVLNode(post);

        if (post.postId < node->post.postId)
            node->left = insertNode(node->left, post);
        else if (post.postId > node->post.postId)
            node->right = insertNode(node->right, post);
        else {
            node->post.content = post.content;
            node->post.timestamp = post.timestamp;
            return node;
        }

        node->height = 1 + max(height(node->left), height(node->right));

        int balance = getBalance(node);

        if (balance > 1 && post.postId < node->left->post.postId)
            return rightRotate(node);

        if (balance < -1 && post.postId > node->right->post.postId)
            return leftRotate(node);

        if (balance > 1 && post.postId > node->left->post.postId) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        if (balance < -1 && post.postId < node->right->post.postId) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    AVLNode* searchNode(AVLNode* node, int postId) const {
        if (!node || node->post.postId == postId)
            return node;
        if (postId < node->post.postId)
            return searchNode(node->left, postId);
        return searchNode(node->right, postId);
    }

    void inOrder(AVLNode* rootNode, vector<Post>& result) const {
        if (rootNode) {
            inOrder(rootNode->left, result);
            result.push_back(rootNode->post);
            inOrder(rootNode->right, result);
        }
    }

    void destroyTree(AVLNode* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    AVLTree() : root(nullptr) {}
    
    ~AVLTree() {
        destroyTree(root);
    }

    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    AVLTree(AVLTree&& other) noexcept : root(other.root) {
        other.root = nullptr;
    }

    AVLTree& operator=(AVLTree&& other) noexcept {
        if (this != &other) {
            destroyTree(root);
            root = other.root;
            other.root = nullptr;
        }
        return *this;
    }

    void insert(const Post& post) {
        root = insertNode(root, post);
    }

    bool getPost(int postId, Post& result) const {
        AVLNode* res = searchNode(root, postId);
        if (res) {
            result = res->post;
            return true;
        }
        return false;
    }

    vector<Post> getAllPosts() const {
        vector<Post> result;
        inOrder(root, result);
        return result;
    }
};

struct User {
    int userId;
    string name;
    AVLTree posts;
    
    User(int id, const string& n) : userId(id), name(n) {}
};

class SocialNetwork {
private:
    unordered_map<int, User*> users;
    unordered_map<int, vector<int>> adjList;

public:
    ~SocialNetwork() {
        for (auto& pair : users) {
            delete pair.second;
        }
    }

    void addUser(int id, const string& name) {
        if (users.find(id) == users.end()) {
            users[id] = new User(id, name);
        }
    }

    void addFriendship(int id1, int id2) {
        adjList[id1].push_back(id2);
        adjList[id2].push_back(id1);
    }

    void addPost(int userId, int postId, const string& content, int timestamp) {
        auto it = users.find(userId);
        if (it != users.end()) {
            it->second->posts.insert({postId, content, timestamp});
        }
    }

    bool fetchPost(int userId, int postId, Post& outPost) {
        auto it = users.find(userId);
        if (it != users.end()) {
            return it->second->posts.getPost(postId, outPost);
        }
        return false;
    }

    int getDegreesOfSeparation(int src, int target) {
        if (src == target) return 0;
        if (adjList.find(src) == adjList.end() || adjList.find(target) == adjList.end()) return -1;

        unordered_map<int, int> distance;
        queue<int> q;

        q.push(src);
        distance[src] = 0;

        while (!q.empty()) {
            int curr = q.front();
            q.pop();

            if (curr == target) return distance[curr];

            for (int neighbor : adjList[curr]) {
                if (distance.find(neighbor) == distance.end()) {
                    distance[neighbor] = distance[curr] + 1;
                    q.push(neighbor);
                }
            }
        }
        return -1; 
    }

    vector<pair<int, int>> recommendFriends(int userId, int maxDegree = 2) {
        vector<pair<int, int>> recommendations;
        if (adjList.find(userId) == adjList.end()) return recommendations;

        unordered_map<int, int> distance;
        unordered_map<int, int> mutualCount;
        queue<int> q;

        q.push(userId);
        distance[userId] = 0;

        while (!q.empty()) {
            int curr = q.front();
            q.pop();

            if (distance[curr] >= maxDegree) continue;

            for (int neighbor : adjList[curr]) {
                if (distance.find(neighbor) == distance.end()) {
                    distance[neighbor] = distance[curr] + 1;
                    q.push(neighbor);
                }
            }
        }

        for (int f : adjList[userId]) {
            for (int fof : adjList[f]) {
                if (fof != userId && distance.find(fof) != distance.end() && distance[fof] == 2) {
                    mutualCount[fof]++;
                }
            }
        }

        for (const auto& pair : mutualCount) {
            recommendations.push_back(pair);
        }

        sort(recommendations.begin(), recommendations.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
            return a.second > b.second; 
        });

        return recommendations;
    }
};

int main() {
    SocialNetwork sn;
    sn.addUser(1, "Alice");
    sn.addUser(2, "Bob");
    sn.addUser(3, "Charlie");
    sn.addUser(4, "David");
    sn.addUser(5, "Eve");

    sn.addFriendship(1, 2);
    sn.addFriendship(2, 3);
    sn.addFriendship(3, 4);
    sn.addFriendship(4, 5);

    sn.addPost(1, 105, "Hello World!", 1000);
    sn.addPost(1, 101, "Data Structures Rock", 1005);
    sn.addPost(1, 108, "AVL Trees are Balanced", 1010);

    Post p;
    if (sn.fetchPost(1, 101, p)) {
        cout << "Post Retrieved (O(log n)): [" << p.postId << "] " << p.content << "\n";
    }

    cout << "Degrees of separation between Alice (1) and David (4): " 
         << sn.getDegreesOfSeparation(1, 4) << " hops\n";
    cout << "Degrees of separation between Alice (1) and Eve (5): " 
         << sn.getDegreesOfSeparation(1, 5) << " hops\n";

    vector<pair<int, int>> recs = sn.recommendFriends(1, 2);
    cout << "Recommendations for Alice (UserId, MutualFriends): ";
    for (const auto& r : recs) {
        cout << "(" << r.first << ", " << r.second << ") ";
    }
    cout << "\n";

    return 0;
}