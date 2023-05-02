/*****************************************************
The code is implementing a Huffman coding algorithm, which is a lossless data compression technique. 
The program takes an input string and compresses it using Huffman coding, which assigns shorter codes to more frequently occurring characters in the input string.

The program first counts the frequency of each character in the input string and creates a binary tree where each leaf node represents a character and its frequency. 
It then traverses the tree to assign unique binary codes to each character based on their frequency.

The program then encodes the input string using the Huffman codes and prints the encoded string to the console. 

It also prints various statistics about the Huffman codes and frequencies, such as the total number of bits used to encode the input string 
and the compression ratio achieved.

Finally, the program decodes the encoded string back to the original input string using the Huffman codes and prints it to the console.

******************************************************/

#include <iostream>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>

using namespace std;

const int MAX_CONSOLE_COLUMNS = 200;
const int ERROR_FILE_OPEN = 1;

// A Tree node
struct Node {
    char ch;
    int freq;
    Node* left, * right;
};

// Function to allocate a new tree node
Node* getNode(char ch, int freq, Node* left, Node* right)
{
    Node* node = new Node();

    node->ch = ch;
    node->freq = freq;
    node->left = left;
    node->right = right;

    return node;
}

// Comparison object to be used to order the heap
struct comp {
    bool operator()(Node* l, Node* r)
    {
        return l->freq > r->freq;
    }
};

// Traverse the Huffman Tree and store Huffman Codes
void encode(Node* root, string str,
            unordered_map<char, string>& huffmanCode,
            vector<pair<char, string>>& order)
{
    if (root == nullptr)
        return;

    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
        order.push_back(make_pair(root->ch, str));
    }

    encode(root->left, str + "0", huffmanCode, order);
    encode(root->right, str + "1", huffmanCode, order);
    
}

// Traverse the Huffman Tree and decode the encoded string
void decode(Node* root, int& index, string str)
{
    if (root == nullptr) {
        return;
    }

    // found a leaf node
    if (!root->left && !root->right)
    {
        cout << root->ch;
        return;
    }

    index++;

    if (str[index] == '0')
        decode(root->left, index, str);
    else
        decode(root->right, index, str);
}

int countTotalCharacters(string text) {
    int count = 0;
    for (char ch : text) {
        count++;
    }
    cout << "\nTotal number of characters: " << count << endl;
    return count;
}

void printError(string message) {
    cerr << "Error: " << message << endl;
}

// Builds Huffman Tree and decodes the input text
void buildHuffmanTree(string text)
{
    // count frequency of appearance of each character
    unordered_map<char, int> freq;

    for (char ch : text) {
        if (!isascii(ch)) {
            printError("Input text contains non-ASCII characters.");
            return;
        }
        freq[ch]++;
    }

    countTotalCharacters(text);

    // Create a priority queue to store live nodes of the Huffman tree
    priority_queue<Node*, vector<Node*>, comp> pq;

    // Create a leaf node for each character and add it to the priority queue.
    for (auto pair : freq) {
        pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
    }

    // do till there is more than one node in the queue
    while (pq.size() != 1)
    {
        // Remove the two nodes of highest priority (lowest frequency) from the queue
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top();    pq.pop();

        // Create a new internal node with these two nodes as children and with frequency equal to the sum of the two nodes' frequencies.
        int sum = left->freq + right->freq;
        pq.push(getNode('\0', sum, left, right));
    }

    // root stores pointer to root of Huffman Tree
    Node* root = pq.top();

    // Traverse the Huffman Tree and store Huffman Codes
    unordered_map<char, string> huffmanCode;
    vector<pair<char, string>> order;
    unordered_map<int, int> bitCountMap;
    unordered_map<int, int> freqBitCountMap;
    encode(root, "", huffmanCode, order);
    
    // Sort the order vector by frequency
    sort(order.begin(), order.end(), [&](pair<char, string>& a, pair<char, string>& b) {
        return freq[a.first] < freq[b.first];
    });


    // Calculate the bit count and frequency of each Huffman code
    for (auto pair : huffmanCode) {
        bitCountMap[pair.second.length()]++;
        freqBitCountMap[pair.second.length()] += freq[pair.first];
    }

    // Find the maximum bit count
    int maxBitCount = 0;
    for (auto pair : freqBitCountMap) {
        if (pair.first > maxBitCount) {
            maxBitCount = pair.first;
        }
    }
    
    // Print out the characters with their Huffman codes and frequencies in order of frequency
    cout << "\nHuffman Codes and Frequencies are (in order of frequency):\n" << endl;
    for (auto pair : order) {
        cout << setw(1) << pair.first << "  ";
        cout << setw(maxBitCount) << pair.second << "  ";
        cout <<"("<< freq[pair.first]<< ")" << endl;
    }

    // Print out the bit count, number of characters, and frequency for each group of characters with the same bit count
    cout << "\nBits, Number, and Frequency of characters with the same bit count: " << endl;
    for (auto pair : freqBitCountMap) {
        int bitCount = pair.first;
        int frequency = pair.second;
        int numChars = bitCountMap[bitCount];
        cout << "Bits: " << bitCount << "  Numbers: " << numChars << "  Frequency: " << frequency << endl;
    }
    
    cout << "\nThe original string is :\n" << text << endl;

    // print encoded string
    string str = "";
    for (char ch : text) {
        str += huffmanCode[ch];
    }
    cout << "\nThe encoded string is :\n" << str << endl;

    // decode the encoded string
    int index = -1;
    cout << "\nThe decoded string is :\n";
    while (index < (int)str.size() - 2) {
        decode(root, index, str);
    }
    
    //

    string encodedText = "";
    for (char c : text) {
        encodedText += huffmanCode[c];
    }
    
    // Calculate compression ratio
    double compressionRatio = (double)encodedText.length() / (text.length() * 8) * 100;
    
    // Print the bits before and after encoding, as well as the compression ratio
    cout << "\n\nBits before encoding: " << text.length() * 8 << endl;
    cout << "Bits after encoding: " << encodedText.length() << endl;
    cout << "Compression ratio: " << compressionRatio << "%" << endl;  
}

// Driver program to test above functions
int main()
{
    ifstream file("input.txt");
    if (!file) {
        printError("Could not open input file.");
        return ERROR_FILE_OPEN;
    }
    string text;
    getline(file, text);
    buildHuffmanTree(text);

    cout << "\nPress Enter to exit...";
    cin.get(); // Wait for user to press Enter
    return 0;
}
