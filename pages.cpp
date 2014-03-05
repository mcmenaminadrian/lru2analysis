#include <iostream>
#include <ctime>
#include <vector>
#include "pthread.h"
#include "redblack.hpp"
#include "threadhandler.h"

using namespace std;

class PageRecord {
	protected:
	long pageNumber;
	time_t lruNumber;
	public:
	PageRecord(const long pgN, const time_t lruN);
	const long getPageNumber(void) const;
	const time_t getLRUNumber(void) const;
	void setLRUNumber(const time_t lruN);
	virtual bool operator==(PageRecord& pR) const;
	virtual bool operator<(PageRecord& pR) const;
};

class PageRecordLRU {
	protected:
	time_t lruNumber;
	vector<long> pageNumbers;
	public:
	PageRecordLRU(const long pgN, const time_t lruN);
	vector<long>& getPageNumbers(void) const;
	void setPageNumbers(const vector<long> vectPN);
	const time_t getLRUNumber(void) const;
	void setLRUNumber(const time_t lruN);
	void addPage(const long pgN);
	virtual bool operator==(PageRecordLRU& pRLRU) const;
	virtual bool operator<(PageRecordLRU& pRLRU) const;
};

class PageRecordTree {
	public:
	redblacktree<redblacknode<PageRecord> >* pageRecordTree;
	redblacktree<redblacknode<PageRecordLRU> >* pageRecordLRUTree;
	pthread_mutex_t tree_lock;
	PageRecordTree(redblacktree<redblacknode<PageRecord> >* prTree,
		redblacktree<redblacknode<PageRecordLRU> >* prLRUTree);
};

PageRecordTree::PageRecordTree(redblacktree<redblacknode<PageRecord> >* prTree,
	redblacktree<redblacknode<PageRecordLRU> >* prLRUTree)
{
	pageRecordTree = prTree;
	pageRecordLRUTree = prLRUTree;
	pthread_mutex_init(&tree_lock, NULL);
}

PageRecord::PageRecord(const long pgN, const time_t lruN)
{
	pageNumber = pgN;
	lruNumber = lruN;
}

PageRecordLRU::PageRecordLRU(const long pgN, const time_t lruN)
{
	lruNumber = lruN;
	pageNumbers.push_back(pgN);
}

vector<long>& PageRecordLRU::getPageNumbers(void) const
{
	return pageNumbers;
}

void PageRecordLRU::setPageNumbers(const vector<long> pgNumbers)
{
	pageNumbers.resize(0);
	pageNumbers = pgNumbers;
}

const time_t PageRecordLRU::getLRUNumber(void) const
{
	return lruNumber;
}

void PageRecordLRU::setLRUNumber(const time_t lruN)
{
	lruNumber = lruN;
}

void PageRecordLRU::addPage(const long pgN)
{
	pageNumbers.push_back(pgN);
}

bool PageRecord::operator==(PageRecord& pRecord) const
{
	return (pageNumber == pRecord.getPageNumber());
}

bool PageRecord::operator<(PageRecord& pRecord) const
{
	return (pageNumber < pRecord.getPageNumber());
}

bool PageRecordLRU::operator==(PageRecordLRU& pRecordLRU) const
{
	return (lruNumber == pRecordLRU.getLRUNumber());
}

bool PageRecordLRU::operator<(PageRecordLRU& pRecordLRU) const
{
	return (lruNumber < pRecordLRU.getLRUNumber());
}

const long PageRecord::getPageNumber(void) const
{
	return pageNumber;
}

const time_t PageRecord::getLRUNumber(void) const
{
	return lruNumber;
}

void PageRecord::setLRUNumber(const time_t lruN)
{
	lruNumber = lruN;
}

redblacknode<PageRecord>* findPagePRInTree(redblacknode<PageRecord>* node, 
	redblacktree<redblacknode<PageRecord> > *tree)
{
	redblacknode<PageRecord> *root = tree->root;
	return (tree->locatenode(node, root));
}

