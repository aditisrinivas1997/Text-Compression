#include "proto.h"
#include "huffman.h"

bstnode *bstroot = NULL;				//root of BST
treenode *huffroot = NULL;

void sortpq (node **arr,int len){				//to sort the frequency table based on the frequency
	node* pivot = arr[0];
	node *arr2 = (node *)malloc(sizeof(node)*len);
	int i,j=0,k=len-1;
	for(i = 1 ; i < len ; i++){
		if(arr[i]->freq < pivot->freq){
			arr2[j++] = *arr[i];
		}
		else{
			arr2[k--] = *arr[i];
		}
	}
	arr2[k] = *pivot;
	for(i = 0 ; i < len ; i++){
		*arr[i] = arr2[i];
	}
	if((k-1) > 1){
		sortpq(arr,k-1);
	} 
	if((len-k-1) > 1){
		sortpq(&arr[k+1],(len-k-1));
	}
}

treenode ** createleaf (node **pq,int len){							//create leaf nodes and store them in an array
	treenode **arr = (treenode **)malloc(sizeof(treenode *)*len);
	int i;
	for(i = 0 ; i < len ; i ++){
		treenode *p = (treenode *)malloc(sizeof(treenode));
		p->ele = pq[i]->ele;
		p->freq = pq[i]->freq;
		p->left = NULL;
		p->right = NULL;
		arr[i] = p;
	}
	return arr;
}

void preorder (bstnode *root){				//print the huffman code for each element
	printf("%d\t%s\n",root->ele,root->s);
	if(root->left){
		preorder(root->left);
	}
	if(root->right){
		preorder(root->right);
	}
}

void addtobst (treenode *leaf , bstnode *root){					//add a node to the bst
	if(leaf->ele > root->ele){
		if(root->right == NULL){
			root->right = (bstnode *)malloc(sizeof(bstnode));
			root->right->ele = leaf->ele;
			root->right->left = root->right->right = NULL;
			return;
		}
		addtobst(leaf,root->right);
	}
	else{
		if(root->left == NULL){
			root->left = (bstnode *)malloc(sizeof(bstnode));
			root->left->ele = leaf->ele;
			root->left->left = root->left->right = NULL;
			return;
		}
		addtobst(leaf,root->left);
	}
}

void addhuffcode (bstnode *root,int e,char *s){		//to store the huffman code for the corresponding element
	if(root->ele == e){
		root->s = s;
	}
	else if(e <= root->ele){
		addhuffcode(root->left,e,s);
	}
	else{
		addhuffcode(root->right,e,s);
	}
}

bstnode * createbst (treenode **leaves,bstnode *bstroot,int count){		//to create a binary search tree
	bstroot = (bstnode *)malloc(sizeof(bstnode));
	bstroot->ele = leaves[0]->ele;
	bstroot->left = bstroot->right = NULL;
	int i;
	for(i = 1 ; i < count ; i++){
		addtobst(leaves[i],bstroot);
	}
	return bstroot;
}

char * dtb (int s, int o){						//converts decimal number (integer) to a binary number and converts the binary number to a string
	int len = 0;
	char *retval = (char *)malloc(sizeof(char));
	char r;

	while(s != 0){
		r = s % 2;
		retval = (char *)realloc(retval,sizeof(char)*(len+1));
		r += '0';
		retval[len++] = r;
		s = s/2;
	}
	//retval = (char *)realloc(retval,sizeof(char)*(len-1));
	//printf("Done realloc %p\n", retval);
	//len -= 2;
	//printf("Before rev\n");
	int i,temp, j;

	for(i = 0; i < len/2 ;i++ ){
		temp = retval[i] ;
		retval[i] = retval[len-1-i];
		retval[len-1-i] = temp;
	}
	//printf("Done rev\n");
	retval = (char *)realloc(retval,sizeof(char)*(len+1));
	
	//printf("Done realloc 2 %p\n", retval);
	retval[len++] = '\0';
	//printf("Added null %s\n", retval);
	
	char *ret;

	switch(o) {
		case 1 :
			return (retval+2);

		case 2 :
			ret = (char *)malloc(sizeof(char) * 9);
			for(i = 0 ; i < (9-len) ; i++)
				ret[i] = '0';
			for(j = 0 ; i < 9 ; i++, j++)
				ret[i] = retval[j];

			free(retval);

			return ret;
	}

	return 0;
}

void genhuffcode (treenode *root , int s , bstnode *bstroot){		//function to generate the huffman code for an element
	s *= 2;
	s += root->pos;
	if(root->left == NULL && root->right == NULL){
		addhuffcode(bstroot,root->ele,dtb(s, 1));
	}
	if(root->left){
		genhuffcode(root->left,s,bstroot);
	}
	if(root->right){
		genhuffcode(root->right,s,bstroot);
	}
}

