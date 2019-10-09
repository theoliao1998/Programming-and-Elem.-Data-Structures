#include <iostream>
#include <cassert>
#include "p2.h"

using namespace std;

int size(list_t list){
    if(list_isEmpty(list)) return 0;
    else return 1+size(list_rest(list));
    //for every element in list the size +1 until the rest list is empty
}

int sum(list_t list){
    if(list_isEmpty(list)) return 0;
    else return list_first(list)+sum(list_rest(list));
    //0 recursively add the first element of the rest list
}

int product(list_t list) {
    if (list_isEmpty(list)) return 1;
    else return list_first(list) * product(list_rest(list));
    //1 recursively multiply the first element of the rest list
}

int accumulate(list_t list, int (*fn)(int, int), int base){
    if (list_isEmpty(list)) return base;
    else return fn(list_first(list), accumulate(list_rest(list), fn, base));
    //the base recursively associate with the first element of the rest list
}

list_t reverse(list_t list){
    if(list_isEmpty(list)) return list;
    else return append(reverse(list_rest(list)),list_make(list_first(list),list_make()));
    //since list_make can add an element before a list,
    //recursively use list_make to add every element in an empty list
}

list_t append(list_t first, list_t second){
    if (list_isEmpty(first) ) return second;
    else return list_make(list_first(first),append(list_rest(first),second));
    //since list_make can add an element before a list,
    //recursively add every element of the first list from the last one to the second list
}

static bool odd(int number) {
    return number%2==1;
}

list_t filter_odd(list_t list){
    return filter(list,odd);
}

static bool even(int number) {
    return number%2==0;
}

list_t filter_even(list_t list){
    return filter(list,even);
}

list_t filter(list_t list, bool (*fn)(int)){
    if (list_isEmpty(list) ) return list;
    else if (fn(list_first(list))) return list_make(list_first(list),filter(list_rest(list),fn));
    else return filter(list_rest(list),fn);
    //recursively check the first element of rest list with fn,
    //if fn returns true, add it in the result
}

list_t insert_list(list_t first, list_t second, unsigned int n){
    if(n>(unsigned int)size(first)){
        cerr << "Error: user passed invalid number\n";
        assert(0);
    }
    else return append(append(chop(first,(unsigned int)size(first)-n),second),reverse(chop(reverse(first),n)));
    //first chop the first list and only remain the first n elements, then append them with the second list,
    //then append the obtained list with the list of the chopped elements before,
    //the list of the chopped elements before can be obtained by chop the reversed first list last n elements and
    //reverse it back
}

list_t chop(list_t list, unsigned int n){
    if(n>(unsigned int)size(list)){
        cerr << "Error: user passed invalid number\n";
        assert(0);
    }
    else if(n==(unsigned int)size(list)) return list_make();
    else return list_make(list_first(list),chop(list_rest(list),n));
    //recursively check whether the rest list has the size of n
    //make the rest list with the size of n to be an empty list
}

int tree_sum(tree_t tree){
    if(tree_isEmpty(tree)) return 0;
    else return tree_elt(tree)+tree_sum(tree_left(tree))+tree_sum(tree_right(tree));
    //the top element recursively add all the top elements of the subtrees
}

bool tree_search(tree_t tree, int key){
    if(tree_isEmpty(tree)) return 0;
    else if (tree_elt(tree)==key) return 1;
    else return tree_search(tree_left(tree),key) || tree_search(tree_right(tree),key);
    //recursively check whether the top elements of the tree and its subtrees are equal to the key
}

int depth(tree_t tree){
    if(tree_isEmpty(tree)) return 0;
    else if (depth(tree_left(tree))>depth(tree_right(tree))) return 1+depth(tree_left(tree));
    else return 1+depth(tree_right(tree));
    //recursively compare the depth of the subtrees until find a route with most layers, add 1 for one layer of elements
}

static int tree_min_helper(int min,list_t list){
    if(list_isEmpty(list)) return min;
    else if(min>list_first(list)) return tree_min_helper(list_first(list),list_rest(list));
    else return tree_min_helper(min,list_rest(list));
    //recursively compare min with the first element of the rest list,
    //if one first element is smaller, let it min have the same value with it
}

int tree_min(tree_t tree){
    list_t tree_list=traversal(tree);
    return tree_min_helper(list_first(tree_list),list_rest(tree_list));
    //change the tree to a list, use tree_min_helper to recursively compare the elements of the list and obtain the minimum one
}

list_t traversal(tree_t tree){
    if(tree_isEmpty(tree)) return list_make();
    else if(depth(tree)==1) return list_make(tree_elt(tree),list_make());
    else return append(traversal(tree_left(tree)),list_make(tree_elt(tree),traversal(tree_right(tree))));
    //for every subtree, recursively make a list with elements in its left tree, and append the list with the list
    //of the top element followed by the list with elements in its right tree
}

bool tree_hasPathSum(tree_t tree, int sum){
    if(tree_isEmpty(tree)) return sum==0;
    else if (sum<tree_elt(tree)) return 0;
    else if (depth(tree)==1) return sum==tree_elt(tree);
    else return tree_hasPathSum(tree_left(tree),sum - tree_elt(tree)) || tree_hasPathSum(tree_right(tree),sum - tree_elt(tree));
    //recursively check whether the subtree has a path with the sum of all elements along the path
    // to be sum minus the top element of the subtree
}

bool covered_by(tree_t A, tree_t B){
    if(tree_isEmpty(A)) return 1;
    else if(tree_isEmpty(B)) return 0;
    else return tree_elt(A)==tree_elt(B) && covered_by(tree_left(A),tree_left(B)) && covered_by(tree_right(A),tree_right(B));
    //recursively check whether the subtrees has left and right trees covered by the corresponded left and right trees,
    //and the top elements the same
}

bool contained_by(tree_t A, tree_t B){
    if (covered_by(A,B)) return 1;
    else if (tree_isEmpty(B)) return 0;
    else return contained_by(A,tree_left(B)) || contained_by(A,tree_right(B));
    //recursively check whether the subtrees of tree B contains tree A
}

tree_t insert_tree(int elt, tree_t tree){
    if(tree_isEmpty(tree)) return tree_make(elt,tree_make(),tree_make());
    else if (elt>=tree_elt(tree)) return tree_make(tree_elt(tree),tree_left(tree),insert_tree(elt,tree_right(tree)));
    else return tree_make(tree_elt(tree),insert_tree(elt,tree_left(tree)),tree_right(tree));
    //recursively compare elt with the root elements of tree and subtrees to insert it an a proper position
    //that if it's greater than or equal to a root element then in its right tree, otherwise in its left tree
}
