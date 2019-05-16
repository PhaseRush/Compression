
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
twoDtree::~twoDtree() {
	clear();
}

/* copy constructor given */
twoDtree::twoDtree(const twoDtree &other) {
	copy(other);
}

/* operator= given */
twoDtree &twoDtree::operator=(const twoDtree &rhs) {
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

twoDtree::Node *twoDtree::buildTree(stats &s, pair<int, int> ul, pair<int, int> lr, bool vert) {

	int thisWidth = lr.first - ul.first + 1;
	int thisHeight = lr.second - ul.second + 1;

	Node *thisNode = new Node(ul, lr, s.getAvg(ul, lr));

	// base case: cease splitting when rectangle size is 1 ///GOOD
	// cant have children, return thisNode
	if (thisWidth == 1 && thisHeight == 1) {
		return thisNode; // compiler warning
	}

	/// override the passed value of vert and set it accordingly
	if (thisWidth == 1) vert = false;
	else if (thisHeight == 1) vert = true;


	long totalArea = s.rectArea(ul, lr); //total area of the current rectangle of pixels

	double minEntropySoFar = 0; // init to high value so everything will be less

	pair<int, int> newLR(0, 0); // 2 points to keep track of, regardless of vert split
	pair<int, int> newUL(0, 0);
	// ------------------------------------------------------------------

	if (vert) {
		//iterating through "vertically"
		for (size_t i = 0; i < thisWidth - 1; i++) {
			//calculating the weighted average of the entropy for each possible vertical split

			//left vertical part
			pair<int, int> leftRectLR(ul.first + i, lr.second);
			long leftArea = s.rectArea(ul, leftRectLR);
			double leftAreaRatio = i + 1;
			double leftEntropy = s.entropy(ul, leftRectLR);
			double weightedLeftEntropy = leftEntropy * leftAreaRatio;

			//right vertical part
			pair<int, int> rightRectUL(ul.first + i + 1, ul.second); // ul.second
			long rightArea = s.rectArea(rightRectUL, lr);
			double rightAreaRatio = thisWidth - i - 1;
			double rightEntropy = s.entropy(rightRectUL, lr);
			double weightedRightEntropy = rightEntropy * rightAreaRatio;

			//total weighted average of the entropy of the this particular vertical split
			double totalWeightedEntropy = weightedLeftEntropy + weightedRightEntropy;

			if (i == 0 || totalWeightedEntropy <= minEntropySoFar)
			{
				minEntropySoFar = totalWeightedEntropy;
				newLR = leftRectLR;
				newUL = rightRectUL;
			}
		}
	}
	else
	{ // ! vert (horizontal)
		//iterating the root's rectangle horizontally
		for (size_t i = 0; i < thisHeight - 1; i++)
		{
			//upper horizontal part
			pair<int, int> upRectLR(lr.first, ul.second + i);
			long upArea = s.rectArea(ul, upRectLR);
			double upAreaRatio = i + 1;
			double upEntropy = s.entropy(ul, upRectLR);
			double weightedUpEntropy = upEntropy * upAreaRatio;

			//lower horizontal part
			pair<int, int> botRectUL(ul.first, ul.second + i + 1);
			long botArea = s.rectArea(botRectUL, lr);
			double botAreaRatio = thisHeight - i - 1;
			double botEntropy = s.entropy(botRectUL, lr);
			double weightedBotEntropy = botEntropy * botAreaRatio;

			//calculate weighted average of the entropy for that particular horizontal split
			double totalWeightedEntropy = weightedUpEntropy + weightedBotEntropy;

			// update the variables if this entropy is lower
			if (i == 0 || totalWeightedEntropy <= minEntropySoFar)
			{
				minEntropySoFar = totalWeightedEntropy;
				newLR = upRectLR;
				newUL = botRectUL;
			}
		}
	}

	// left top subNode --> parent upperLeft, new lowerRight
	thisNode->LT = buildTree(s, ul, newLR, !vert); // flip vert every time, width==1 will catch edge case
	// right bot subNode --> new upperLeft, parent lowerRight
	thisNode->RB = buildTree(s, newUL, lr, !vert);

	return thisNode; //return thisNode to fulfill recursive contract
} //end of buildtree helper function

twoDtree::twoDtree(PNG &imIn) {
	stats stats_imIn(imIn);

	height = imIn.height();
	width = imIn.width();
	root = buildTree(stats_imIn, make_pair(0, 0), make_pair(width - 1, height - 1), true);
}

//------
void twoDtree::renderNode(PNG &im, Node *&node) {

	if (node->RB == NULL && node->LT == NULL)
	{
		for (size_t i = node->upLeft.first; i <= node->lowRight.first; i++)
		{ //added +1 -- running gives warnings
			for (int j = node->upLeft.second; j <= node->lowRight.second; j++)
			{
				*(im.getPixel(i, j)) = node->avg; // "root-> avg" -> "node->avg"
			}
		}
		return;
	}

	if (node->LT)
	{
		renderNode(im, node->LT);
	}
	if (node->RB)
	{
		renderNode(im, node->RB);
	}
}

PNG twoDtree::render() {
	PNG ansPNG(width, height);
	cout << "start rendering\n";
	renderNode(ansPNG, root);
	cout << "render done\n";
	return ansPNG;
}

// ----------
bool twoDtree::shouldPrune(Node *node, HSLAPixel avg, double tol) {
	//GOAL: find the highest (tallest) node which all children (subtrees/leaves) are within tolerance
	//when we traverse to the leaves from the current node (parent or root)
	if (node->LT == NULL && node->RB == NULL) {
		//compares if the distances between the leaves' avg and the current node's avg is within tolerance
		double diffLeft = node->avg.dist(avg);
		return diffLeft <= tol;
	}

	//traverse into the leaves
	else return shouldPrune(node->LT, avg, tol) && shouldPrune(node->RB, avg, tol);

}

void twoDtree::pruneRecursive(Node *node, double tol) {
	if (node == NULL) return;

	HSLAPixel pix = node->avg; //average pixel of the current node (aka the parent)

	if (shouldPrune(node, pix, tol)) {
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
void twoDtree::prune(double tol) {
	pruneRecursive(root, tol);
}

void twoDtree::clearRecurse(Node *&node) {
	if (node == NULL) return;

	clearRecurse(node->LT);
	clearRecurse(node->RB);

	delete node;
	node = NULL;
}

/* helper function for destructor and op= */
/* frees dynamic memory associated w the twoDtree */
void twoDtree::clear() {
	clearRecurse(root);
}

/* copy constructor given */

//recursive helper for copy
void twoDtree::insertRecurse(Node *&thisRoot, Node *otherRoot) {
	if (otherRoot == nullptr) {
		thisRoot = NULL;
		return;
	}

	thisRoot = new Node(otherRoot->upLeft, otherRoot->lowRight, otherRoot->avg);
	insertRecurse(thisRoot->RB, otherRoot->RB);
	insertRecurse(thisRoot->LT, otherRoot->LT);
}

/* helper function for copy constructor and op= */
void twoDtree::copy(const twoDtree &orig) {
	//deep copies root ;
	width = orig.width;
	height = orig.height;
	insertRecurse(root, orig.root);
}
