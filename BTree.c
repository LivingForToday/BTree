#include "BTree.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int TREE_LEVEL = 0;

BTreeNode* Make_BTNode()
{
	BTreeNode * newNode = (BTreeNode*)malloc(sizeof(BTreeNode));
	newNode->numOfKeys = 0;

	for (int i = 0; i < KEYS_LENGTH + 1; i++)
		newNode->childPtr[i] = NULL;

	return newNode;
}

BTreeNode* BTree_Init()
{
	BTreeNode * root = Make_BTNode();
	
	return root;
}

/* btNode에 key를 삽입하고, key의 좌우에 left, rifht 연결. */
void Insert_Key(BTreeNode * btNode, BTreeNode * right, const BTData key)
{
	int i;
	i = btNode->numOfKeys;

	while ((btNode->key[i - 1] > key) && (i > 0))
	{
		btNode->key[i] = btNode->key[i - 1];
		btNode->childPtr[i + 1] = btNode->childPtr[i];
		i--;
	}
	btNode->key[i] = key;
	btNode->childPtr[i + 1] = right;
	btNode->numOfKeys++;

	//assert(btNode->numOfKeys >= MINIMIN_NUM_OF_KEYS);
}

/* 루트부터 시작하여 level까지 key를 찾아가는 경로 중 있는 노드 or 리프 노드 반환 
   특정 노드의 부모 노드를 찾거나, 특정 리프 노드를 찾는데에 사용 됨. */
BTreeNode* Find_BTNode(BTreeNode * root, const BTData key, const int level)
{
	BTreeNode * curNode = root;
	int i, j;

	for (i = 0; i < level; i++)
	{
		for (j = 0; j < curNode->numOfKeys; j++)
		{
			if (curNode->key[j] == key)
			{
				return NULL;
			}
			else if (curNode->key[j] > key)
			{
				curNode = curNode->childPtr[j];
				break;
			}
			else if (j == curNode->numOfKeys - 1 && curNode->key[j] < key)
			{
				curNode = curNode->childPtr[j + 1];
				break;
			}
		}
	} // end for

	for (i = 0; i < curNode->numOfKeys; i++)
		if (curNode->key[i] == key)	return NULL;

	return curNode;
}

/* 노드를 left, rifht로 분할, 오른쪽이 2/m 안될시, 재분배까지. */
int Split_BTNode(BTreeNode * left, BTreeNode * right, BTreeNode * subRight, const int key)
{
	/*left right스플릿하고, key도 적절한 곳에 넣고, 상위 레벨에 전달해야 할 key값을 반환. */

	int mid = KEYS_LENGTH / 2;
	int i, rightIdx = 0;

	i = (left->key[mid] > key) ? mid : mid + 1;

	for (; i < KEYS_LENGTH; i++)
	{
		right->key[rightIdx] = left->key[i];
		right->childPtr[rightIdx] = left->childPtr[i];
		rightIdx++;
		left->numOfKeys--;
		right->numOfKeys++;
	}
	right->childPtr[rightIdx] = left->childPtr[i]; //left->numOfKeys == 2 or 3

												   // key를 left or right에 삽입.
	if (left->numOfKeys > MINIMIN_NUM_OF_KEYS) //right에 삽입되는 경우
	{
		assert(left->numOfKeys == mid + 1);
		for (i = right->numOfKeys - 1; i >= 0; i--)
		{
			if (right->key[i] < key)
			{
				right->key[i + 1] = key;
				right->childPtr[i + 2] = subRight;
				right->numOfKeys++;
				break;
			}
			right->key[i + 1] = right->key[i];
			right->childPtr[i + 2] = right->childPtr[i + 1];
		}// end for

		if (i == -1 && right->key[0] > key)
		{
			right->key[0] = key;
			right->childPtr[1] = subRight;
			right->numOfKeys++;
		}
		assert(right->numOfKeys >= MINIMIN_NUM_OF_KEYS);
	}
	else // left에 key가 삽입되는 경우
	{
		if (left->key[left->numOfKeys - 1] < key) //인자로 받은 키가 left의 마지막 키값보다 클 경우.
		{
			right->childPtr[0] = subRight;		//오른쪽의 첫번째 포인터를 subRight로.
			left->key[mid] = key;				//레프트의 마지막에 key 추가.
		}
		else
		{
			left->key[mid] = left->key[mid - 1]; // 일단 왼쪽의 가장 마지막 키를 mid번째에 둠.

			for (i = mid - 1; i > 0; i--)
			{
				if (left->key[i - 1] < key) {
					left->key[i] = key;
					left->childPtr[i + 1] = subRight;
					break;
				}

				left->key[i] = left->key[i - 1];
				left->childPtr[i + 1] = left->childPtr[i];
			} //end for

			if (i == 0 && left->key[0] > key)
			{
				left->key[0] = key;
				left->childPtr[1] = subRight;
			}
		}
	}

	assert(right->numOfKeys >= MINIMIN_NUM_OF_KEYS);
	assert(left->numOfKeys >= MINIMIN_NUM_OF_KEYS);

	left->numOfKeys = mid;
	right->numOfKeys = mid;

	return left->key[mid];
}

