#include "preCompiler.h"
#include "RBTree.h"

int main()
{
	RBTree<int, int> rbt;
	rbt.insert({ 5,5 });
	rbt.insert({ 2,2});
	rbt.insert({ 3,3 });
	rbt.insert({ 1,1 });
	rbt.insert({ 4,4 });
	rbt.insert({ 9,9 });
	auto itr = rbt.find(9);
	cout << itr->value;
	_getch();
	return 0;
}
