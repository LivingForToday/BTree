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

/* Insert key value into certain btNode and connect right child node.
   Left child is already connected to btNode or ,if btNode is root, the value is null, 
   so we don't have to connect btNode with left child. */
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

}

/* Find the node into which the key value must be inserted and return the node, 
   if this function would find it successfully.
   Another way this function is used is that when we insert or delete a key value, 
   the operation would cause a node to overflow or to underflow.
   In this case, we must find the parent node. Anyway, this function is used to find a certain node.*/
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

/* when we insert a key value into tree, the insertion operation would make some node
   overflowed - a node must have at most m keys, for m = the maximum number of keys in a node.
   If an overflow occurs in a node, this function splits the node and inserts the key value into appopriate node, left or right. */
int Split_BTNode(BTreeNode * left, BTreeNode * right, BTreeNode * subRight, const int key)
{

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
	right->childPtr[rightIdx] = left->childPtr[i];
	

	if (left->numOfKeys > MINIMIN_NUM_OF_KEYS) 
	{
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
	}
	else 
	{
		if (left->key[left->numOfKeys - 1] < key) 
		{
			right->childPtr[0] = subRight;	
			left->key[mid] = key;	
		}
		else
		{
			left->key[mid] = left->key[mid - 1]; 

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


	left->numOfKeys = mid;
	right->numOfKeys = mid;

	return left->key[mid];
}

/* this function resolves an overflow caused by insertion operation. */
void Clear_Overflow(BTreeNode ** root, BTreeNode * btNode, BTreeNode * right, const int key, int level)
{
	BTreeNode * parent;
	BTreeNode * newLeft;
	BTreeNode * newRight;
	BTreeNode * newRoot;
	int n;	
	int mid = KEYS_LENGTH / 2;
	
	if (btNode == *root && (*root)->numOfKeys == KEYS_LENGTH) // if the btNode is root of the tree
	{
		newRoot = Make_BTNode();				
		newRight = Make_BTNode();				
		n = Split_BTNode(btNode, newRight, right, key);	
		newLeft = btNode;				
		newRoot->key[0] = n;		
		newRoot->childPtr[0] = newLeft;			
		newRoot->childPtr[1] = newRight;		
		newRoot->numOfKeys = 1;
		*root = newRoot;
	
		TREE_LEVEL += 1;	
	}
	else 
	{
		parent = Find_BTNode(*root, key, level - 1); // find the parent of btNode.
		
		newRight = Make_BTNode();	
		n = Split_BTNode(btNode, newRight, right, key);	// split the btNode		

		if (parent->numOfKeys >= KEYS_LENGTH)		
			Clear_Overflow(root, parent, newRight, n, level - 1); 
		else									
			Insert_Key(parent, newRight, n); 		
	} // end if ~ else
}

/* this function must be called initially, 
   when we insert kay value into tree.  */
int BTree_Insert(BTreeNode ** pRoot, const BTData key)
{
	BTreeNode * iNode;
	iNode = Find_BTNode(*pRoot, key, TREE_LEVEL);
	
	if (iNode == NULL)
		return FALSE;

	if (iNode->numOfKeys == KEYS_LENGTH) // an overflow occurs in the iNode.
		Clear_Overflow(pRoot, iNode, NULL, key, TREE_LEVEL);
	else
		Insert_Key(iNode, NULL, key);

	return TRUE;
}

/* when we delete a key value from tree, the deletion operation could cause an underflow.
   In this case, the node which be underflowed can bollow a key value from sibling,
   if sibling has over m/2 keys. */
void Give_Key_To_Right(BTreeNode * parent, BTreeNode * btNode, const int idx)
{
	int i;
	for (i = parent->childPtr[idx + 1]->numOfKeys; i > 0; i--) {
		parent->childPtr[idx + 1]->key[i] = parent->childPtr[idx + 1]->key[i - 1]; 
		parent->childPtr[idx + 1]->childPtr[i + 1] = parent->childPtr[idx + 1]->childPtr[i];
	}
	parent->childPtr[idx + 1]->childPtr[i + 1] = parent->childPtr[idx + 1]->childPtr[i];

	parent->childPtr[idx + 1]->key[0] = parent->key[idx]; 
	parent->key[idx] = btNode->key[btNode->numOfKeys-1];   
	parent->childPtr[idx + 1]->childPtr[0] = btNode->childPtr[btNode->numOfKeys]; 
	btNode->numOfKeys--;							
	parent->childPtr[idx + 1]->numOfKeys++;		

	
}

/* this function is functionally the same with the function above
   except for the sibling's position. */
void Give_Key_To_Left(BTreeNode * parent, BTreeNode * btNode, const int idx)
{
	int i;
	parent->childPtr[idx - 1]->key[parent->childPtr[idx - 1]->numOfKeys] = parent->key[idx - 1]; 
	parent->key[idx - 1] = btNode->key[0]; 
	parent->childPtr[idx - 1]->childPtr[parent->childPtr[idx - 1]->numOfKeys + 1] = btNode->childPtr[0]; 
	for (i = 0; i < btNode->numOfKeys; i++) 
	{
		btNode->key[i] = btNode->key[i + 1];
		btNode->childPtr[i] = btNode->childPtr[i + 1];
	}
	btNode->childPtr[i] = btNode->childPtr[i + 1]; 
	parent->childPtr[idx - 1]->numOfKeys++;
	btNode->numOfKeys--;

}

/* this function calls 'Give_Key_To_Left()' or 'Give_Key_To_Right()' */
int Borrow_Key_From_Sibling(BTreeNode * parent, BTreeNode * btNode)
{
	int i;
	for (i = 0; (i < parent->numOfKeys + 1) && (parent->childPtr[i] != btNode); i++); 

	if (i == 0 && (parent->childPtr[i + 1]->numOfKeys >= MINIMIN_NUM_OF_KEYS + 1))
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

/* if underflow occurs and the node which be underflowed cannot bollow key value from its siblings,
   we must merge two nodes. */
BTreeNode* Merge_Two_Nodes(BTreeNode * parent, BTreeNode * btNode)
{
	BTreeNode * left;
	BTreeNode * right;
	int mid = KEYS_LENGTH / 2;
	int i, j;

	for (i = 0; (i < parent->numOfKeys + 1) && (parent->childPtr[i] != btNode); i++); 

	if (i == parent->numOfKeys) 
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

	left->key[left->numOfKeys] = parent->key[i]; 
	left->numOfKeys++;

	for (j = 0; j < right->numOfKeys; j++)	
	{
		left->key[left->numOfKeys] = right->key[j];	
		left->childPtr[left->numOfKeys] = right->childPtr[j]; 
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


void Clear_Underflow(BTreeNode ** pRoot, BTreeNode * btNode, int level)
{
	BTreeNode * parent;


	parent = Find_BTNode(*pRoot, btNode->key[0], level - 1); 

	if (Borrow_Key_From_Sibling(parent, btNode)) 
		return;

	if (parent == *pRoot && parent->numOfKeys == 1)
	{
		*pRoot = Merge_Two_Nodes(parent, btNode);
		TREE_LEVEL--;
		return;
	}

	Merge_Two_Nodes(parent, btNode); // º´ÇÕ

	if(parent->numOfKeys < MINIMIN_NUM_OF_KEYS && parent != *pRoot)
		Clear_Underflow(pRoot, parent, level - 1);
}

/* if dNode has key, this function returns true. 
   if it doesn't, return false. */
int Find_Same_Key(const BTreeNode * dNode, const BTData key)
{
	int i;
	for (i = 0; i < dNode->numOfKeys; i++)
		if (dNode->key[i] == key)	return TRUE;

	return FALSE;
}

/* find the node which has the key value and return the node. 
   the key value, the 2nd parameter, will be removed from tree. */
int Find_DNode(const BTreeNode ** dNode, const BTData key)
{
	int i, j;

	for (i = 0; i < TREE_LEVEL; i++)
	{

		if (Find_Same_Key(*dNode, key)) 
		{

			if (i < TREE_LEVEL) 
				return 0;
			else                
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

	return -1; 
}

/* when we delete a key value from tree, 
   this function must be called initially. */
int BTree_Delete(BTreeNode ** pRoot, const BTData key)
{
	BTreeNode * dNode = *pRoot;
	BTreeNode * rNode;
	int i, j, n, temp;

	if((n = Find_DNode(&dNode, key)) == -1)
		return FALSE;	
	
	rNode = dNode;

	if (n == 0) 
	{
		i = 0;
		while (i < rNode->numOfKeys && key != rNode->key[i])
			i++;
		rNode = rNode->childPtr[i + 1]; 

		while (rNode->childPtr[0] != NULL)	
			rNode = rNode->childPtr[0];

		
		temp = dNode->key[i];
		dNode->key[i] = rNode->key[0];
		rNode->key[0] = temp;
	}
	
			
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

	if (rNode->numOfKeys < MINIMIN_NUM_OF_KEYS)
		Clear_Underflow(pRoot, rNode, TREE_LEVEL);

	return TRUE;
}

/* search a key value and show the path */
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

	return FALSE; 
}

/* show all key value in tree. */
void ShowAll(BTreeNode * btNode, int level)
{
	if (level > TREE_LEVEL)
		return;

	for (int i = 0; i < level * 4; i++)
		fputs(" ", stdout);
	printf("[%d] ", level + 1);

	for (int i = 0; i < btNode->numOfKeys; i++)
		printf("%d ", btNode->key[i]);
	puts("\n");

	for (int i = 0; i < btNode->numOfKeys + 1; i++)
		if (btNode->childPtr[i] != NULL) {
			ShowAll(btNode->childPtr[i], level + 1);
		}
}
