#include <bits/stdc++.h>
using namespace std;

struct Node {
	int ch, freq;
	Node *left, *right;

	Node(int ch, int freq)
		: ch(ch), freq(freq), left(NULL), right(NULL) {}
};

struct comp {
	bool operator()(const Node* l, const Node* r) const {
		return l->freq > r->freq;
	}
};

bool isLeaf(Node* root) {
	return !root->left && !root->right;
}

void dfs(Node *temp, string st, vector<string>&code) {
	if (isLeaf(temp)) {
		code[temp->ch] = (st.size() ? st : "1");
		return;
	}

	dfs(temp->left, st + "0", code);
	dfs(temp->right, st + "1", code);
}

string getEncoded(string &text, vector<string>&code) {
	string coded = "";
	for (auto &ch : text) {
		coded += code[(int)ch];
	}

	return coded;
}

string buildTree(string text) {
	if (text.empty()) return "";

	vector<int> freq(128);
	for (auto &ch : text) {
		freq[(int)ch]++;
	}

	priority_queue<Node*, vector<Node*>, comp> pq;
	for (int i = 0; i < 128; i++) {
		if (!freq[i]) continue;

		Node *temp = new Node(i, freq[i]);
		pq.push(temp);
	}

	while (pq.size() > 1) {
		Node *l = pq.top(); pq.pop();
		Node *r = pq.top(); pq.pop();

		Node *merged = new Node(-1, l->freq + r->freq);
		merged->left = l;
		merged->right = r;

		pq.push(merged);
	}

	Node *root = pq.top();
	Node *temp = root;

	string st = "";
	vector<string> code(128);

	dfs(temp, st, code);

	string coded = getEncoded(text, code);

	temp = root;
	string decoded = "";
	for (auto &ch : coded) {
		if (ch == '1') temp = temp->right;
		else temp = temp->left;

		if (isLeaf(temp)) {
			decoded += (char)temp->ch;
			temp = root;
		}
	}

	return coded;
}

int main(int argc, char *argv[]) {
	ifstream inputFile;
	inputFile.open((string)argv[1] + ".txt");

	ofstream binaryFile((string)argv[2] + ".bin", ios::binary);

	if (inputFile.is_open() && binaryFile.is_open()) {
		string text = "";
		string line;

		int cnt = 0;
		while (getline(inputFile, line)) {
			if (cnt) text += "\n";
			text += line;
			cnt++;
		}

		inputFile.close();

		string coded = buildTree(text);
		for (size_t i = 0; i < coded.size(); i += 8) {
			string byteStr = coded.substr(i, 8);
			char byte = static_cast<char>(stoi(byteStr, nullptr, 2));
			binaryFile.write(&byte, sizeof(char));
		}

		binaryFile.close();
	}

	else {
		cout << "Error! Something went wrong.";
	}

	return 0;
}