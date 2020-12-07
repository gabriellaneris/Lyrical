/*Code references and uses from Kattrina's Gator AVL project  */

/* AVL tree of artist nodes, each node has an artist name and a vector pair that consists of a song title and song lyrics  */
#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <iterator>
using namespace std;
class Tree {

    struct Node {//artist nodes
        string artist;
        vector<pair<string, string>> song;//title, lyrics
        Node* left;
        Node* right;
        int height;
        Node() : artist(nullptr), left(nullptr), right(nullptr), height(1) {}
        Node(string a, pair<string, string> p) {
            artist = a;
            song.push_back(p);
            left = right = nullptr;
            height = 1;
        }
    };
private:
    Node* root;
    unordered_map<string, int> totalFreqTree;
public:

    Tree() : root(nullptr) {}
    Node* getRoot();
    /* Parse data and create tree */
    Node* insertArtist(Node* node, string artist, string songTitle, string songLyrics);
    void parseData(string fileName, Tree& artistTree);
    Node* rightRotate(Node* node);
    Node* leftRotate(Node* node);
    int compareHeights(Node* node);
    int height(Node* node);
    int balanceFactor(Node* node);
    int biggerNum(int left, int right);

    /*Option 1 */
    unordered_map<string, int> makeFreqMapTree(Node* node);

    /*Option 1 & 2 */
    Node* artistSearch(Node* node, string key);

    /* Option 2 */
    void inorderTraversal(vector<pair<int, string>>& v, int& totalFrequency, Node* node, string word);
    void lyricsSearch(vector<pair<int, string>>& t, int& totalFrequency, Node* node, string word);

    /*Option 3 */
    int singleWordFreq(Node* node, string key);

    /*Option 4 */
    void inorderTraversal(Node* node);
    void setTotalFreqTree(Node* node);
    unordered_map<string, int>& getTotalFreqTree();

    /* Testing methods */
    void writeTree(Node* node);
    void inorder(Node* node, ofstream& testFile);
};
