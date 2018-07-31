#pragma once
#include <queue>

namespace CR
{
	//B树节点结构体
	template<class T>
	struct BTreeNode
	{
		//已存储信息的数目
		int size;
		//储存信息数组
		T * keys;
		//父节点指针
		BTreeNode<T> * parent;
		//子节点指针数组
		BTreeNode<T>  ** childs;

		BTreeNode<T>() : size(0) {}
		BTreeNode<T>(BTreeNode <T> * bParent) : parent(bParent), size(0) {}
		BTreeNode<T>(BTreeNode <T> * bParent, int rank) : parent(bParent), size(0),keys(new T[rank + 1]),childs(new BTreeNode<T> *[rank + 1])
		{
			for (int i = 0; i <= rank; i++)
			{
				childs[i] = nullptr;
			}
		}
		~BTreeNode<T>()
		{
			delete[] keys;
			delete[] childs;
		}
	};

	//返回值结构体
	template<class T>
	struct Result
	{
		//返回地址
		BTreeNode<T> * ptr;
		//储存信息数组下标
		int sub;
		Result()
		{
			ptr = nullptr;
		}
	};

	//B树模版类
	template<class T>
	class BalanceTree
	{
	private:
		//B树的阶
		const int m_BRank;
		BTreeNode<T> * m_BTree;

	private:
		//返回子节点对应父节点的孩子数组下标
		int CountChild(BTreeNode<T> * bTree);
		//节点的分裂
		void Split(BTreeNode<T> * & bTree);
		//辅助销毁
		void DestroyHelper(BTreeNode<T> * & bTree);

	public:
		BalanceTree(int rank);
		~BalanceTree();
		//字典法搜索关键字
		bool Search(T key, Result<T> & result);
		//插入关键字
		bool Insert(T key);
		//删除关键字
		bool Delete(T key);
		//输出B树
		void Show();
		//销毁B树
		void Destroy();
	};

	template<class T>
	inline int BalanceTree<T>::CountChild(BTreeNode<T> * bTree)
	{
		BTreeNode<T> * parent = bTree->parent;
		if (parent)
		{
			int i = 0;
			while (true)
			{
				if (parent->childs[i] == bTree)
				{
					return i;
				}
				i++;
			}
		}
		else
		{
			return -1;
		}
	}

	template<class T>
	inline void BalanceTree<T>::Split(BTreeNode<T> * & bTree)
	{
		if (bTree->size == m_BRank)
		{
			int mid = m_BRank / 2;
			int newNodeSize = m_BRank - mid - 1;
			BTreeNode<T> * parent = bTree->parent;
			//如果不是树顶
			if (parent)
			{
				int count = CountChild(bTree);
				BTreeNode<T> * newPtr = new BTreeNode<T>(parent, m_BRank);				
				for (int i = m_BRank; i > count; i--)
				{
					parent->childs[i] = parent->childs[i - 1];
					parent->keys[i - 1] = parent->keys[i - 2];
				}
				parent->keys[count] = bTree->keys[mid];
				//把新节点接在插入点右边
				parent->childs[count + 1] = newPtr;
				newPtr->size = newNodeSize;
				//mid后面的数据转移到新节点
				memmove_s(newPtr->keys, sizeof(T) * newNodeSize, bTree->keys + mid + 1, sizeof(T) * newNodeSize);
				memcpy_s(newPtr->childs, sizeof(BTreeNode<T> *)* (newNodeSize + 1), bTree->childs + mid + 1, sizeof(BTreeNode<T> *)* (newNodeSize + 1));
				for (int i = mid + 1; i <= m_BRank; i++)
				{
					bTree->childs[i]->parent = newPtr;
					bTree->childs[i] = nullptr;
				}
				bTree->size = mid;
				parent->size++;
				return Split(parent);
			}
			//树顶
			else
			{
				bTree->parent = new BTreeNode<T>(nullptr, m_BRank);
				parent = bTree->parent;
				parent->keys[0] = bTree->keys[mid];
				parent->childs[0] = bTree;
				parent->childs[1] = new BTreeNode<T>(parent, m_BRank);
				BTreeNode<T> * newPtr = parent->childs[1];
				parent->size = 1;	
				newPtr->size = newNodeSize;
				memmove_s(newPtr->keys, sizeof(T) * newNodeSize, bTree->keys + mid + 1, sizeof(T) * newNodeSize);
				memcpy_s(newPtr->childs, sizeof(BTreeNode<T> *) * (newNodeSize + 1), bTree->childs + mid + 1, sizeof(BTreeNode<T> *) * (newNodeSize + 1));
				for (int i = mid + 1; i <= m_BRank; i++)
				{
					bTree->childs[i]->parent = newPtr;
					bTree->childs[i] = nullptr;
				}
				bTree->size = mid;
				m_BTree = parent;
			}
		}
	}

