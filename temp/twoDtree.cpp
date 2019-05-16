
/**
*
* twoDtree (pa3)
* slight modification of a Kd tree of dimension 2.
* twoDtree.cpp
* This file will be used for grading.
*
*/

#include "twoDtree.h"

/* node constructor given */
twoDtree::Node::Node(pair<int, int> ul, pair<int, int> lr, HSLAPixel a)
: upLeft(ul), lowRight(lr), avg(a), LT(NULL), RB(NULL)
{
}

/* destructor given */
twoDtree::~twoDtree()
{
	clear();
}

/* copy constructor given */
twoDtree::twoDtree(const twoDtree &other)
{
	copy(other);
}

/* operator= given */
twoDtree &twoDtree::operator=(const twoDtree &rhs)
{
	if (this != &rhs)
	{
		clear();
		copy(rhs);
	}
	return *this;
}
//-----

/* buildTree helper for twoDtree constructor */
// assume stats 's' is correct and initialized
// maybe tolerance consideration

// NEW TODO: Move this to above the call

int twoDtree::calcEntropyH(stats &s, pair<int,int> ul, pair<int,int> lr) {
	int minI = -1;
	double min = 1000000L; //std::numeric_limits<double>::max()
	long totalArea = s.rectArea(ul,lr);


	for (size_t i = ul.first; i < lr.first; i++) {
		pair<int, int> upRectLR(lr.first, ul.second + i);
		long upArea = s.rectArea(ul, upRectLR);
		double upAreaRatio = upArea / totalArea;
		double upEntropy = s.entropy(ul, upRectLR);
		double weightedUpEntropy = upEntropy * upAreaRatio;

		//lower horizontal part
		pair<int, int> botRectUL(ul.first, ul.second + i + 1);
		long botArea = s.rectArea(botRectUL, lr);
		double botAreaRatio = botArea / totalArea;
		double botEntropy = s.entropy(botRectUL, lr);
		double weightedBotEntropy = botEntropy * botAreaRatio;

		//calculate weighted average of the entropy for that particular horizontal split
		double totalWeightedEntropy = weightedUpEntropy + weightedBotEntropy;

		// update the variables if this entropy is lower
		if (totalWeightedEntropy <= min) { //
			min = totalWeightedEntropy;
			minI = i;
		}
	}
	return minI;
}

int twoDtree::calcEntropyV(stats &s, pair<int,int> ul, pair<int,int> lr) {
	int minI = -1;
	double min = 10000000L; //std::numeric_limits<double>::max()
	long totalArea = s.rectArea(ul,lr);

	for (size_t i = ul.first; i < lr.first; i++) {
		//left vertical part
			pair<int, int> leftRectLR(i, lr.second);
			long leftArea = s.rectArea(ul, leftRectLR);
			double leftAreaRatio = leftArea / totalArea;
			double leftEntropy = s.entropy(ul, leftRectLR);
			double weightedLeftEntropy = leftEntropy * leftAreaRatio;

			//right vertical part
			pair<int, int> rightRectUL(i + 1, ul.second);
			long rightArea = s.rectArea(rightRectUL, lr);
			double rightAreaRatio = rightArea / totalArea;
			double rightEntropy = s.entropy(rightRectUL, lr);
			double weightedRightEntropy = rightEntropy * rightAreaRatio;

			//total weighted average of the entropy of the this particular vertical split
			double totalWeightedEntropy = weightedLeftEntropy + weightedRightEntropy;

			// update the variables if this entropy is lower
			if (totalWeightedEntropy <= minEntropySoFar) { // <= SAID ON PIAZZA
				minEntropySoFar = totalWeightedEntropy;
				minEntIdx = i;
				newLR = leftRectLR;
				newUL = rightRectUL;
			}
	}
	return minI;
}

twoDtree::Node *twoDtree::buildTree(stats &s, pair<int, int> ul, pair<int, int> lr, bool vert)
{
	std::cout << "thisHue@ UL   "<< ul.first << " * " << ul.second << "  LR   "<<
	lr.first << " * " << lr.second << '\n';
	Node* node = new Node(ul, lr, s.getAvg(ul, lr));

	std::cout << "after newnode" << '\n';
	// base case
	if (ul == lr) {
		return node;
	}

	pair<int,int> f1 = ul;
	pair<int,int> f2;

	pair<int,int> s1;
	pair<int,int> s2 = lr;

	if (ul.first == lr.first) {
		int i = calcEntropyH(s, ul, lr); //returns min index
		f2 = make_pair(lr.first, i);
		s1 = make_pair(ul.first, i+1);

	} else if (ul.second == lr.second) {
		int i = calcEntropyV(s, ul, lr);
		f2 = make_pair(i, lr.second);
		s1 = make_pair(i+1, ul.second);
	} else {
		if(vert) {
			int i = calcEntropyV(s, ul, lr);
			std::cout << "minI" << i << '\n';
			f2 = make_pair(i, lr.second);
			s1 = make_pair(i+1, ul.second);
		} else {
			int i = calcEntropyH(s, ul, lr);
			f2 = make_pair(lr.first, i);
			s1 = make_pair(ul.first, i+1);
		}
	}

	node ->LT = buildTree(s, f1, f2, !vert);
	node ->RB = buildTree(s, s1, s2, !vert);

	return node;
}

