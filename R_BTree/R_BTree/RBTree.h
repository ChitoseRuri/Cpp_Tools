#pragma once
#if defined(_DEBUG)|defined(DEBUG)
#else
#define NDEBUG
#endif // defined(_Debug)|defined(Debug)
#include <assert.h>

template<typename T>
class RBTree
{
private:
	enum class Color
	{
		red = 1,
		black
	}
	
	template<typename T>
	struct Node<T>
	{
		Color color;
		T data;
		Node * left, * right;
	};

public:
	RBTree();
	~RBTree();
};