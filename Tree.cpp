#include "Tree.h"


void eraseExtraChars(string& word) {
    word.erase(remove(word.begin(), word.end(), '?'), word.end());
    word.erase(remove(word.begin(), word.end(), '!'), word.end());
    word.erase(remove(word.begin(), word.end(), ','), word.end());
    word.erase(remove(word.begin(), word.end(), '.'), word.end());
    word.erase(remove(word.begin(), word.end(), '|'), word.end());
    word.erase(remove(word.begin(), word.end(), '"'), word.end());
    word.erase(remove(word.begin(), word.end(), ')'), word.end());
    word.erase(remove(word.begin(), word.end(), '('), word.end());
}
/* takes in an artist node and goes through the songs of that artist and then the lyrics of each song. incremements freq everytime
a word matches the key */
int Tree::singleWordFreq(Tree::Node* node, string key) {
    int freq = 0;
    int numSongs = node->song.size();
    for (int i = 0; i < numSongs; i++) {
        string word;
        stringstream lyrics(node->song[i].second);
        while (std::getline(lyrics, word, ' ')) {
            eraseExtraChars(word);
            if (word.size() > 0 && key == word)
                freq++;
        }
    }
    return freq;
}

//overloaded method
/* iterates through every artist node within the tree and counts the frequency of each word in every song  */
void Tree::inorderTraversal(Tree::Node* node) {
    //go thru artists lyrics, if they say the word more than 0 times then push back to v, do search maybe inorder
    if (node == nullptr) {
        return; // call method to go thru lyrics
    }
    inorderTraversal(node->left);
    setTotalFreqTree(node);
    inorderTraversal(node->right);
}
unordered_map<string, int>& Tree::getTotalFreqTree() {
    return totalFreqTree;
}
/*for each song of an artist node, go through its lyrics and add to frequency map */
void Tree::setTotalFreqTree(Node* node) {
    int numSongs = node->song.size();
    for (int i = 0; i < numSongs; i++) {
        string word;
        stringstream lyrics(node->song[i].second);
        while (std::getline(lyrics, word, ' ')) {
            eraseExtraChars(word);
            if (word.size() > 0)
                totalFreqTree[word]++;
        }
    }
}

/*enter word, print total frequency of that word in all database and 10 artists that use it the most */
// traverse tree for every artist and calls check lyrics methods
void Tree::inorderTraversal(vector<pair<int, string>>& t, int& totalFrequency, Tree::Node* node, string word) {
    //go thru artists lyrics, if they say the word more than 0 times then push back to v, do search maybe inorder
    if (node == nullptr)
        return;
    (inorderTraversal(t, totalFrequency, node->left, word));
    (lyricsSearch(t, totalFrequency, node, word));
    (inorderTraversal(t, totalFrequency, node->right, word));
}
/* counts the total frequency of a word in the entire database and the frequency of the same word by a single artist */
void Tree::lyricsSearch(vector<pair<int, string>>& t, int& totalFrequency, Tree::Node* node, string word) {
    int artistFrequency = 0;
    //iterate thru songs of an artist
    for (unsigned int i = 0; i < node->song.size(); i++) {
        string str;
        //iterate thru lyrics of song
        stringstream lyrics(node->song[i].second);
        while (getline(lyrics, str, ' ')) {
            eraseExtraChars(str);
            //if the word appears then add to frequency
            if (str == word) {
                totalFrequency++;
                artistFrequency++;
            }
        }
    }
    if (artistFrequency > 0)
        t.push_back(make_pair(artistFrequency, node->artist));
}

/* counts the frequency of words by a particular artist*/
unordered_map<string, int> Tree::makeFreqMapTree(Tree::Node* node) {
    unordered_map<string, int> freqMap;
    int numSongs = node->song.size();
    //for each song, go through its lyrics and add to frequency map
    for (int i = 0; i < numSongs; i++) {
        string word;
        stringstream lyrics(node->song[i].second);
        while (std::getline(lyrics, word, ' ')) {
            eraseExtraChars(word);
            if (word.size() > 0)
                freqMap[word]++;
        }
    }
    return freqMap;

}
/* checks if a particular artist is located within the tree */
Tree::Node* Tree::artistSearch(Node* node, string key) {
    if (node == nullptr || node->artist == key)
        return node;
    if ((key) < (node->artist)) {
        return artistSearch(node->left, key);
    }
    return artistSearch(node->right, key);
}

/* returns the root node*/
Tree::Node* Tree::getRoot() {
    return root;
}

