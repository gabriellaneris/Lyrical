
//syntax for function timer developed from https://www.geeksforgeeks.org/measure-execution-time-function-cpp/ 

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <set>
#include <chrono>
#include "Tree.h"
using namespace std;
using namespace std::chrono;

//takes in data from files
void parseData(string fileName, unordered_map<string, vector<pair<string, string>>>& artistLibrary);

//makes frequency map for a single artist's lyrics
unordered_map<string, int> makeFreqMap(string artist, unordered_map<string, vector<pair<string, string>>>& artistLibrary);

//makes frequency map for all artists in database
void makeTotalFreqMap(unordered_map<string, vector<pair<string, string>>>& artistLibrary, unordered_map<string, int>& totalFreq);

//gets top words from a frequency map
bool topWords(unordered_map<string, int>& freq, string input);
//helper function for topWords
string wordsInput();

//check if artist exists in database
bool artistExists(string& artist, unordered_map<string, vector<pair<string, string>>>& artistLibrary);

//gets rid of non-alphabetical characters in words
void eraseExtraChars(string& word);
//censors curse words
string censor(string word);

//sorts map into a vector of pairs with frequency first and artist name second
vector<pair<int, string>> sortMap(unordered_map<string, int>& freq);

//testing method for printing out the map
//void printMap(unordered_map<string, vector<pair<string, string>>>& artistLibrary);
//testing method for writing map to a file
//void writeMap(unordered_map<string, vector<pair<string, string>>>& artistLibrary);

