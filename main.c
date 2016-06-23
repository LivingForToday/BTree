#include <stdio.h>
#include "BTree.h"

/* 파일 인풋 함수 선언 */
int InputElementsFromFile(BTreeNode ** pRoot);

int main()
{
	BTreeNode * btRoot;
	btRoot = BTree_Init();

	int choice, num;
	while (1)
	{
		fputs("1.삽입\n2.삭제\n3.검색\n4.출력\n9.일괄 삽입\n0.종료\n선택: ", stdout);
		scanf("%d", &choice);
		puts("\n");

		if (choice == 0)
			break;

		switch (choice)
		{
		case 1:

			while (1)
			{
				fputs("삽입할 데이터 입력(종료 -1): ", stdout);
				scanf("%d", &num);

				if (num == -1)
					break;

				if (BTree_Insert(&btRoot, num) == FALSE)
					puts("입력 실패.");
			}
			break;

		case 2:

			while (1)
			{
				fputs("삭제할 데이터 입력(종료 -1): ", stdout);
				scanf("%d", &num);

				if (num == -1)
					break;

				if (BTree_Delete(&btRoot, num) == FALSE)
					puts("입력한 데이터가 트리에 존재하지 않음.");
			}
			break;

		case 3:

			while (1)
			{
				fputs("검색할 데이터 입력(종료 -1): ", stdout);
				scanf("%d", &num);

				if (num == -1)
					break;

				fputs("\n<<검색경로>>\n\n", stdout);
				if (BTree_Search(&btRoot, num) == FALSE)
					puts("입력한 데이터가 트리에 존재하지 않음.");
				puts("");
			}
			break;

		case 4:

			ShowAll(btRoot, 0);
			break;

		case 9:
			printf("<< 일괄삽입된 %d개 데이터 출력 >>\n\n", InputElementsFromFile(&btRoot));
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