redblacknode<PageRecordLRU>*
findPageLRUInTree(redblacknode<PageRecordLRU>* node,
	redblacktree<redblacknode<PageRecordLRU> > *tree)
{
	redblacknode<PageRecordLRU> *root = tree->root;
	return (tree->locatenode(node, root));
}	

redblacknode<PageRecord>* locatePR(long pageNumber, PageRecordTree* prTree)
{
	redblacknode<PageRecord> *finder, *pageNode;
	PageRecord prFind = PageRecord(pageNumber, 0);
	finder = new redblacknode<PageRecord>(prFind);
	pageNode = findPagePRInTree(finder, prTree->pageRecordTree);
	delete finder;
	return pageNode;
}

redblacknode<PageRecordLRU>* locateLRU(long pageNumber, PageRecordTree* prTree)
{
	redblacknode<PageRecord> *finder, *pageNode;
	redblacknode<PageRecordLRU> *finderLRU, *lruNode;

	PageRecord prFind = PageRecord(pageNumber, 0);
	finder = new redblacknode<PageRecord>(prFind);
	
	pageNode = findPagePRInTree(finder, prTree->pageRecordTree);
	delete finder;
	printf("Pagenumber is %li PageNode has pageNumber %li and LRU of %li\n",pageNumber, pageNode->getvalue().getPageNumber(), pageNode->getvalue().getLRUNumber());	
	PageRecordLRU lruFind = PageRecordLRU(pageNumber,
		pageNode->getvalue().getLRUNumber());
	finderLRU = new redblacknode<PageRecordLRU>(lruFind);
	lruNode = findPageLRUInTree(finderLRU, prTree->pageRecordLRUTree);
	printf("We are looking for %li at time %li\n", finderLRU->getvalue().getPageNumber(), finderLRU->getvalue().getLRUNumber());
	delete finderLRU;
	return lruNode;
}

void 
buildPageChain(struct PageChain** headChain,
	struct PageChain** activeChain, redblacknode<PageRecord>* node)
{
	if (node == NULL) {
		return;
	}
	buildPageChain(headChain, activeChain, node->left);
	struct PageChain *nextChain =
		(struct PageChain*)malloc(sizeof(struct PageChain));
	nextChain->next = NULL;
	nextChain->page = node->getvalue().getPageNumber();
	if (*activeChain == NULL) {
		*activeChain = nextChain;
		*headChain = *activeChain;
	} else {
		(*activeChain)->next = nextChain;
		*activeChain = nextChain;
	}
	buildPageChain(headChain, activeChain, node->right);
}

