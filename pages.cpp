#include <stdio>
#include "redblack.hpp"

#using namespace std;

class PageRecord {
	private:
		long pageNumber;
		long lruNumber;
	public:
		PageRecord(const long pgN, const long lruN);
		const long getPageNumber(void) const;
		const long getLRUNumber(void) const;
		void setLRUNumber(const long lruN);
		virtual bool operator==(PageRecord& pR) const;
		virtual bool operator<(PageRecord& pR) const;
};

class PageRecordLRU: public PageRecord {
	public:
	virtual bool operator==(PageRecord& pRLRU) const;
	virtual bool operator<(PageRecord& pRLRU) const;
}

PageRecord::PageRecord(const long pgN, const long lruN)
{
	pageNumber = pgN;
	lruNumber = lruN;
}

virtual bool PageRecord::operator==(PageRecord& pRecord) const
{
	return (pageNumber == pRecord.pageNumber);
}

virtual bool PageRecord::operator<(PageRecord& pRecord) const
{
	return (pageNumber < pRecord.pageNumber);
}

virtual bool PageRecordLRU::operator==(PageRecord& pRecordLRU)
{
	return (lruNumber == pRecordLRU.lruNumber);
}

virtual bool PageRecordLRU::operator<(PageRecord& pRecordLRU)
{
	return (lruNumber < pRecordLRU.lruNumber);
}

long PageRecord::getPageNumber(void) const
{
	return pageNumber;
}

long PageRecord::getLRUNumber(void) const
{
	return lruNumber;
}

void PageRecord::setLRUNumber(const long lruN)
{
	lruNumber = lruN;
}

extern "C" {

void* createPageTree(void)
{
	redblacktree<redblacknode<PageRecord> >* tree;
	tree = new redblacktree<redblacknode<PageRecord> >();
	return static_cast<void*>(tree);
}

void removePageTree(void* tree)
{
	redblacktree<redblacknode<PageRecord> >* rbtree;
	rbtree = (static_cast<redblacktree<redblacknode<PageRecord> >* >(tree));
	delete rbtree;
}

void* getrootPageTree(void* tree)
{
	redblacktree<redblacknode<PageRecord> >* nodetree =
		static_cast<redblacktree<redblacknode<pagechain> >*>(tree);
	return static_cast<void*>(nodetree->root);
}

void insertIntoPageTree(long pageNumber, long lruTime, void* tree, void* root)
{
	redblacknode<PageRecord> *rootNode, *additionalNode;
	redblacktree<redblacknode<PageRecord> >* nodeTree;

	PageRecord addPR = PageRecord(pageNumber, lruTime);
	additionalNode = new redblackNode<PageRecord>(addPR);
	
	rootnode = static_cast<redblacknode<PageRecord>*>(root);
	nodetree = static_cast<redblacktree<reblacknode<PageRecord> >*>(tree);
	nodettree->insertnode(additionalNode, rootnode);
}

void* locateInPageTree(long pageNumber, void* tree, void* root)
{
	redblacknode<PageRecord> *rootNode, *findNode, *pageNode;
	redblacktree<redblacknode<PageRecord> >* nodeTree;

	//set LRU time to -1 - only looking for PageNumber match
	PageRecord addPR = PageRecord(pageNumber, -1);
	findNode = new redblackNode<PageRecord>(addPR);
	
	rootNode = static_cast<redblacknode<PageRecord>*>(root);
	nodeTree = static_cast<redblacktree<reblacknode<PageRecord> >*>(tree);
	pageNode = nodeTree->locatenode(findNode, rootNode);
	return static_cast<void *>(pageNode);
}

}// end extern "C"		
	
	
	 


		