	template<class T>
	inline void BalanceTree<T>::DestroyHelper(BTreeNode<T>*& bTree)
	{
		if (bTree)
		{
			for (int i = 0; i <= m_BRank; i++)
			{
				DestroyHelper(bTree->childs[i]);
			}
			delete bTree;
		}
	}

	template<class T>
	inline BalanceTree<T>::BalanceTree(int rank):m_BRank(rank)
	{
		m_BTree = new BTreeNode<T>(nullptr, rank);
	}

	template<class T>
	inline BalanceTree<T>::~BalanceTree()
	{
		Destroy();
	}

	template<class T>
	inline bool BalanceTree<T>::Search(T key, Result<T> & result)
	{
		BTreeNode <T> * node = m_BTree;
		while (node)
		{
			if (!node->size)
			{
				result.ptr = node;
				break;
			}
			int i = 0;
			for (i; i < node->size; i++)
			{
				if (node->keys[i] == key)
				{
					result.ptr = node;
					result.sub = i;
					return true;
				}
				if (node->keys[i] > key)
				{
					break;
				}
			}
			node = node->childs[i];
		}
		return false;
	}

	template<class T>
	inline bool BalanceTree<T>::Insert(T key)
	{		
		Result<T> result;
		//先判断树内是否已经有该关键字
		if (Search(key, result))
		{
			return false;
		}
		//插入点
		BTreeNode<T> * node = nullptr;			
		//如果不是树顶
		if (result.ptr->parent)
		{
			node = result.ptr->parent;
		}
		else
		{
			node = result.ptr;
			if (!node->size)
			{
				//先创建叶子节点
				result.ptr->childs[0] = new BTreeNode<T>(result.ptr, m_BRank);
				result.ptr->childs[1] = new BTreeNode<T>(result.ptr, m_BRank);
				result.ptr->size = 1;
				result.ptr->keys[0] = key;
				return true;
			}
		}
		int count = CountChild(result.ptr);
		//先把父节点的数据右移1位
		for (int i = node->size; i > count; i--)
		{
			node->keys[i] = node->keys[i - 1];
		}
		//叶子节点 ++
		node->size++;
		node->childs[node->size] = new BTreeNode<T>(node, m_BRank);
		node->keys[count] = key;
		Split(node);
		return true;
	}

	template<class T>
	inline bool BalanceTree<T>::Delete(T key)
	{
		Result<T> result;
		//如果关键字存在
		if (Search(key, result))
		{
			//不是最底层节点
			if (result.ptr->childs[result.sub]->size)
			{

			}
			//是最底层节点
			else
			{

			}
			return true;
		}
		return false;
	}

	template<class T>
	inline void BalanceTree<T>::Show()
	{
		std::queue<BTreeNode<T> *> bQueue;
		std::queue<bool> bMark;
		bQueue.push(m_BTree);
		bMark.push(true);
		while (!bQueue.empty())
		{
			BTreeNode<T> * node = bQueue.front();
			int mark = bMark.front();
			bQueue.pop();
			bMark.pop();
			if (!node)
			{
				continue;
			}
			int i = 0;
			for (i; i < node->size; i++)
			{
				std::cout << node->keys[i] << "  ";
				bQueue.push(node->childs[i]);
				bMark.push(false);
			}
			bQueue.push(node->childs[i]);
			bMark.push(mark);
			std::cout << '|';
			if (mark)
			{
				std::cout << std::endl;
			}
		}
	}

	template<class T>
	inline void BalanceTree<T>::Destroy()
	{
		DestroyHelper(m_BTree);
	}
}