twoDtree::twoDtree(PNG &imIn)
{
	stats stats_imIn(imIn);

	height = imIn.height();
	width = imIn.width();
	root = buildTree(stats_imIn, make_pair(0, 0), make_pair(width - 1, height - 1), true);
}
//

// /* buildTree helper for twoDtree constructor */
// twoDtree::Node *twoDtree::buildTree(stats &s, pair<int, int> ul, pair<int, int> lr, bool vert)
// {

// 	/* your code here */
// }

/* render your twoDtree into a png */
/*
iterate until hit a leaf node,
*/

//std::cout << "thisHue@ UL   "<< node->upLeft.first << " * " << node->upLeft.second << "  LR   "<<
//node->lowRight.first << " * " << node->lowRight.second <<
//" : " << node->avg.h << '\n';

void twoDtree::renderNode(PNG& im, Node* node) {
	if (!node->RB && !node->LT) {
		for (size_t i = node->upLeft.first; i <= root->lowRight.first; i++) { //added +1 -- running gives warnings
			for (int j = root->upLeft.second; j <= root->lowRight.second; j++) {
				*(im.getPixel(i,j)) = node ->avg; // "root-> avg" -> "node->avg"
			}
		}

		// std::cout << "thisHue@ UL   "<< node->upLeft.first << " * " << node->upLeft.second << "  LR   "<<
		// node->lowRight.first << " * " << node->lowRight.second <<
		// " : " << node->avg.h << '\n';

		return;
	}

	if (node->LT) {
		renderNode(im, node->LT);
	}
	if (node->RB) {
		renderNode(im, node->RB);
	}
}

PNG twoDtree::render()
{
	PNG ansPNG(width, height);
	renderNode(ansPNG, root);

	return ansPNG;
}


// ----------

void twoDtree::pruneRecursive(Node* node, double tol) {
	if (!node) {
		return;
	}

	HSLAPixel pix = node->avg;
	double ltDiff = pix.dist(node->LT->avg);
	double rbDiff = pix.dist(node->RB->avg);

	if (ltDiff <= tol && rbDiff <= tol) {
		clearRecurse(node->LT);
		clearRecurse(node->RB);
	} else {
		pruneRecursive(node->LT, tol);
		pruneRecursive(node->RB, tol);
	}
}

/* prune function modifies tree by cutting off
* subtrees whose leaves are all within tol of
* the average pixel value contained in the root
* of the subtree
*/
void twoDtree::prune(double tol)
{
	pruneRecursive(root, tol);
	// render(); //dont need to call render
}

void twoDtree::clearRecurse(Node *node)
{
	if (node->LT == nullptr && node->RB == nullptr){
		delete node;
	} else	{
		//at this point, no more leaves
		if (node->LT)
		clearRecurse(node->LT);

		//all of node's LT is deleted
		if (node->RB)
		clearRecurse(node->RB);
		//all of node's RB is deleted
	}
}

/* helper function for destructor and op= */
/* frees dynamic memory associated w the twoDtree */
void twoDtree::clear()
{
	clearRecurse(root);
	root->LT = nullptr;
	root->RB = nullptr;
	root = nullptr;
}

/* copy constructor given */

//recursive helper for copy
void twoDtree::insertRecurse(Node *&thisRoot, Node *otherRoot)
{
	if (otherRoot->RB)
	{
		Node *otherRB = otherRoot->RB;
		thisRoot->RB = new Node(otherRB->upLeft, otherRB->lowRight, otherRB->avg);
		//thisRoot->avg = otherRB->avg; //mayb look into this
		insertRecurse(thisRoot->RB, otherRoot->RB);
	}

	if (otherRoot->LT)
	{
		Node *otherLT = otherRoot->LT;
		thisRoot->LT = new Node(otherLT->upLeft, otherLT->lowRight, otherLT->avg);
		//thisRoot->avg = otherLT->avg; //mayb look into this
		insertRecurse(thisRoot->LT, otherRoot->LT);
	}
}

/* helper function for copy constructor and op= */
void twoDtree::copy(const twoDtree &orig)
{
	//deep copies root
	Node *otherRoot = orig.root;
	root = new Node(otherRoot->upLeft, otherRoot->lowRight, otherRoot->avg);
	insertRecurse(root, otherRoot);
}

// twoDtree::Node* twoDtree::buildTreeRecurse(stats & s, pair<int,int> ul, pair<int,int> lr, bool vert, Node* root) {
// 		Node* node = new Node();
// 		if (root == nullptr) return root;

// 		//--------------------------------------------------

// 		// find the minimum weighted average of the entropies for image splitting

// 		//local variables for finding weighted averages

// 		//dimension of the current "rectangle of pixel"
// 		int thisWidth = lr.first - ul.first + 1;
// 		int thisHeight = lr.second - ul.second + 1;
// 		int vectorSize = (vert ? thisWidth - 1 : thisHeight - 1); //vector size of the vector for storing weighted average of entropies
// 		vector<double> weightedAvgs(vectorSize, 0); //vector of the weighted average of the entropies
// 		long totalArea = s.rectArea(ul, lr); //total area of the current rectangle of pixels

