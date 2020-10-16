#include <iostream>
#include <string>
#include <errno.h>

struct tree
{
	char data;
	tree* left, * right;
	tree(char data) :data(data), left(NULL), right(NULL) {}
};

tree* root = new tree('$');

void build_tree(char data, std::string code)
{
	tree* temp(root);
	for (int i = 0; i < code.size(); i++)
	{
		if (code[i] == '0')
		{
			if (temp->left == NULL)
				temp->left = new tree('$');
			temp = temp->left;
		}
		else
		{
			if (temp->right == NULL)
				temp->right = new tree('$');
			temp = temp->right;
		}
	}
	temp->data = data;
}

void freqfile_decode(FILE*& freq)
{
	char c;
	while ((c = fgetc(freq)) != EOF)
	{
		char a = c;
		std::string code;
		c = fgetc(freq);
		while ((c = fgetc(freq)) != '\n')
			code += c;
		build_tree(a, code);
	}
}

void decode(FILE*& encoded, FILE* decoded)
{
	char c;
	tree* temp = root;
	while ((c = fgetc(encoded)) != EOF)
	{
		for (int i = 6; i >= 0; i--)
		{
			int x = ((c >> i) & 1);
			if (!x)
			{
				temp = temp->left;
				if (temp->data != '$')
				{
					fprintf(decoded, "%c", temp->data);
					temp = root;
				}
			}
			else
			{
				temp = temp->right;
				if (temp->data != '$')
				{
					fprintf(decoded, "%c", temp->data);
					temp = root;
				}
			}
		}
	}
}

void free_memory(tree* node)
{
	if (node->left)
	{
		tree* left, * right;
		left = node->left;
		right = node->right;
		if (left)
		{
			free_memory(left);
			delete left;
		}
		if (right)
		{
			free_memory(right);
			delete right;
		}
	}
}

int main(int argc, char** argv)
{
	FILE* freq, * encoded,* decoded;

	if (argc != 4)
	{
		fprintf(stderr, "huffman_decode_dynamic.exe <frequency file> <encoded_file.DAT> <output file>");
	}

	std::string frequency, encode, decodestr;
	frequency = argv[1];
	encode = argv[2];
	decodestr = argv[3];
	freq = fopen(frequency.c_str(), "r");
	encoded = fopen(encode.c_str(), "rb");
	decoded = fopen(decodestr.c_str(), "wt");
	freqfile_decode(freq);
	decode(encoded, decoded);
	free_memory(root);

	return 0;
}
