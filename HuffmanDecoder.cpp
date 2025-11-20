#include <bits/stdc++.h>
using namespace std;

struct Node{
    int ch, freq;
    Node *left, *right;
    Node(int ch, int freq){
        this->ch = ch;
        this->freq = freq;
        left = NULL;
        right = NULL;
    }
};

struct comp{
    bool operator()(const Node* l, const Node* r) const {
        return l->freq > r->freq;
    }
};

bool isLeaf(Node* root){
    return !root->left && !root->right;
}

Node* buildTree(const vector<int> &freq){
    priority_queue<Node*, vector<Node*>, comp> pq;
    for(int i = 0; i < 128; i++){
        if(!freq[i]) continue;
        pq.push(new Node(i, freq[i]));
    }

    if(pq.empty()) return nullptr;

    while(pq.size() > 1){
        Node* l = pq.top(); pq.pop();
        Node* r = pq.top(); pq.pop();
        Node* merged = new Node(-1, l->freq + r->freq);
        merged->left = l;
        merged->right = r;
        pq.push(merged);
    }

    return pq.top();
}

int main(int argc, char* argv[]){
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <input_filename_without_extension> <output_filename_without_extension>\n";
        return 1;
    }

    string inputName = (string)argv[1] + ".bin";
    string outputName = (string)argv[2] + ".txt";

    ifstream binaryFile(inputName, ios::binary);
    ofstream outputFile(outputName);

    if (!binaryFile.is_open() || !outputFile.is_open()) {
        cout << "Error! Could not open input or output file.\n";
        return 1;
    }

    // Read frequency table
    vector<int> freq(128, 0);
    for(int i = 0; i < 128; i++){
        binaryFile.read(reinterpret_cast<char*>(&freq[i]), sizeof(int));
    }

    Node* root = buildTree(freq);

    // Read padding info
    unsigned char padInfo;
    binaryFile.read(reinterpret_cast<char*>(&padInfo), 1);

    // Read binary data
    vector<unsigned char> bytes(
        (istreambuf_iterator<char>(binaryFile)),
        (istreambuf_iterator<char>())
    );
    binaryFile.close();

    string bits = "";
    for(unsigned char byte : bytes) {
        bitset<8> b(byte);
        bits += b.to_string();
    }

    // Remove padding bits
    if(padInfo > 0 && padInfo <= 7)
        bits = bits.substr(0, bits.size() - padInfo);

    // Decode
    string decoded = "";
    Node* temp = root;
    for(char bit : bits){
        temp = (bit == '0') ? temp->left : temp->right;
        if (isLeaf(temp)) {
            decoded += (char)temp->ch;
            temp = root;
        }
    }

    outputFile << decoded;
    outputFile.close();

    cout << "Decompression successful! Output file: " << outputName << endl;
    return 0;
}
