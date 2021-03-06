#include <iostream>
#include <string>
#include <errno.h>

struct tree
{
	char data;
	tree* left, * right;
	tree(char data) :data(data),left(NULL),right(NULL){}
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

void decode(FILE*& encoded)
{
	FILE* decoded;
	errno_t err = fopen_s(&decoded, "decoded_file.txt", "wt");
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

int main()
{
	FILE* freq, * encoded;
	errno_t err1 = fopen_s(&freq, "frequency_file.txt", "rt");
	if (err1 != 0)
	{
		std::cout << "Frequency file was not opened" << std::endl;
		return 0;
	}
	errno_t err2 = fopen_s(&encoded, "encoded_file.DAT", "rb");
	if (err2 != 0)
	{
		std::cout << "Encoded file was not opened" << std::endl;
		return 0;
	}
	freqfile_decode(freq);
	decode(encoded);
	free_memory(root);

	return 0;
}