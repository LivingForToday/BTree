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

/* btNode�� key�� �����ϰ�, key�� �¿쿡 left, rifht ����. */
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

/* ��Ʈ���� �����Ͽ� level���� key�� ã�ư��� ��� �� �ִ� ��� or ���� ��� ��ȯ 
   Ư�� ����� �θ� ��带 ã�ų�, Ư�� ���� ��带 ã�µ��� ��� ��. */
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

/* ��带 left, rifht�� ����, �������� 2/m �ȵɽ�, ��й����. */
int Split_BTNode(BTreeNode * left, BTreeNode * right, BTreeNode * subRight, const int key)
{
	/*left right���ø��ϰ�, key�� ������ ���� �ְ�, ���� ������ �����ؾ� �� key���� ��ȯ. */

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

												   // key�� left or right�� ����.
	if (left->numOfKeys > MINIMIN_NUM_OF_KEYS) //right�� ���ԵǴ� ���
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
	else // left�� key�� ���ԵǴ� ���
	{
		if (left->key[left->numOfKeys - 1] < key) //���ڷ� ���� Ű�� left�� ������ Ű������ Ŭ ���.
		{
			right->childPtr[0] = subRight;		//�������� ù��° �����͸� subRight��.
			left->key[mid] = key;				//����Ʈ�� �������� key �߰�.
		}
		else
		{
			left->key[mid] = left->key[mid - 1]; // �ϴ� ������ ���� ������ Ű�� mid��°�� ��.

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

/* key���Խ�, ��尡 �� á�� ��, ȣ��.
   ��쿡 ���� ���ø� �ϰų�, �������� Ű�� �ְų�. */
void Clear_Overflow(BTreeNode ** root, BTreeNode * btNode, BTreeNode * right, const int key, int level)
{
	BTreeNode * parent;
	BTreeNode * newLeft;
	BTreeNode * newRight;
	BTreeNode * newRoot;
	int n;	// ���ø� ��, ���� ������ ������ key�� ������ ����.
	int mid = KEYS_LENGTH / 2;
	
	if (btNode == *root && (*root)->numOfKeys == KEYS_LENGTH)// ��Ʈ ���ø�
	{
		newRoot = Make_BTNode();				//���ο� ��Ʈ ����
		newRight = Make_BTNode();				//���ο� ������ ����
		n = Split_BTNode(btNode, newRight, right, key);	// ������ ��Ʈ���� btNode�� ���ø�
		newLeft = btNode;						// ���� ��Ʈ���� btNode�� ���ݺ��� ����
		newRoot->key[0] = n;		// ���ο� ��Ʈ�� Ű�� ����
		newRoot->childPtr[0] = newLeft;			// ù��° �ڽ� ����
		newRoot->childPtr[1] = newRight;		// �ι�° �ڽ� ����
		newRoot->numOfKeys = 1;
		*root = newRoot;
	
		TREE_LEVEL += 1;							// Ʈ���� ���� ������ ��� �ִ� �۷ι� ������ ���� 1 ����.
	}
	else 
	{
		parent = Find_BTNode(*root, key, level - 1); // btNode�� �θ��� ã��
		
		newRight = Make_BTNode();	
		n = Split_BTNode(btNode, newRight, right, key);	// �ɰ���, ���ø��� newLeft, newRight ����		

		if (parent->numOfKeys >= KEYS_LENGTH)	// �θ���� ������ ���ٴ�, ���θ���� ã�ư��߰ڱ�. �־��� ��� �������� �Ž��� �ö󰣴�.
			Clear_Overflow(root, parent, newRight, n, level - 1); // �θ� ������� ���ȣ��
		else									
			Insert_Key(parent, newRight, n); // �θ� ���� ������. �θ����� n��.		
	} // end if ~ else
}

/* Ű ���Խ� ���ʷ� ȣ��Ǵ� �Լ�. */
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

/*���� ����� Ű�� �ϳ��� ������ ��忡�� ��*/
void Give_Key_To_Right(BTreeNode * parent, BTreeNode * btNode, const int idx)
{
	int i;
	for (i = parent->childPtr[idx + 1]->numOfKeys; i > 0; i--) {
		parent->childPtr[idx + 1]->key[i] = parent->childPtr[idx + 1]->key[i - 1]; 
		parent->childPtr[idx + 1]->childPtr[i + 1] = parent->childPtr[idx + 1]->childPtr[i];
	}
	parent->childPtr[idx + 1]->childPtr[i + 1] = parent->childPtr[idx + 1]->childPtr[i];

	parent->childPtr[idx + 1]->key[0] = parent->key[idx]; // �θ����� i��°�� ������ ������ ù��° �ڽ����� 
	parent->key[idx] = btNode->key[btNode->numOfKeys-1];    // �θ����� i��° Ű���� btNode�� ������ Ű������.
	parent->childPtr[idx + 1]->childPtr[0] = btNode->childPtr[btNode->numOfKeys]; // btNode�� ������ �����͸� ������ ���� ����� ù��° �����ͷ�.
	btNode->numOfKeys--;							// btNode�� Ű�� -1
	parent->childPtr[idx + 1]->numOfKeys++;			//������ ������ Ű�� +1

	
}

/*������ ����� Ű�� �ϳ��� ���� ��忡�� ��*/
void Give_Key_To_Left(BTreeNode * parent, BTreeNode * btNode, const int idx)
{
	int i;
	parent->childPtr[idx - 1]->key[parent->childPtr[idx - 1]->numOfKeys] = parent->key[idx - 1]; //��Ʈ�� ���� �������� ������
	parent->key[idx - 1] = btNode->key[0]; //�������� ���� ��Ʈ�� �ø���
	parent->childPtr[idx - 1]->childPtr[parent->childPtr[idx - 1]->numOfKeys + 1] = btNode->childPtr[0]; // ������ ù���� �����͸� ���� ������ �����ͷ�.
	for (i = 0; i < btNode->numOfKeys; i++) // ������ Ű, ������ ��ĭ�� �����
	{
		btNode->key[i] = btNode->key[i + 1];
		btNode->childPtr[i] = btNode->childPtr[i + 1];
	}
	btNode->childPtr[i] = btNode->childPtr[i + 1]; //������ �����͵� ��ĭ �����
	parent->childPtr[idx - 1]->numOfKeys++;
	btNode->numOfKeys--;

}

/* ����÷ο찡 �߻����� ��, �������� Ű���� �޴� �Լ� */
int Borrow_Key_From_Sibling(BTreeNode * parent, BTreeNode * btNode)
{
	int i;
	for (i = 0; (i < parent->numOfKeys + 1) && (parent->childPtr[i] != btNode); i++); // parent�� btNode�� ����Ű�� ���ϵ� ������ �ε����� ã��.

	if (i == 0 && (parent->childPtr[i + 1]->numOfKeys >= MINIMIN_NUM_OF_KEYS + 1))// btNode�� ù��° �ڽ��̶��, ������ ������ �����Ƿ�, �������̿��Ը� �Ⱥθ� ���´�.
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

/* �� ��带 ����. */
BTreeNode* Merge_Two_Nodes(BTreeNode * parent, BTreeNode * btNode)
{
	BTreeNode * left;
	BTreeNode * right;
	int mid = KEYS_LENGTH / 2;
	int i, j;

	for (i = 0; (i < parent->numOfKeys + 1) && (parent->childPtr[i] != btNode); i++); // parent�� btNode�� ����Ű�� ���ϵ� ������ �ε����� ã��.

	if (i == parent->numOfKeys) // btNode�� ������ �ڽ��̶�� 
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

	left->key[left->numOfKeys] = parent->key[i]; //�θ��� Ű�� �ϳ��� left�� ���� ������ Ű��.
	left->numOfKeys++;

	for (j = 0; j < right->numOfKeys; j++)	// �������� Ű�� �����͸� ���ʿ� ����.
	{
		left->key[left->numOfKeys] = right->key[j];	// j+mid+1 �� �ٲ����.
		left->childPtr[left->numOfKeys] = right->childPtr[j]; //j+mid+1 �� �ٲ�����
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

/* ��忡�� �ϳ��� Ű�� �����ϰ�, ����÷ο찡 �߻����� ��, �̸� �ذ� */
void Clear_Underflow(BTreeNode ** pRoot, BTreeNode * btNode, int level)
{
	BTreeNode * parent;


	parent = Find_BTNode(*pRoot, btNode->key[0], level - 1); // �θ��� ã��

	if (Borrow_Key_From_Sibling(parent, btNode)) // ������尡 ������ �ִٸ� �ϳ� �������� �Լ� ����.
		return;

	/* �θ� ��尡 ��Ʈ�̰�, Ű ������ 1�� ������
	   ��Ʈ, ����, ������ ��带 ��ġ�� Ʈ���� ������ 1 ����*/
	if (parent == *pRoot && parent->numOfKeys == 1)
	{
		*pRoot = Merge_Two_Nodes(parent, btNode);
		TREE_LEVEL--;
		return;
	}

	Merge_Two_Nodes(parent, btNode); // ����

	/* �θ����� Ű ������ 2/m�� �ȵǰ� ���ͳ� ����� ��, ���ȣ��. */
	if(parent->numOfKeys < MINIMIN_NUM_OF_KEYS && parent != *pRoot)
		Clear_Underflow(pRoot, parent, level - 1);
}

/* dNode ���� key�� �ִٸ� TRUE, �ƴϸ� FALSE ��ȯ */
int Find_Same_Key(const BTreeNode * dNode, const BTData key)
{
	int i;
	for (i = 0; i < dNode->numOfKeys; i++)
		if (dNode->key[i] == key)	return TRUE;

	return FALSE;
}

/* Ʈ���� ��Ʈ���� �����ؼ� key�� ���� �ִ� ��带 ã�ư�.
   ����������, Ž���� �����Ͽ��� ��,
   ���ڷ� ���޹޴� ���� ������ dNode�� key�� ���� ��带 ����Ű�� �ǰ�
   �ش� ��尡 ���ͳγ���� 0��, ������� 1�� ��ȯ��.
   Ž���� �������� ���� -1�� ��ȯ.*/
int Find_DNode(const BTreeNode ** dNode, const BTData key)
{
	int i, j;

	for (i = 0; i < TREE_LEVEL; i++)
	{

		if (Find_Same_Key(*dNode, key)) // key�� �ִ� ��带 ã��.
		{

			if (i < TREE_LEVEL) // dNode�� ���ͳ��̶�� 0 ��ȯ
				return 0;
			else                // dNode�� ������� 1 ��ȯ
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

	return -1; // key�� Ʈ�� ���� ������ -1 ��ȯ.
}

/* ��Ʈ�� ������ Ű�� ������ �� ���ʷ� ȣ��Ǵ� �Լ�. */
int BTree_Delete(BTreeNode ** pRoot, const BTData key)
{
	BTreeNode * dNode = *pRoot;
	BTreeNode * rNode;
	int i, j, n, temp;

	if((n = Find_DNode(&dNode, key)) == -1)
		return FALSE;	// ã�� Ű�� Ʈ���� ����.
	
	rNode = dNode;

	if (n == 0) //dNode�� ���ͳ� �����
	{
		/* key ���� ū ��� ���� �� ���� ���� key�� ���� �ִ� ���� ��带 ã��. */
		i = 0;
		while (i < rNode->numOfKeys && key != rNode->key[i])
			i++;
		rNode = rNode->childPtr[i + 1]; // i�� rNode������ key�� �ִ� �ε���.

		while (rNode->childPtr[0] != NULL)	// ������ ã��.
			rNode = rNode->childPtr[0];

		/* key�� ���Ŀ����� �ڸ��� �ٲ�. */
		temp = dNode->key[i];
		dNode->key[i] = rNode->key[0];
		rNode->key[0] = temp;
	}
	
	/* rNode���� key�� ����. �̶� rNode�� ������ ������带 ������. */
		
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

	/* rNode���� key ���� ��, Ű�� ������ 2/m�� �ƴϵȴٸ�, Clear_Underflow ȣ��. */
	if (rNode->numOfKeys < MINIMIN_NUM_OF_KEYS)
		Clear_Underflow(pRoot, rNode, TREE_LEVEL);

	return TRUE;
}

/*Ʈ�� ������ key�� ã�� �Լ�.
  Ž�� ��θ� �������. Ž���� �����ϸ� TRUE��, �����ϸ� FALSE�� ��ȯ. */
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

	return FALSE; // key�� Ʈ�� ���� ������ -1 ��ȯ.
}

/* Ʈ������ ��� �������� ���. */
void ShowAll(BTreeNode * btNode, int level)
{
	if (level > TREE_LEVEL)
		return;

	/* ���� ��� */
	for (int i = 0; i < level * 4; i++)
		fputs(" ", stdout);
	printf("[%d] ", level + 1);

	/* ��� ���� ������ ��� */
	for (int i = 0; i < btNode->numOfKeys; i++)
		printf("%d ", btNode->key[i]);
	puts("\n");

	/* ���� ���ϵ���� ���������δٰ� ȣ�� */
	for (int i = 0; i < btNode->numOfKeys + 1; i++)
		if (btNode->childPtr[i] != NULL) {
			ShowAll(btNode->childPtr[i], level + 1);
		}
}