void Tree::parseData(string fileName, Tree& artistTree) {

    string firstLine;
    string artist;
    string songTitle;
    string songLyrics;

    ifstream file(fileName);
    if (!file.is_open())
        throw "file not open";

    //skip first line
    getline(file, firstLine);

    while (file) {
        bool containedQuotes = false;
        getline(file, artist, '|');

        //fix faults in data set
        if (artist.find("https://www.azlyrics.com/") != string::npos) {
            artist = artist.substr(0, artist.find("https://www.azlyrics.com/"));
            containedQuotes = true;
        }
        //extra line for artist url
        getline(file, songTitle, '|');

        //if artist name contains delimiter |
        if (!containedQuotes && songTitle.substr(0, 25) != "https://www.azlyrics.com/") {
            artist += "|" + songTitle;
            //extra line again for actual artist url
            getline(file, songTitle, '|');
        }

        if (!containedQuotes)
            getline(file, songTitle, '|');


        //extra line for song url
        getline(file, songLyrics, '|');
        getline(file, songLyrics, '\n');

        //fix faults in dataset
        if (songTitle.find("\"\",\"\"") != string::npos) {
            songTitle = songTitle.substr(0, songTitle.find("\"\",\"\""));
        }

        //remove extra quotes
        artist.erase(remove(artist.begin(), artist.end(), '"'), artist.end());
        songTitle.erase(remove(songTitle.begin(), songTitle.end(), '"'), songTitle.end());

        //insert artist method
        artistTree.root = artistTree.insertArtist(artistTree.root, artist, songTitle, songLyrics);


    }
}
/* adds artist to tree and balances the tree */
Tree::Node* Tree::insertArtist(Node* node, string key, string songTitle, string songLyrics) {// key = current artist name

    if (node != nullptr && key == node->artist) {
        node->song.push_back(make_pair(songTitle, songLyrics));
    }
    if (node == nullptr)
        return new Node(key, make_pair(songTitle, songLyrics));
    if (key > node->artist)
        node->right = insertArtist(node->right, key, songTitle, songLyrics);
    else if (key < node->artist)
        node->left = insertArtist(node->left, key, songTitle, songLyrics);
    else {
        return node;
    }
    node->height = 1 + compareHeights(node);
    int balance = balanceFactor(node);
    if (balance > 1) {
        if ((key) < (node->left->artist)) {//left left case
            return rightRotate(node);
        }
        else if ((key) > (node->left->artist)) {
            node->left = leftRotate(node->left);//left right case
            return rightRotate(node);
        }
    }
    if (balance < -1) {
        if ((key) > (node->right->artist)) {//right right case
            return leftRotate(node);
        }
        else if ((key) < (node->right->artist)) {//right left case
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
    }
    return node;
}

Tree::Node* Tree::rightRotate(Tree::Node* node) {
    Node* newParent = node->left;
    Node* grandchild = newParent->right;
    newParent->right = node;
    node->left = grandchild;
    node->height = compareHeights(node) + 1;
    newParent->height = compareHeights(newParent) + 1;
    return newParent;
}
Tree::Node* Tree::leftRotate(Tree::Node* node) {
    Node* newParent = node->right;
    Node* grandchild = newParent->left;
    newParent->left = node;
    node->right = grandchild;
    node->height = compareHeights(node) + 1;
    newParent->height = compareHeights(newParent) + 1;
    return newParent;
}
int Tree::height(Tree::Node* node) {
    if (node == nullptr)
        return 0;
    else
        return node->height;
}
int Tree::compareHeights(Tree::Node* node) {
    int left, right = 0;
    if (node == nullptr)
        return 0;
    else {//find the longest path
        left = height(node->left);
        right = height(node->right);
    }
    return biggerNum(left, right);
}
int Tree::balanceFactor(Tree::Node* node) {
    int left, right = 0;
    if (node == nullptr)
        return 0;
    left = height(node->left);
    right = height(node->right);
    return left - right;
}
int Tree::biggerNum(int left, int right) {
    if (left > right)
        return left;
    return right;
}

/*testing methods */
void Tree::writeTree(Node* node) {// writes file Inorder
    ofstream testFile;
    testFile.open("tree_test_file.txt");
    inorder(node, testFile);
    testFile.close();
}

void Tree::inorder(Node* node, ofstream& testFile) {
    if (node == nullptr)
        return;
    inorder(node->left, testFile);
    testFile << node->artist << " ";
    for (unsigned int i = 0; i < node->song.size(); i++) {
        testFile << i + 1 << ". " << node->song[i].first << " ";
    }
    testFile << '\n';
    testFile << '\n';
    inorder(node->right, testFile);
}