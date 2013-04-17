#include <iostream>
#include <stack>
#include <cstdlib>
#include <cstdio>
#include <vector>
using namespace std;

enum operator_t {
    operator_none,
    operator_left_bracket,
    operator_right_bracket,
    operator_plus,
    operator_mul,
    operator_power
    
};

enum node_type_t {
    node_type_operand,
    node_type_operator
};

struct node_t {
    node_t *parent;
    node_t *left;
    node_t *right;
    node_type_t type;
    char bra_level;
    union {
        char opera;
        char operand[32];
    }u;    
};


#define IS_OPERATOR(c)  (c == '+' || c == '*' || c == '^' || c == '(' || c == ')')

static operator_t whatis(char c)
{
    if(c == '(') return operator_left_bracket;
    if(c == ')') return operator_right_bracket;
    if(c == '+') return operator_plus;
    if(c == '*') return operator_mul;
    if(c == '^') return operator_power;
    
    return operator_none;
}
      
static inline node_t* new_node()
{
    return new node_t;
}

static inline void del_node(node_t* node)
{
    delete node;
}

stack<node_t*> operator_stack;
stack<node_t*> operand_stack;
vector<node_t*> top_node_list;

static inline node_t* new_op_node(char* c, char bracket_level)
{
    node_t *node = new_node();
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    node->bra_level = bracket_level;

    if(!IS_OPERATOR(*c)) {
		int i = 0;
        while(*c != '\0' && !IS_OPERATOR(*c)) {
			node->u.operand[i++] = *c++;
		}
		node->u.operand[i] = '\0';

        node->type = node_type_operand;
    }
    else {
        node->u.opera = *c;
        node->type = node_type_operator;
    }
    return node;
}

static inline void cons_expr(node_t *expr_cur,node_t *expr_left,node_t *expr_right)
{
    expr_cur->left = expr_left;
    expr_cur->right = expr_right;

	if(expr_left)
		expr_left->parent = expr_cur;

	if(expr_right)
		expr_right->parent = expr_cur;
}

#define IS_HIGHER_PRIORITY(op1,op2)   (whatis(op1) > whatis(op2)) 
#define IS_EQUAL_PRIORITY(op1,op2) (whatis(op1) == whatis(op2))

static node_t* synax_tree(char *expr)
{
    char *tmp = expr;

    char c;
    node_t *root = NULL;
    char level = 0;
    char top_op = 0;
    node_t *top_node;
    
    while(*tmp != '\0') {
        c = *tmp;
		node_t *cur = new_op_node(tmp,level);
		tmp++;
        if(!IS_OPERATOR(c)) {
			while(*tmp != '\0' && !IS_OPERATOR(*tmp))
				tmp++;
            operand_stack.push(cur);
        } 
        else {
           if(operator_stack.size() == 0) {
			   if(c == '(') 
					level++;
               operator_stack.push(cur);
           } 
           else {
               top_node = operator_stack.top();
               if(operator_stack.size() > 0)
                    top_op = top_node->u.opera;

               if(c == '(') {
                    level++;
                    operator_stack.push(cur);
               } 
               else if( c == ')') {
					node_t *prev_node = NULL;

                    while(top_op != '(') {
                        if(operand_stack.size() >= 2) {
                            node_t *right = operand_stack.top();
                            operand_stack.pop();
                            node_t *left = operand_stack.top();
                            operand_stack.pop();

                            cons_expr(top_node,left,right);
                            operand_stack.push(top_node);
                        }
                        operator_stack.pop();
						
                        top_node = operator_stack.top();
                        top_op = top_node->u.opera;
                    }
                    
					prev_node = operand_stack.top();
					operand_stack.pop();

					cons_expr(top_node,prev_node,NULL);
					operand_stack.push(top_node);
                    operator_stack.pop();
                    level--;
               }
               else if(IS_HIGHER_PRIORITY(c,top_op)) {
                    operator_stack.push(cur);
               } 
               else {
                    while(!IS_HIGHER_PRIORITY(c,top_op) && top_op != '(') {
                        if(operand_stack.size() >= 2) {
                            node_t *right= operand_stack.top();
                            operand_stack.pop();
                            node_t *left = operand_stack.top();
                            operand_stack.pop();
                            cons_expr(top_node,left,right);
                            operand_stack.push(top_node);
                        }
                        operator_stack.pop();
                        if(operator_stack.size() == 0)
                            break;
                        
                        top_node = operator_stack.top();
                        top_op = top_node->u.opera;
                    }
                    operator_stack.push(cur);
               }
           }
        }
    }


    while(operator_stack.size() > 0) {
        top_node = operator_stack.top();
        if(operand_stack.size() >= 2) {
            node_t *right = operand_stack.top();
            operand_stack.pop();
            node_t *left = operand_stack.top();
            operand_stack.pop();
            cons_expr(top_node,left,right);
            operand_stack.push(top_node);
        }
        operator_stack.pop();
    }
	
    root = operand_stack.top();
	operand_stack.pop();

    return root;
}