extern "C" {

void* createPageTree(void)
{
	redblacktree<redblacknode<PageRecord> >* treePR;
	redblacktree<redblacknode<PageRecordLRU> >* treeLRU;
	PageRecordTree* prTree;
	treePR = new redblacktree<redblacknode<PageRecord> >();
	treeLRU = new redblacktree<redblacknode<PageRecordLRU> >();
	prTree = new PageRecordTree(treePR, treeLRU);
	return static_cast<void*>(prTree);
}

void removePageTree(void* tree)
{
	PageRecordTree* prTree;
	prTree = static_cast<PageRecordTree *>(tree);
	pthread_mutex_lock(&prTree->tree_lock);
	delete prTree->pageRecordTree;
	delete prTree->pageRecordLRUTree;
	pthread_mutex_unlock(&prTree->tree_lock);
	delete prTree;
}

void insertIntoPageTree(long pageNumber, time_t lruTime, void* tree)
{
	PageRecordTree *prTree;
	redblacknode<PageRecord> *additionPRNode;
	redblacknode<PageRecordLRU> *additionLRUNode;

	prTree = static_cast<PageRecordTree *>(tree);
	PageRecord addPR = PageRecord(pageNumber, lruTime);
	PageRecordLRU addPRLRU = PageRecordLRU(pageNumber, lruTime);

	additionPRNode = new redblacknode<PageRecord>(addPR);
	additionLRUNode = new redblacknode<PageRecordLRU>(addPRLRU);

	pthread_mutex_lock(&prTree->tree_lock);
	
	prTree->pageRecordTree->insertnode(additionPRNode,
		prTree->pageRecordTree->root);
	prTree->pageRecordLRUTree->insertnode(additionLRUNode,
		prTree->pageRecordLRUTree->root);
	pthread_mutex_unlock(&prTree->tree_lock);
}

void* locatePageTreePR(long pageNumber, void* tree)
{
	PageRecordTree *prTree;
	prTree = static_cast<PageRecordTree *>(tree);
	pthread_mutex_lock(&prTree->tree_lock);
	redblacknode<PageRecord> *pageNode = locatePR(pageNumber, prTree);
	pthread_mutex_unlock(&prTree->tree_lock);
	return static_cast<void*>(pageNode);
}

void* locatePageTreeLRU(long pageNumber, void* tree)
{
	PageRecordTree *prTree;
	prTree = static_cast<PageRecordTree *>(tree);
	pthread_mutex_lock(&prTree->tree_lock);
	redblacknode<PageRecordLRU> *lruNode = locateLRU(pageNumber, prTree);
	pthread_mutex_unlock(&prTree->tree_lock);
	return static_cast<void*>(lruNode);
}


void removeFromPageTree(long pageNumber, void* tree)
{
	PageRecordTree *prTree;
	prTree = static_cast<PageRecordTree *>(tree);
	pthread_mutex_lock(&prTree->tree_lock);
	redblacknode<PageRecord> *pageNode = locatePR(pageNumber, prTree);
	redblacknode<PageRecordLRU> *lruNode = locateLRU(pageNumber, prTree);
	if (lruNode == NULL)
		printf("FAIL\n");
	prTree->pageRecordTree->removenode(*pageNode);
	printf("B\n");prTree->pageRecordLRUTree->removenode(*lruNode);printf("C\n");
	pthread_mutex_unlock(&prTree->tree_lock);
}

void* removeOldestFromPageTree(void* tree)
{
	PageRecordTree *prTree;
	long pageNumber;
	redblacknode<PageRecord> *pageNode;
	redblacknode<PageRecordLRU> *lruNode;

	prTree = static_cast<PageRecordTree *>(tree);
	pthread_mutex_lock(&prTree->tree_lock);
	redblacknode<PageRecordLRU> *oldest = prTree->pageRecordLRUTree->max();
	if (oldest == NULL)
		goto exit;
	pageNumber = oldest->getvalue().getPageNumber();
	pageNode = locatePR(pageNumber, prTree);
	lruNode = locateLRU(pageNumber, prTree);
	prTree->pageRecordTree->removenode(*pageNode);
	prTree->pageRecordLRUTree->removenode(*lruNode);	
exit:
	pthread_mutex_unlock(&prTree->tree_lock);
	return oldest;
}

int countPageTree(void* tree)
{
	PageRecordTree *prTree;
	prTree = static_cast<PageRecordTree *>(tree);
	return prTree->pageRecordTree->count();
}

void updateLRU(long pageNumber, time_t lruTime, void* tree)
{
	removeFromPageTree(pageNumber, tree);
	insertIntoPageTree(pageNumber, lruTime, tree);
}

struct PageChain* getPageChain(void *tree)
{
	PageRecordTree *prTree;
	struct PageChain* activeChain = NULL;
	struct PageChain* headChain = NULL;
	prTree = static_cast<PageRecordTree *>(tree);
	buildPageChain(&headChain, &activeChain,
		prTree->pageRecordTree->root);
	return activeChain;
}

void cleanPageChain(struct PageChain* inChain)
{
	if (inChain == NULL){
		return;
	}
	struct PageChain* nextChain = inChain->next;
	delete inChain;
	cleanPageChain(nextChain);
}

}// end extern "C"		