/* key삽입시, 노드가 꽉 찼을 때, 호출.
   경우에 따라서 스플릿 하거나, 형제에게 키를 주거나. */
void Clear_Overflow(BTreeNode ** root, BTreeNode * btNode, BTreeNode * right, const int key, int level)
{
	BTreeNode * parent;
	BTreeNode * newLeft;
	BTreeNode * newRight;
	BTreeNode * newRoot;
	int n;	// 스플릿 시, 상위 레벨에 전달한 key를 저장할 변수.
	int mid = KEYS_LENGTH / 2;
	
	if (btNode == *root && (*root)->numOfKeys == KEYS_LENGTH)// 루트 스플릿
	{
		newRoot = Make_BTNode();				//새로운 루트 생성
		newRight = Make_BTNode();				//새로운 오른쪽 생성
		n = Split_BTNode(btNode, newRight, right, key);	// 기존의 루트였던 btNode를 스플릿
		newLeft = btNode;						// 기존 루트였던 btNode가 지금부터 왼쪽
		newRoot->key[0] = n;		// 새로운 루트의 키값 셋팅
		newRoot->childPtr[0] = newLeft;			// 첫번째 자식 연결
		newRoot->childPtr[1] = newRight;		// 두번째 자식 연결
		newRoot->numOfKeys = 1;
		*root = newRoot;
	
		TREE_LEVEL += 1;							// 트리의 레벨 정보를 담고 있는 글로벌 변수의 값을 1 증가.
	}
	else 
	{
		parent = Find_BTNode(*root, key, level - 1); // btNode의 부모노드 찾기
		
		newRight = Make_BTNode();	
		n = Split_BTNode(btNode, newRight, right, key);	// 쪼개고, 스플릿된 newLeft, newRight 생성		

		if (parent->numOfKeys >= KEYS_LENGTH)	// 부모님이 여유가 없다니, 조부모님을 찾아가야겠군. 최악의 경우 시조까지 거슬러 올라간다.
			Clear_Overflow(root, parent, newRight, n, level - 1); // 부모를 대상으로 재귀호출
		else									
			Insert_Key(parent, newRight, n); // 부모 여유 있으니. 부모한테 n줌.		
	} // end if ~ else
}

/* 키 삽입시 최초로 호출되는 함수. */
int BTree_Insert(BTreeNode ** pRoot, const BTData key)
{
	BTreeNode * iNode;
	iNode = Find_BTNode(*pRoot, key, TREE_LEVEL);
	
	if (iNode == NULL)
		return FALSE;

	if (iNode->numOfKeys == KEYS_LENGTH)
		Clear_Overflow(pRoot, iNode, NULL, key, TREE_LEVEL);
	else
		Insert_Key(iNode, NULL, key);

	return TRUE;
}

