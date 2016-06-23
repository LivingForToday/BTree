#ifndef __B_TREE_H__
#define __B_TREE_H__

#define KEYS_LENGTH				4
#define MINIMIN_NUM_OF_KEYS		KEYS_LENGTH / 2
#define TRUE					1
#define FALSE					0

typedef int BTData;

typedef struct _bTreeNode
{
	BTData key[KEYS_LENGTH];
	struct _bTreeNode * childPtr[KEYS_LENGTH + 1];
	int numOfKeys;
} BTreeNode;

BTreeNode* Make_BTNode();
BTreeNode* BTree_Init();

/* 삽입에 사용되는 함수들 */
void Insert_Key(BTreeNode * btNode, BTreeNode * right, const BTData key);
BTreeNode* Find_BTNode(BTreeNode * root, const BTData data, const int level);
void Give_Key_To_Right(BTreeNode * parent, BTreeNode * btNode, const int idx);
void Give_Key_To_Left(BTreeNode * parent, BTreeNode * btNode, const int idx);
int Split_BTNode(BTreeNode * left, BTreeNode * right, BTreeNode * subRight, const int key);
void Clear_Overflow(BTreeNode ** root, BTreeNode * btNode, BTreeNode * right, const int key, int level);
int BTree_Insert(BTreeNode ** pRoot, const BTData key);

/* 삭제에 사용되는 함수들 */
void Give_Key_To_Right(BTreeNode * parent, BTreeNode * btNode, const int idx);
void Give_Key_To_Left(BTreeNode * parent, BTreeNode * btNode, const int idx);
int Borrow_Key_From_Sibling(BTreeNode * parent, BTreeNode * btNode);
BTreeNode* Merge_Two_Nodes(BTreeNode * parent, BTreeNode * btNode);
void Clear_Underflow(BTreeNode ** pRoot, BTreeNode * btNode, int level);
int Find_Same_Key(const BTreeNode * btNode, const BTData key);
int Find_DNode(const BTreeNode ** dNode, const BTData key);
int BTree_Delete(BTreeNode ** pRoot, const BTData key);

/*그 외, 검색과 출력 함수 */
int BTree_Search(BTreeNode ** pRoot, const BTData key);
void ShowAll(BTreeNode * btNode, int level);

#endif