int main() {
    cout << "Welcome to Lyrical!" << endl;
    cout << "Loading..." << endl;

    //create tree 
    Tree artistTree;

    //create map
    unordered_map<string, vector<pair<string, string>>> artistLibrary;
    unordered_map<string, int> totalFreq;

    string word;
    string artist = "";

    /*Load all files */
    char letters = 'a';
    parseData("lyrics_19.txt", artistLibrary);
    artistTree.parseData("lyrics_19.txt", artistTree);
    for (int i = 0; i < 26; i++) {
          string fileName = "lyrics_.txt";
          fileName.insert(7, 1, letters);
          parseData(fileName, artistLibrary);
          artistTree.parseData(fileName, artistTree);
          letters++;
      } 

      /* calls to testing methods */
      //printMap(artistLibrary);
      //writeMap(artistLibrary); //uncomment this to create + write all output to a new file
      //artistTree.writeTree(artistTree.getRoot());//uncomment this to create + write all output to a new file

    string option = "0";

    while (stoi(option) != 5) {
        cout << "What would you like to do?" << endl;
        cout << "1. Search for an artist" << endl;
        cout << "2. Search for a word" << endl;
        cout << "3. Search for a word within a specific artist's songs" << endl;
        cout << "4. See most popular words in all songs" << endl;
        cout << "5. Exit the program" << endl;

        getline(cin, option, '\n');
        try {
            stoi(option);
        }
        catch (invalid_argument& err) {
            cout << "Invalid input.";
            option = "0";
        }
        switch (stoi(option)) {
            /*Takes in the name of an artist and returns the most frequently used words by that artist */
        case 1: {
            cout << "Enter the name of the artist: ";
            getline(cin, artist, '\n');
            if (artistExists(artist, artistLibrary)) {
                cout << endl << "Unordered Map: " << endl;
                string input = wordsInput();
                //start time
                auto mapStart = high_resolution_clock::now();

                //makes frequency map for artist lyrics
                unordered_map<string, int> freqMap = makeFreqMap(artist, artistLibrary);
                if (topWords(freqMap, input)) {
                    // stop time, calculate, convert to microseconds
                    auto mapStop = high_resolution_clock::now();
                    auto mapDuration = duration_cast<microseconds>(mapStop - mapStart);
                    cout << endl << "Time taken for Unordered Map: " << mapDuration.count() / 1000000.0 << " seconds." << endl;
                }
            }
            else
                cout << "Artist not in map database." << endl << endl;

            auto artistNode = artistTree.artistSearch(artistTree.getRoot(), artist);
            if (artistNode != nullptr) {
                cout << endl << " AVL Tree: " << endl;
                string input = wordsInput();
                //start time
                auto treeStart = high_resolution_clock::now();

                // Makes FreqMap from artist node
                auto freqMapTree = artistTree.makeFreqMapTree(artistNode);

                if (topWords(freqMapTree, input)) {
                    // stop time
                    auto treeStop = high_resolution_clock::now();
                    auto treeDuration = duration_cast<microseconds>(treeStop - treeStart);
                    cout << endl << "Time taken for AVL Tree: " << treeDuration.count() / 1000000.0 << " seconds." << endl << endl;
                }
            }
            else
                cout << "Artist not in tree database." << endl << endl;
            break;
        }
              /*takes in a word then prints the total frequency of that word in database and
              the 10 artists that use it the most */
        case 2: {
            cout << "Enter the word you would like to search for: ";
            getline(cin, word, '\n');
            //start time
            auto mapStart = high_resolution_clock::now();
            vector<pair<int, string>> v;
            int totalFrequency = 0;
            int artistFrequency = 0;
            //iterates thru all artists
            for (auto iter = artistLibrary.begin(); iter != artistLibrary.end(); iter++) {
                //iterates thru lyrics of artist
                for (unsigned int i = 0; i < iter->second.size(); i++) {
                    string str;
                    stringstream lyrics(iter->second[i].second);
                    while (getline(lyrics, str, ' ')) {
                        eraseExtraChars(str);
                        //if the word appears then add to frequency
                        if (str == word) {
                            totalFrequency++;
                            artistFrequency++;
                        }
                    }
                }
                //if the artist uses that word at least once, add to vector
                if (artistFrequency > 0)
                    v.push_back(make_pair(artistFrequency, iter->first));
                //reset artistFrequency for next artist
                artistFrequency = 0;
            }

            if (totalFrequency == 0)
                cout << "Word does not appear in Map database." << endl;

            else {
                cout << endl << "Unordered Map: " << endl;
                string censoredWord = censor(word); // new variable because tree implementation also needs to use word for searching
                cout << censoredWord << " appears " << totalFrequency;
                if (totalFrequency == 1)
                    cout << " time." << endl << endl;
                else
                    cout << " times." << endl << endl;
                cout << "Artists who say \"" << censoredWord << "\" the most: " << endl;
                sort(v.begin(), v.end());
                sort(v.begin(), v.end());
                //print top artists who use the word, top 10 if there are more than 10
                for (int i = v.size() - 1; i >= 0; i--) {
                    cout << v[i].second << ": " << v[i].first;
                    if (v[i].first == 1)
                        cout << " time." << endl;
                    else
                        cout << " times." << endl;

                    if (v.size() > 10) {
                        if (i == v.size() - 10)
                            break;
                    }
                }

                // stop time
                auto mapStop = high_resolution_clock::now();
                auto mapDuration = duration_cast<microseconds>(mapStop - mapStart);
                cout << "Time taken for Unordered Map: " << mapDuration.count() / 1000000.0 << " seconds." << endl;
            }

            //start time
            auto treeStart = high_resolution_clock::now();
            vector<pair<int, string>> t;
            int totalFrequencyTree = 0;
            artistTree.inorderTraversal(t, totalFrequencyTree, artistTree.getRoot(), word);
            if (totalFrequencyTree == 0)
                cout << "Word does not appear in Tree database." << endl;
            else {
                cout << endl << "AVL Tree: " << endl;
                string censoredWord = censor(word);
                cout << censoredWord << " appears " << totalFrequencyTree;
                if (totalFrequencyTree == 1)
                    cout << " time." << endl << endl;
                else
                    cout << " times." << endl << endl;

                cout << "Artists who say \"" << censoredWord << "\" the most: " << endl;
                sort(t.begin(), t.end());
                for (int i = t.size() - 1; i >= 0; i--) {
                    cout << t[i].second << ": " << t[i].first;
                    if (t[i].first == 1)
                        cout << " time." << endl;
                    else
                        cout << " times." << endl;
                    if (t.size() > 10) {
                        if (i == t.size() - 10)
                            break;
                    }
                }
                // stop time
                auto treeStop = high_resolution_clock::now();
                auto treeDuration = duration_cast<microseconds>(treeStop - treeStart);
                cout << "Time taken for AVL Tree: " << treeDuration.count() / 1000000.0 << " seconds." << endl << endl;
            }
            break;
        }
        case 3: {
            cout << "Enter the artist you wish to analyze: ";
            string artist;
            getline(cin, artist, '\n');

            if (artistExists(artist, artistLibrary)) {
                cout << "Enter the word you would like to search for within their library: ";
                getline(cin, word, '\n');
                int totalFrequency = 0;

                //start time
                auto mapStart = high_resolution_clock::now();
                auto iter = artistLibrary.find(artist);
                //iterates thru lyrics of artist
                for (unsigned int i = 0; i < iter->second.size(); i++) {
                    string str;
                    stringstream lyrics(iter->second[i].second);
                    while (getline(lyrics, str, ' ')) {
                        eraseExtraChars(str);
                        //if the word appears then add to frequency
                        if (str == word) {
                            totalFrequency++;
                        }
                    }
                }

                cout << endl << "Unordered Map: " << endl;
                if (totalFrequency == 0)
                    cout << "Word does not appear in lyrics of " << artist << endl;
                else {
                    string censoredWord = censor(word); // new variable because tree implementation also needs to use word for searching
                    cout << censoredWord << " appears " << totalFrequency;
                    if (totalFrequency == 1)
                        cout << " time." << endl;
                    else
                        cout << " times." << endl;

                    // stop time, calculate, convert to microseconds
                    auto mapStop = high_resolution_clock::now();
                    auto mapDuration = duration_cast<microseconds>(mapStop - mapStart);
                    cout << "Time taken for Unordered Map: " << mapDuration.count() / 1000000.0 << " seconds." << endl;
                }

                //start time
                auto treeStart = high_resolution_clock::now();
                cout << endl << "AVL Tree: " << endl;
                vector<pair<int, string>> t;
                int singleWordFreq = 0;
                auto artistNode = artistTree.artistSearch(artistTree.getRoot(), artist);//if artist exists, look for word 
                if (artistNode != nullptr) {
                    // count number of times word pops up
                    singleWordFreq = artistTree.singleWordFreq(artistNode, word);
                }
                if (singleWordFreq == 0)
                    cout << "Word does not appear in lyrics of " << artist << endl;
                else {
                    string censoredWord = censor(word);
                    cout << censoredWord << " appears " << singleWordFreq;
                    if (singleWordFreq == 1)
                        cout << " time." << endl;
                    else
                        cout << " times." << endl;
                    // stop time, calculate, convert to microseconds
                    auto treeStop = high_resolution_clock::now();
                    auto treeDuration = duration_cast<microseconds>(treeStop - treeStart);
                    cout << "Time taken for AVL Tree: " << treeDuration.count() / 1000000.0 << " seconds." << endl << endl;
                }

            }
            else
                cout << "Artist does not exist in database." << endl << endl;
            break;
        }
        case 4: {
            cout << endl << "Unordered Map: " << endl;
            string input = wordsInput();
            //start time
            auto mapStart = high_resolution_clock::now();
            if (totalFreq.empty())
                makeTotalFreqMap(artistLibrary, totalFreq);
            if (topWords(totalFreq, input)) {
                // stop time, calculate, convert to microseconds
                auto mapStop = high_resolution_clock::now();
                auto mapDuration = duration_cast<microseconds>(mapStop - mapStart);
                cout << "Time taken for Unordered Map: " << mapDuration.count() / 1000000.0 << " seconds." << endl;
            }

            cout << endl << "AVL Tree: " << endl;
            input = wordsInput();
            //start time
            auto treeStart = high_resolution_clock::now();

            if (artistTree.getTotalFreqTree().empty())
                artistTree.inorderTraversal(artistTree.getRoot());

            if (topWords(artistTree.getTotalFreqTree(), input)) {
                // stop time, calculate, convert to microseconds
                auto treeStop = high_resolution_clock::now();
                auto treeDuration = duration_cast<microseconds>(treeStop - treeStart);
                cout << "Time taken for AVL Tree: " << treeDuration.count() / 1000000.0 << " seconds." << endl << endl;

            }

            break;
        }
        case 5: {
            exit(0);
        }
        default:
            cout << "Please enter a valid menu option." << endl;
            break;
        }
    }
    return 0;
}