void huffman (int *ar,int len, FILE *fop){				//function to create a huffman tree 
												//file to store the PQ

	int arr[len], i;
	for(i = 0 ; i < len ; i++)
		arr[i] = ar[i];
	char *s = (char *)calloc(sizeof(char),1);
	treenode *start = NULL;
	treenode **leaves,**temp;
	int *leavescpy;
	
	node **pq;
	int j,count=1,flag;
	node **ftable = NULL;
	ftable = (node **)realloc(ftable,sizeof(node *));	
	ftable[0] = (node *)malloc(sizeof(node));
	ftable[0]->ele = arr[0];
	ftable[0]->freq = 1;
	//for(i = 0 ; i < len ; i++)
	//	printf("%d\n", arr[i]);
	//printf("%d\t", ftable[0]->ele);
	//printf("%d\n", ftable[0]->freq);
	for(i = 1 ; i < len ; i++){					//create a frequency table i.e element and its frequency
		
		flag = 0;
		for(j = 0; j < count ; j++){
			if(arr[i] == ftable[j]->ele){
				ftable[j]->freq++;
				flag = 1;
				break;
			}
		}
		if(flag == 0){
			count++;
			ftable = (node **)realloc(ftable,sizeof(node *)*count);
			ftable[count-1] = (node *)malloc(sizeof(node));
			//printf("ARR %d %d\n", i, arr[i]);
			ftable[count-1]->ele = arr[i];
			ftable[count-1]->freq = 1;
			//printf("%d\t%d\t", arr[i], ftable[count-1]->ele);
			//printf("%d\n", ftable[count-1]->freq);
		}
	}
	
	sortpq(ftable,count);						//sort the frequency table

	printf("Count %d\n", count);
	fwrite(&count, sizeof(count), 1, fop);
	for(i = 0 ; i < count ; i++)				//store the priority queue
		fwrite(ftable[i], sizeof(node), 1, fop);
	
/*	for(i = 0 ; i < count ; i++) {
		printf("%d\t%d\n", ftable[i]->ele, ftable[i]->freq);
	}*/

	
	leaves = createleaf(ftable,count);				//creates an array of leaf nodes
	leavescpy = (int *)malloc(sizeof(int)*count);	

	for(i = 0 ; i < count ; i++){				//make a copy of the leaves array
		leavescpy[i] = leaves[i]->ele;
	}

	temp = leaves;
	bstroot = createbst(leaves,bstroot,count);	//create a binary search tree

	for(i = count ; i > 1 ; i--){							//generate the huffman tree
		treenode *p = (treenode *)malloc(sizeof(treenode));
		p->freq = leaves[0]->freq + leaves[1]->freq;
		p->left = leaves[0];
		p->left->pos = 0;
		p->right = leaves[1];
		p->right->pos = 1;
		count--;
		for(j = 1; j < count-1 ; j++){
			if(leaves[j+1]->freq < p->freq){
				leaves[j] = leaves[j+1];
			}
			else{
				break;
			}
		}
		leaves[j] = p;
		leaves++;
	}
	huffroot = leaves[0];		//root of the huffman tree
	huffroot->pos = 0 ;			
	
	genhuffcode(huffroot,1,bstroot);	//generate and store the huffman code for all the elements in the huffman tree
	free(temp);
	//preorder(bstroot);
}

char * search(bstnode *root, int n,char *bstr) {
	//printf("%p %d %d\n", root, root->ele, n);
	//bstr = (char *)malloc(sizeof(char));
	//int s;
	if(root->ele == n) {
		//printf("Q %d\n", root->ele);
		//s = root->s;
		//return root->s;
		//printf("%lu\t",blen);
		//bstr = (char *)realloc(bstr, blen + (sizeof(char) * strlen(root->s)));
		
		//printf("%lu\t%lu\n",strlen(root->s),sizeof(bstr));
		bstr = (char *)malloc(sizeof(char) * strlen(root->s));
		strcpy(bstr, root->s);
		//blen  = blen + strlen(root->s);
		//printf("%s\n",bstr);
		return bstr;
	}
	else if(n > root->ele)
		bstr = search(root->right, n,bstr);
	else
		bstr = search(root->left, n,bstr);
	return bstr;
}

void clearTree(void *root, short o) {		//1 = huffman tree; 2 = BST
	switch(o) {
		case 1 :
			if(((treenode *)root)->left != NULL)
				clearTree(((treenode *)root)->left, o);
			if(((treenode *)root)->right != NULL)
				clearTree(((treenode *)root)->right, o);
			
			break;

		case 2 :
			if(((bstnode *)root)->left != NULL)
				clearTree(((bstnode *)root)->left, o);
			if(((bstnode *)root)->right != NULL)
				clearTree(((bstnode *)root)->right, o);
			
			break;
	}

	free(root);
}