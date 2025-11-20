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
    return root && !root->left && !root->right;
}


void dfs(Node* temp, const string &st, vector<string>& code){
    if(!temp) return;
    if(isLeaf(temp)) {
        code[temp->ch] = (st.size() ? st : "1");
        return;
    }
    dfs(temp->left, st + "0", code);
    dfs(temp->right, st + "1", code);
}

string getEncoded(string &text, vector<string> &code){
    string coded;
    coded.reserve(text.size() * 2);
    for(char ch : text){
        coded += code[(int)ch];
    }
    return coded;
}

static vector<int> g_freq(128, 0);
static Node* g_root = nullptr;

string buildTree(string text){
    if(text.empty()) return "";

    fill(g_freq.begin(), g_freq.end(), 0);
    if(g_root) {
        g_root = nullptr;
    }

    for(char ch : text) {
        g_freq[(int)ch]++;
    }

    priority_queue<Node*, vector<Node*>, comp> pq;
    for(int i = 0; i < 128; i++) {
        if (!g_freq[i]) continue;
        Node *temp = new Node(i, g_freq[i]);
        pq.push(temp);
    }

    if(pq.empty()) return "";

    while(pq.size() > 1){
        Node *l = pq.top(); pq.pop();
        Node *r = pq.top(); pq.pop();

        Node *merged = new Node(-1, l->freq + r->freq);
        merged->left = l;
        merged->right = r;

        pq.push(merged);
    }

    g_root = pq.top();
    Node *temp = g_root;

    string st = "";
    vector<string> code(128);
    dfs(temp, st, code);

    string coded = getEncoded(text, code);
    return coded;
}

int main(int argc, char* argv[]){
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <input_filename_without_extension> <output_filename_without_extension>\n";
        return 1;
    }

    string inputName = string(argv[1]) + ".txt";
    string outputName = string(argv[2]) + ".bin";
    string outputName2 = string(argv[3]) + ".txt";

    ifstream inputFile(inputName);
    ofstream binaryFile(outputName, ios::binary);
    ofstream outFile(outputName2);//.....................>

    if(!inputFile.is_open() || !binaryFile.is_open()) {
        cout << "Error! Could not open input or output file.\n";
        return 1;
    }

    string text, line;
    bool first = true;
    while(getline(inputFile, line)){
        if(!first) text += '\n';
        text += line;
        first = false;
    }
    inputFile.close();
    cout<<text<<endl;

    cout<<text.size()<<endl;
    string coded = buildTree(text);

    outFile.write(coded.data(), coded.size());//.....................>

    if(coded.empty() && text.empty()){
        cout << "Empty input file.\n";
        return 0;
    }

    // Pad to multiple of 8 bits (if not a multiple of 8......)
    int padding = (8 - (int)(coded.size() % 8)) % 8;
    if(padding) coded.append(padding, '0');
    // cout<<coded<<endl;

    int x = 0;

    // Write frequency table first (for decoding)
    for(int i = 0; i < 128; i++){
        int val = g_freq[i];
        binaryFile.write(reinterpret_cast<char*>(&val), sizeof(int));
        x += sizeof(int); 
    }

    // Write padding info
    char padInfo = static_cast<char>(padding);
    binaryFile.write(&padInfo, sizeof(char));
    x += sizeof(char); 

// Write binary encoded data
    for(int i = 0; i < coded.size(); i += 8){
        string byteStr = coded.substr(i, 8);
        unsigned char byte = static_cast<char>(stoi(byteStr, nullptr, 2));
        // cout<<byte<<endl;
        binaryFile.write(reinterpret_cast<char*>(&byte), 1);
        x += 1;  
    }

    binaryFile.close();
    cout<<x<<endl;

    cout << "Compression successful! Output file: " << outputName << endl;
    return 0;
}