void parseData(string fileName, unordered_map<string, vector<pair<string, string>>>& artistLibrary) {

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
        if (artist.find("\"\",\"\"") != string::npos) {
            artist = artist.substr(0, artist.find("\"\",\"\""));
        }

        //remove extra quotes
        artist.erase(remove(artist.begin(), artist.end(), '"'), artist.end());
        songTitle.erase(remove(songTitle.begin(), songTitle.end(), '"'), songTitle.end());
        artistLibrary[artist].push_back(make_pair(songTitle, songLyrics));

    }
}

unordered_map<string, int> makeFreqMap(string artist, unordered_map<string, vector<pair<string, string>>>& artistLibrary) {

    int numSongs = artistLibrary[artist].size();
    unordered_map<string, int> freqMap;
    //for each song, go through its lyrics and add to frequency map
    for (int i = 0; i < numSongs; i++) {
        string word;
        stringstream lyricStream(artistLibrary[artist][i].second);
        while (getline(lyricStream, word, ' ')) {
            eraseExtraChars(word);
            if (word.size() > 0)
                freqMap[word]++;
        }

    }

    return freqMap;

}

void makeTotalFreqMap(unordered_map<string, vector<pair<string, string>>>& artistLibrary, unordered_map<string, int>& totalFreq) {

    for (auto iter = artistLibrary.begin(); iter != artistLibrary.end(); iter++) {
        //iterates thru lyrics of artist
        for (unsigned int i = 0; i < iter->second.size(); i++) {
            string word;
            stringstream lyrics(iter->second[i].second);
            while (getline(lyrics, word, ' ')) {
                eraseExtraChars(word);
                totalFreq[word]++;
            }
        }
    }

}