/*왼쪽 노드의 키값 하나를 오른쪽 노드에게 줌*/
void Give_Key_To_Right(BTreeNode * parent, BTreeNode * btNode, const int idx)
{
	int i;
	for (i = parent->childPtr[idx + 1]->numOfKeys; i > 0; i--) {
		parent->childPtr[idx + 1]->key[i] = parent->childPtr[idx + 1]->key[i - 1]; 
		parent->childPtr[idx + 1]->childPtr[i + 1] = parent->childPtr[idx + 1]->childPtr[i];
	}
	parent->childPtr[idx + 1]->childPtr[i + 1] = parent->childPtr[idx + 1]->childPtr[i];

	parent->childPtr[idx + 1]->key[0] = parent->key[idx]; // 부모노드의 i번째를 오른쪽 형제의 첫번째 자식으로 
	parent->key[idx] = btNode->key[btNode->numOfKeys-1];    // 부모노드의 i번째 키값을 btNode의 마지막 키값으로.
	parent->childPtr[idx + 1]->childPtr[0] = btNode->childPtr[btNode->numOfKeys]; // btNode의 마지막 포인터를 오른쪽 형제 노드의 첫번째 포인터로.
	btNode->numOfKeys--;							// btNode의 키값 -1
	parent->childPtr[idx + 1]->numOfKeys++;			//오른쪽 형제의 키값 +1

	
}

/*오른쪽 노드의 키값 하나를 왼쪽 노드에게 줌*/
void Give_Key_To_Left(BTreeNode * parent, BTreeNode * btNode, const int idx)
{
	int i;
	parent->childPtr[idx - 1]->key[parent->childPtr[idx - 1]->numOfKeys] = parent->key[idx - 1]; //루트의 값을 왼쪽으로 내리고
	parent->key[idx - 1] = btNode->key[0]; //오른쪽의 값을 루트로 올리고
	parent->childPtr[idx - 1]->childPtr[parent->childPtr[idx - 1]->numOfKeys + 1] = btNode->childPtr[0]; // 오른쪽 첫번재 포인터를 왼쪽 마지막 포인터로.
	for (i = 0; i < btNode->numOfKeys; i++) // 오른쪽 키, 포인터 한칸씩 땡기고
	{
		btNode->key[i] = btNode->key[i + 1];
		btNode->childPtr[i] = btNode->childPtr[i + 1];
	}
	btNode->childPtr[i] = btNode->childPtr[i + 1]; //마지막 포인터도 한칸 땡기고
	parent->childPtr[idx - 1]->numOfKeys++;
	btNode->numOfKeys--;

}

/* 언더플로우가 발생했을 때, 형제에게 키값을 받는 함수 */
int Borrow_Key_From_Sibling(BTreeNode * parent, BTreeNode * btNode)
{
	int i;
	for (i = 0; (i < parent->numOfKeys + 1) && (parent->childPtr[i] != btNode); i++); // parent가 btNode를 가리키는 차일드 포인터 인덱스를 찾음.

	if (i == 0 && (parent->childPtr[i + 1]->numOfKeys >= MINIMIN_NUM_OF_KEYS + 1))// btNode가 첫번째 자식이라면, 오른쪽 형제만 있으므로, 오른쪽이에게만 안부를 묻는다.
	{
		Give_Key_To_Left(parent, parent->childPtr[1], i + 1);
		return TRUE;		
	}
	else if ((i == parent->numOfKeys) && (parent->childPtr[i - 1]->numOfKeys >= MINIMIN_NUM_OF_KEYS + 1))
	{
		Give_Key_To_Right(parent, parent->childPtr[i - 1], i - 1);
		return TRUE;
	}
	else if(i != 0 && i != parent->numOfKeys )
	{
		if (parent->childPtr[i + 1]->numOfKeys >= MINIMIN_NUM_OF_KEYS + 1) {
			Give_Key_To_Left(parent, parent->childPtr[i + 1], i + 1);
			return TRUE;
		}
		else if (parent->childPtr[i - 1]->numOfKeys >= MINIMIN_NUM_OF_KEYS + 1) {
			Give_Key_To_Right(parent, parent->childPtr[i - 1], i - 1);
			return TRUE;
		}
	}

	return FALSE;
}