node_t *find_node(node_t *cur, int target_index,bool first)
{
	node_t *parent = cur;
	int i = 0;
	node_t *left_most = NULL;
	
	if(cur == NULL)
		return NULL;

	left_most = cur;
	
	node_t *ret = left_most;

	while(left_most->left != NULL ) {
		if(left_most->left->type != node_type_operator) {
			ret = left_most->left;
			break;
		}
		
		if(left_most->left->bra_level != left_most->bra_level) {
			ret = left_most->left;
			break;
		}

		if(!IS_EQUAL_PRIORITY(left_most->left->u.opera, left_most->u.opera)) {
			ret = left_most->left;
			break;
		}
		left_most = left_most->left;
	}
	
//	if(left_most->left != NULL)
	i++;

	//if cur is a operand.op(1,"b")="b"
	if(i == target_index)
		return left_most->left == NULL ? left_most:left_most->left;

	parent = left_most;

	while(parent != NULL) {
		if(parent->right != NULL) {
			i++;
			ret = parent->right;
			if(i == target_index)
				return parent->right;
		} else 
			break;

		parent = parent->parent;
	}

	return ret;
}


static void print_node(node_t *node)
{
    if(node == NULL)
        return;
    
    int level_diff = 0;
	bool is_bracket = false;

    if(node->parent) {
        level_diff = node->bra_level - node->parent->bra_level;
    } else
        level_diff = node->bra_level;
    
	//int i;
   // for(i = 0; i < level_diff; i++) {
        //cout << "(";
    //}

	if(node->type == node_type_operator && node->u.opera == '(')
		is_bracket = true;

	if(is_bracket)
		cout << "(";
    print_node(node->left);
	
	if(!is_bracket) {
		if(node->type == node_type_operator)
			cout << node->u.opera;
		else
			cout << node->u.operand;
	}
    
    print_node(node->right);
   // for(i = 0; i < level_diff; i++) {
       // cout << ")";
    //}
	if(is_bracket)
		cout << ")";

}

static void clean_nodes(node_t *root)
{
    node_t *cur = root;
    if(root == NULL)
        return;

    clean_nodes(root->left);
    clean_nodes(root->right);
    del_node(root);
    
}



static void parse_and_do_cmds(char *cmds,node_t *root,char *expr_name)
{
	char *cur = cmds;
	char *del = strchr(cmds,' ');
	int i = 0;
	bool first = true;
	char res[1024] = {0};
	char *pre_res = NULL;
	node_t *cur_node = root;

	while(del != NULL && *del != '\0') {
		*del = '\0';
		i = atoi(cur);

		if(first) {
			strcpy(res,"op");
			strcat(res,"(");
			strcat(res,cur);
			strcat(res,",");
			strcat(res,expr_name);
			strcat(res,")");
			pre_res = strdup(res);
			cur_node = find_node(root,i,first);
			first = false;
		} else {
			strcpy(res,"op");
			strcat(res,"(");
			strcat(res,cur);
			strcat(res,",");
			strcat(res,pre_res);
			strcat(res,")");
			if(pre_res) free(pre_res);
			pre_res = strdup(res);
			cur_node = find_node(cur_node,i,first);
		}
		cur = del + 1;
		del = strchr(cur,' ');
	}
	
	i = atoi(cur);
	strcpy(res,"op");
	strcat(res,"(");
	strcat(res,cur);
	strcat(res,",");

	if(!first)
		strcat(res,pre_res);
	else
		strcat(res,expr_name);

	strcat(res,")");
	if(pre_res) free(pre_res);
	cur_node = find_node(cur_node,i,first);
	cout << res << "=";
	print_node(cur_node);
	cout << endl;
}

int main(int argc,char *argv[])
{
    char input[1024] = {0};
	bool is_first = true;

	while(1)
	{
		cin.getline(input,1024);
		if(input[0] == '*')
			break;

		char *tmp = strchr(input,':');
		if(tmp == NULL) {
			cerr << "bad input" << input << endl;
			continue;
		}
		char *expr = tmp + 2;
		*tmp = '\0';
		char *expr_name = input;
		
		if(!is_first)
			cout << endl;
		is_first = false;
		node_t *root = synax_tree(expr);
		cout << "Expression " << expr_name << ":" << endl;
		int ops_number;
		char c;
		//scanf("%d",&ops_number);
		cin>>ops_number;
		cin.get(c);
		while(ops_number-- > 0) {
			char ops_idxs[1024] = {0};
		//	scanf("%s",ops_idxs);
		//	gets(ops_idxs);
			cin.getline(ops_idxs,1024);
			parse_and_do_cmds(ops_idxs,root,expr_name);
		}
		clean_nodes(root);
		operand_stack.empty();
		operator_stack.empty();
		top_node_list.clear();
	}
//	system("@pause");
    return 0;
}