// 		//--------------------------------------------------

// 		//storing all possible weighted averages during vertical split
// 		if (vert) {

// 			//iterating through "vertically"
// 			for (size_t i = 0; i < thisWidth - 1; i++) {

// 				//calculating the weighted average of the entropy for the split

// 				//left vertical part
// 				pair<int, int> leftRectLR(ul.first + i, lr.second);
// 				long leftArea = s.rectArea(ul, leftRectLR);
// 				double leftAreaRatio = leftArea/totalArea;
// 				double leftEntropy = s.entropy(ul, leftRectLR);
// 				double weightedLeftEntropy = leftEntropy * leftAreaRatio;

// 				//right vertical part
// 				pair<int, int> rightRectUL(ul.first + i + 1, lr.second);
// 				long rightArea = s.rectArea(rightRectUL, lr);
// 				double rightAreaRatio = rightArea/totalArea;
// 				double rightEntropy = s.entropy(rightRectUL, lr);
// 				double weightedRightEntropy = rightEntropy * rightAreaRatio;

// 				//total weighted average of the entropy of the whole vertical split
// 				double totalWeightedEntropy = weightedLeftEntropy + weightedRightEntropy;

// 				//storing the weighted average of the entropy of that part split
// 				weightedAvgs[i] = totalWeightedEntropy;
// 			}
// 		}

// 		//storing all possible weighted averages during horizontal split
// 		else {

// 			//iterating through horizontally
// 			for (size_t i = 0; i < thisHeight - 1; i++) {

// 				//calculate weighted average of the entropy for the upper rectangle part
// 				pair<int, int> upRectLR(lr.first, ul.second + i);
// 				long upArea = s.rectArea(ul, upRectLR);
// 				double upAreaRatio = upArea/totalArea;
// 				double upEntropy = s.entropy(ul, upRectLR);
// 				double weightedUpEntropy = upEntropy * upAreaRatio;

// 				//calculate weighted average of the entropy for the lower rectangle part
// 				pair<int, int> botRectUL(ul.first, ul.second + i + 1);
// 				long botArea = s.rectArea(botRectUL, lr);
// 				double botAreaRatio = botArea/totalArea;
// 				double botEntropy = s.entropy(botRectUL, lr);
// 				double weightedBotEntropy = botEntropy * botAreaRatio;

// 				//calculate weighted average of the entropy for the whole horizontal split
// 				double totalWeightedEntropy = weightedUpEntropy + weightedBotEntropy;

// 				//store the weighted average of that particular horizontal split into the vector
// 				weightedAvgs[i] = totalWeightedEntropy;
// 			}
// 		}

// 		//--------------------------------------------------

// 		//get the index of the minimum value of the weighted average in the vector

// 		int minEntropyIndex = distance(weightedAvgs.begin(), min_element(weightedAvgs.begin(), weightedAvgs.end()));

// 		//left and right nodes of the root

// 		Node leftTopNode();
// 		Node rightBotNode;

// 		//creating or "constructing" the left and right nodes of the root for each particular split

// 		//left and right nodes during the vertical split
// 		if (vert) {
// 			node->LT = buildTree();

// 			//left node
// 			pair<int, int> llr(ul.first + minEntropyIndex, lr.second);
// 			leftTopNode = new Node(ul, llr, s.getAvg(ul, llr));

// 			//right node
// 			pair<int, int> rul(ul.first + minEntropyIndex + 1, ul.second);
// 			rightBotNode = new Node(rul, lr, s.getAvg(rul, lr));
// 		}

// 		//left and right nodes during the horizontal split
// 		else {

// 			//left node (the top part of the horizontal split)
// 			pair<int, int> ulr(lr.first, ul.second + minEntropyIndex); //up lower right
// 			leftTopNode = new Node(ul, ulr, s.getAvg(ul, ulr));

// 			//right node (the bottom part of the horizontal split)
// 			pair<int, int> bul(ul.first, ul.second + minEntropyIndex + 1); //bottom upper left
// 			rightBotNode = new Node(bul, lr, s.getAvg(bul, lr));
// 		}

// 		// have the current root connect into its new left and right nodes
// 		root->LT = leftTopNode;
// 		root->RB = rightBotNode;

// 		//negate vert for the next round of split
// 		vert = !vert;

// 		//keep on continuing the split until xxxxxxxxxxx
// 		buildTreeRecurse(s, leftTopNode->upLeft, leftTopNode->lowRight, vert, root->LT);
// 		buildTreeRecurse(s, rightBotNode->upLeft, rightBotNode->lowRight, vert, root->RB);
// 	}

// 	/* buildTree helper for twoDtree constructor */
// 	// assume stats 's' is correct and initialized
// 	// maybe tolerance consideration
// 	twoDtree::Node * twoDtree::buildTree(stats & s, pair<int,int> ul, pair<int,int> lr, bool vert) {
// 		return buildTreeRecurse(s, ul, lr, vert, root);
// 	}
