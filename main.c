#include <stdio.h>
#include "BTree.h"

/* ���� ��ǲ �Լ� ���� */
int InputElementsFromFile(BTreeNode ** pRoot);

int main()
{
	BTreeNode * btRoot;
	btRoot = BTree_Init();

	int choice, num;
	while (1)
	{
		fputs("1.����\n2.����\n3.�˻�\n4.���\n9.�ϰ� ����\n0.����\n����: ", stdout);
		scanf("%d", &choice);
		puts("\n");

		if (choice == 0)
			break;

		switch (choice)
		{
		case 1:

			while (1)
			{
				fputs("������ ������ �Է�(���� -1): ", stdout);
				scanf("%d", &num);

				if (num == -1)
					break;

				if (BTree_Insert(&btRoot, num) == FALSE)
					puts("�Է� ����.");
			}
			break;

		case 2:

			while (1)
			{
				fputs("������ ������ �Է�(���� -1): ", stdout);
				scanf("%d", &num);

				if (num == -1)
					break;

				if (BTree_Delete(&btRoot, num) == FALSE)
					puts("�Է��� �����Ͱ� Ʈ���� �������� ����.");
			}
			break;

		case 3:

			while (1)
			{
				fputs("�˻��� ������ �Է�(���� -1): ", stdout);
				scanf("%d", &num);

				if (num == -1)
					break;

				fputs("\n<<�˻����>>\n\n", stdout);
				if (BTree_Search(&btRoot, num) == FALSE)
					puts("�Է��� �����Ͱ� Ʈ���� �������� ����.");
				puts("");
			}
			break;

		case 4:

			ShowAll(btRoot, 0);
			break;

		case 9:
			printf("<< �ϰ����Ե� %d�� ������ ��� >>\n\n", InputElementsFromFile(&btRoot));
			ShowAll(btRoot, 0);
			break;

		} // end switch
	} // end while

	return 0;
}

int InputElementsFromFile(BTreeNode ** pRoot)
{

	FILE * fd = fopen("C:\\rand100.txt", "r");
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