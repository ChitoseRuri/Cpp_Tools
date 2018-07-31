#include "stdafx.h"

int main()
{
	{
		//system("color F0");
		CR::BalanceTree<int> bTree(3);
		bTree.Insert(5);
		bTree.Insert(7);
		bTree.Insert(10);
		bTree.Insert(3);
		bTree.Insert(1);
		bTree.Insert(2);
		bTree.Insert(4);
		bTree.Insert(0);
		bTree.Insert(11);
		bTree.Insert(12);
		bTree.Insert(13);
		bTree.Insert(14);
		bTree.Show();
	}
	_CrtDumpMemoryLeaks();
	return 0;
}