/* 두 노드를 병합. */
BTreeNode* Merge_Two_Nodes(BTreeNode * parent, BTreeNode * btNode)
{
	BTreeNode * left;
	BTreeNode * right;
	int mid = KEYS_LENGTH / 2;
	int i, j;

	for (i = 0; (i < parent->numOfKeys + 1) && (parent->childPtr[i] != btNode); i++); // parent가 btNode를 가리키는 차일드 포인터 인덱스를 찾음.

	if (i == parent->numOfKeys) // btNode가 마지막 자식이라면 
	{
		left = parent->childPtr[i - 1];
		right = btNode;
		i -= 1;
	}
	else
	{
		left = parent->childPtr[i];
		right = parent->childPtr[i + 1];
	}

	left->key[left->numOfKeys] = parent->key[i]; //부모노드 키값 하나를 left의 현재 마지막 키로.
	left->numOfKeys++;

	for (j = 0; j < right->numOfKeys; j++)	// 오른쪽의 키와 포인터를 왼쪽에 삽입.
	{
		left->key[left->numOfKeys] = right->key[j];	// j+mid+1 로 바꿔야함.
		left->childPtr[left->numOfKeys] = right->childPtr[j]; //j+mid+1 로 바꺼야함
		left->numOfKeys++;
	}
	left->childPtr[left->numOfKeys] = right->childPtr[j];

	if (i + 1 == parent->numOfKeys)
		parent->numOfKeys--;
	else 
	{
		for (; i < parent->numOfKeys - 1; i++)
		{
			parent->key[i] = parent->key[i + 1];
			parent->childPtr[i + 1] = parent->childPtr[i + 2];
		}
		parent->numOfKeys--;
	}

	free(right);
	assert(left->numOfKeys >= MINIMIN_NUM_OF_KEYS);
	return left;
}

/* 노드에서 하나의 키를 삭제하고, 언더플로우가 발생했을 때, 이를 해결 */
void Clear_Underflow(BTreeNode ** pRoot, BTreeNode * btNode, int level)
{
	BTreeNode * parent;


	parent = Find_BTNode(*pRoot, btNode->key[0], level - 1); // 부모노드 찾기

	if (Borrow_Key_From_Sibling(parent, btNode)) // 형제노드가 여유가 있다면 하나 가져오고 함수 종료.
		return;

	/* 부모 노드가 루트이고, 키 개수가 1개 있을때
	   루트, 왼쪽, 오른쪽 노드를 합치고 트리의 레벨을 1 낮춤*/
	if (parent == *pRoot && parent->numOfKeys == 1)
	{
		*pRoot = Merge_Two_Nodes(parent, btNode);
		TREE_LEVEL--;
		return;
	}

	Merge_Two_Nodes(parent, btNode); // 병합

	/* 부모노드의 키 개수가 2/m가 안되고 인터널 노드일 때, 재귀호출. */
	if(parent->numOfKeys < MINIMIN_NUM_OF_KEYS && parent != *pRoot)
		Clear_Underflow(pRoot, parent, level - 1);
}

/* dNode 내에 key가 있다면 TRUE, 아니면 FALSE 반환 */
int Find_Same_Key(const BTreeNode * dNode, const BTData key)
{
	int i;
	for (i = 0; i < dNode->numOfKeys; i++)
		if (dNode->key[i] == key)	return TRUE;

	return FALSE;
}

/* 트리의 루트부터 시작해서 key를 갖고 있는 노드를 찾아감.
   최종적으로, 탐색에 성공하였을 시,
   인자로 전달받는 더블 포인터 dNode가 key를 갖는 노드를 가리키게 되고
   해당 노드가 인터널노드라면 0을, 리프라면 1을 반환함.
   탐색에 실패했을 때는 -1을 반환.*/
int Find_DNode(const BTreeNode ** dNode, const BTData key)
{
	int i, j;

	for (i = 0; i < TREE_LEVEL; i++)
	{

		if (Find_Same_Key(*dNode, key)) // key가 있는 노드를 찾음.
		{

			if (i < TREE_LEVEL) // dNode가 인터널이라면 0 반환
				return 0;
			else                // dNode가 리프라면 1 반환
				return 1;		 
		
		}
		else
		{
			j = 0;

			while (j < (*dNode)->numOfKeys && key > (*dNode)->key[j])
				j++;

			*dNode = (*dNode)->childPtr[j];
		}

	}// end for
	if (Find_Same_Key(*dNode, key)) 	
		return 1;

	return -1; // key가 트리 내에 없으면 -1 반환.
}

