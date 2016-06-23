#include <stdio.h>
#include "BTree.h"

int InputElementsFromFile(BTreeNode ** pRoot);

int main()
{
	BTreeNode * btRoot;
	btRoot = BTree_Init();

	int choice, num;
	while (1)
	{
		fputs("1.insert\n2.delete\n3.search\n4.print all keys\n9.read file\n0.exit\nchoose one: ", stdout);
		scanf("%d", &choice);
		puts("\n");

		if (choice == 0)
			break;

		switch (choice)
		{
		case 1:

			while (1)
			{
				fputs("input a key value to be inserted(menu -1): ", stdout);
				scanf("%d", &num);

				if (num == -1)
					break;

				if (BTree_Insert(&btRoot, num) == FALSE)
					puts("insert failed");
			}
			break;

		case 2:

			while (1)
			{
				fputs("input a key value to be deleted(menu -1): ", stdout);
				scanf("%d", &num);

				if (num == -1)
					break;

				if (BTree_Delete(&btRoot, num) == FALSE)
					puts("the key value you inputted does not exist in tree. ");
			}
			break;

		case 3:

			while (1)
			{
				fputs("input a key value you want to search(menu -1): ", stdout);
				scanf("%d", &num);

				if (num == -1)
					break;

				fputs("\n<< path >>\n\n", stdout);
				if (BTree_Search(&btRoot, num) == FALSE)
					puts("the key value you inputted does not exist in tree.");
				puts("");
			}
			break;

		case 4:

			ShowAll(btRoot, 0);
			break;

		case 9:
			printf("<< %d key values have been inserted successfully. >>\n\n", InputElementsFromFile(&btRoot));
			ShowAll(btRoot, 0);
			break;

		} // end switch
	} // end while

	return 0;
}

int InputElementsFromFile(BTreeNode ** pRoot)
{

	FILE * fd = fopen("C:\\", "r"); // you should change path of the file.
	int data, count = 0;

	if (fd == NULL)
	{
		puts("open failed");
		return -1;
	}

	while (fscanf(fd, "%d", &data) != EOF) {
		BTree_Insert(pRoot, data);
		count++;
	}

	fclose(fd);

	return count;
}
