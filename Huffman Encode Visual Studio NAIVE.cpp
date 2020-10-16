#include <iostream>
#include <unordered_map>
#include <queue>
#include <string>
#include <errno.h>
#include <vector>


//structure to build the Huffman Tree
struct tree
{
	char data;
	long long int weight;
	tree* left, * right;
	tree(char data, long long int weight) :data(data), weight(weight), left(NULL), right(NULL){}
};
s

//comparator function for priority queue
struct compare
{
	bool operator()(tree* l, tree* r)
	{
		return (l->weight > r->weight);
	}
};

#define MAX_HEIGHT 256

std::unordered_map<int, std::string> code;
std::priority_queue<tree*, std::vector<tree*>, compare> buff;
long long int freq[MAX_HEIGHT];

int bit_counter;
unsigned char current_byte;


//for calculating the frequency distribution of each character present in file
void definefreq(FILE*& orig)
{
	char c;
	while ((c = fgetc(orig)) != EOF)
	{
		++freq[(int)c];
	}
}


//building huffman tree
void hufftree()
{
	tree* left, * right, * top;
	for (int i = 0; i < MAX_HEIGHT; i++)
	{
		if (freq[i] > 0)
		{
			buff.push(new tree((char)i, freq[i]));
		}
	}
	while (buff.size() != 1)
	{
		left = buff.top();
		buff.pop();
		right = buff.top();
		buff.pop();
		top = new tree('$', left->weight + right->weight);
		top->left = left;
		top->right = right;
		buff.push(top);
	}
}


//getting codes for each character
void savecode(tree* root, std::string a)
{
	if (!root)
		return;
	if (root->data != '$')
	{
		code[(int)root->data] = a;
	}
	savecode(root->left, a + "0");
	savecode(root->right, a + "1");
}


//to free the dynamic memory allocated during building the huffman tree
void freequeue(tree* root)
{
	tree* left, * right;
	left = root->left;
	right = root->right;
	if (left)
	{
		freequeue(left);
		delete left;
	}
	if (right)
	{
		freequeue(right);
		delete right;
	}
}


//writing alphabet codes distribution file
void write_freq_file()
{
	FILE* ff;
	errno_t err = fopen_s(&ff, "frequency_file.txt", "w+");

	for (int i = 0; i < MAX_HEIGHT; i++)
	{
		if (freq[i] > 0)
		{
			fprintf(ff, "%c %s\n", (char)i, code[i].c_str());
		}
	}
}


//writing encoded file
void write(char a, FILE*& enco)
{
	if (bit_counter == 7)
	{
		fwrite(&current_byte, 1, 1, enco);
		bit_counter = 0;
		current_byte = 0;
	}
	bit_counter++;
	current_byte <<= 1;
	current_byte |= a;
}

void write_encoded_file(FILE*& orig)
{
	FILE* enco;
	errno_t err2 = fopen_s(&enco, "encoded_file.DAT", "wb");
	char a;
	current_byte = 0;
	bit_counter = 0;

	while ((a = fgetc(orig)) != EOF)
	{
		std::string s = code[(int)a];
		for (int i = 0; i < s.size(); i++)
			write((int)s[i] - 48, enco);
	}

	if (bit_counter > 0)
	{
		current_byte <<= 7 - bit_counter;
		fwrite(&current_byte, 1, 1, enco);
	}
	_fcloseall();
}



int main()
{
	FILE* orig;
	errno_t err = fopen_s(&orig, "text.txt", "rt");
	if (err != 0)
	{
		std::cout << "File was not opened" << std::endl;
		return 0;
	}

	definefreq(orig);
	fseek(orig, 0, SEEK_SET);

	hufftree();
	savecode(buff.top(), "");
	freequeue(buff.top());
	write_freq_file();
	write_encoded_file(orig);

	return 0;
}