bool artistExists(string& artist, unordered_map<string, vector<pair<string, string>>>& artistLibrary) {

    if (artistLibrary.count(artist))
        return true;

    //check if artist is in database in format last name, first name
    else if (artist.find(' ') != string::npos) {
        string newArtist = artist.substr(artist.find(' ') + 1, artist[artist.length() - 1]);
        newArtist += ", " + artist.substr(0, artist.find(' '));
        if (artistLibrary.count(newArtist)) {
            artist = newArtist;
            return true;
        }
    }
    return false;
}

vector<pair<int, string>> sortMap(unordered_map<string, int>& freq) {
    vector<pair<int, string>> orderedFreq;
    for (auto iter = freq.begin(); iter != freq.end(); iter++) {
        orderedFreq.push_back(make_pair(iter->second, iter->first));
    }

    sort(orderedFreq.begin(), orderedFreq.end());
    return orderedFreq;
}

bool topWords(unordered_map<string, int>& freq, string input) {
    int count = 1;

    if (input == "10" || input == "20" || input == "50" || input == "100") {
        cout << "Top " << input << " most popular words: " << endl;

        vector<pair<int, string>> orderedFreq = sortMap(freq);
        for (unsigned int i = orderedFreq.size() - 1; i >= orderedFreq.size() - stoi(input); i--) {
            //method here that accepts orderedFreq[i], maybe assign string here so we are not couting wrong word
            string word = censor(orderedFreq[i].second);
            cout << count << ". " << word << ": " << orderedFreq[i].first << " times." << endl;
            if (orderedFreq.size() > stoi(input)) {
                if (i == orderedFreq.size() - stoi(input))
                    break;
            }
            count++;
        }
        return true;
    }
    else {
        cout << "Invalid input." << endl;
        return false;
    }
}
string wordsInput() {
    cout << "View top 10, 20, 50, or 100 words? ";
    string input;
    getline(cin, input, '\n');

    return input;

}

/* We only have these curse words here so we do not have the full word printed when the program is running */
string censor(string word) {
    set<string> badWords = { "fuck", "fucked", "fuckin'", "fucking", "motherfucker", "shit", "bitch", "bitches", "ass", "bullshit", "damn", " asshole", "dick", "nigga", "nigger", "niggas", "faggot", "fag" };

    if (badWords.count(word)) {
        //store length before length becomes 1
        int length = word.length() - 1;
        word = word.at(0);
        for (int i = 0; i < length; i++) {
            word += "*";
        }
    }
    return word;
}