/* 비트리 내에서 키를 삭제할 때 최초로 호출되는 함수. */
int BTree_Delete(BTreeNode ** pRoot, const BTData key)
{
	BTreeNode * dNode = *pRoot;
	BTreeNode * rNode;
	int i, j, n, temp;

	if((n = Find_DNode(&dNode, key)) == -1)
		return FALSE;	// 찾는 키가 트리에 없음.
	
	rNode = dNode;

	if (n == 0) //dNode가 인터널 노드라면
	{
		/* key 보다 큰 모든 수들 중 가장 작은 key를 갖고 있는 리프 노드를 찾음. */
		i = 0;
		while (i < rNode->numOfKeys && key != rNode->key[i])
			i++;
		rNode = rNode->childPtr[i + 1]; // i는 rNode내에서 key가 있는 인덱스.

		while (rNode->childPtr[0] != NULL)	// 리프를 찾음.
			rNode = rNode->childPtr[0];

		/* key와 직후원소의 자리를 바꿈. */
		temp = dNode->key[i];
		dNode->key[i] = rNode->key[0];
		rNode->key[0] = temp;
	}
	
	/* rNode에서 key를 제거. 이때 rNode는 무조건 리프노드를 참조함. */
		
	for (i = 0; i < rNode->numOfKeys; i++)
	{
		if (rNode->key[i] == key)
		{
			for (j = i; j < rNode->numOfKeys - 1; j++)
				rNode->key[j] = rNode->key[j + 1];
			rNode->numOfKeys--;
			break;
		}
	}

	/* rNode에서 key 제거 후, 키의 개수가 2/m가 아니된다면, Clear_Underflow 호출. */
	if (rNode->numOfKeys < MINIMIN_NUM_OF_KEYS)
		Clear_Underflow(pRoot, rNode, TREE_LEVEL);

	return TRUE;
}

/*트리 내에서 key를 찾는 함수.
  탐색 경로를 출력해줌. 탐색에 성공하면 TRUE를, 실패하면 FALSE를 반환. */
int BTree_Search(BTreeNode ** pRoot, const BTData key)
{
	BTreeNode * curNode = *pRoot;
	int i, j;

	for (i = 0; i < TREE_LEVEL; i++)
	{
		for (int j = 0; j < i; j++)
			fputs("    ", stdout);
		printf("[%d] ", i + 1);

		for (j = 0; j < curNode->numOfKeys; j++)
		{
			if (curNode->key[j] == key)
			{
				printf("*%d* \n", curNode->key[j]);
				return TRUE;
			}
			printf("%d ", curNode->key[j]);
		}
		puts("\n");

		j = 0;

		while (j < curNode->numOfKeys && key > curNode->key[j])
			j++;

		curNode = (curNode)->childPtr[j];

	}// end for

	for (int j = 0; j < i; j++)
		fputs("    ", stdout);
	printf("[%d] ", i + 1);

	for (j = 0; j < curNode->numOfKeys; j++)
	{
		if (curNode->key[j] == key)
		{
			printf("*%d* \n", curNode->key[j]);
			return TRUE;
		}
		printf("%d ", curNode->key[j]);
	}
	puts("\n");

	return FALSE; // key가 트리 내에 없으면 -1 반환.
}

/* 트리내의 모든 컨텐츠를 출력. */
void ShowAll(BTreeNode * btNode, int level)
{
	if (level > TREE_LEVEL)
		return;

	/* 공백 출력 */
	for (int i = 0; i < level * 4; i++)
		fputs(" ", stdout);
	printf("[%d] ", level + 1);

	/* 노드 내의 데이터 출력 */
	for (int i = 0; i < btNode->numOfKeys; i++)
		printf("%d ", btNode->key[i]);
	puts("\n");

	/* 왼쪽 차일드부터 순차적으로다가 호출 */
	for (int i = 0; i < btNode->numOfKeys + 1; i++)
		if (btNode->childPtr[i] != NULL) {
			ShowAll(btNode->childPtr[i], level + 1);
		}
}