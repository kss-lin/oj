#include <iostream>
#include <vector>

using namespace std;

int ca,cb,target;


static void fill_a(unsigned short& a, unsigned short& b)
{
	a = ca;
}

static void fill_b(unsigned short& a, unsigned short& b)
{
	b = cb;	
}

static void empty_a(unsigned short& a, unsigned short& b)
{
	a = 0;
}

static void empty_b(unsigned short& a, unsigned short& b)
{
	b = 0;
}

static void pour_ba(unsigned short& a, unsigned short& b)
{
	int left = ca > a ? ca - a : 0;
	if(b > left) {
		b -= left;
		a += left;
	} else {
		a += b;
		b = 0;
	}
}

static void pour_ab(unsigned short& a, unsigned short& b)
{
	int left = cb > b ? cb - b : 0;
	if(a > left) {
		a -= left;
		b += left;
	} else {
		b += a;
		a = 0;
	}
}

typedef void (*fn_t)(unsigned short& a,unsigned short& b);

static fn_t cmds[6] = {fill_a,pour_ab,fill_b,pour_ba,empty_a,empty_b};


bool need_extent(int a,int b,int pre_ops,int cur_ops)
{
	if(pre_ops == cur_ops)
		return false;
	
	if(pre_ops == 0)
		return cur_ops != 3 && cur_ops != 4 && cur_ops != 2;
	if(pre_ops == 2)
		return cur_ops != 5 && cur_ops != 1 && cur_ops != 0;
	if(pre_ops == 4)
		return cur_ops != 0 && cur_ops != 1 && cur_ops != 5;
	if(pre_ops == 5)
		return cur_ops != 2 && cur_ops != 3 && cur_ops != 0;
	if(a == 0)
		return cur_ops != 1 && cur_ops != 4 && cur_ops != 5;
	if(b == 0)
		return cur_ops != 2 && cur_ops != 5 && cur_ops != 4;
	if(a == ca)
		return cur_ops != 3 && cur_ops != 0 && cur_ops != 2;
	if(b == cb)
		return cur_ops != 1 && cur_ops != 2 && cur_ops != 0;
	return true;
}

struct node_t;
static node_t *head = NULL;
static node_t *tail = NULL;

static node_t* pop_front();
static void push_back(node_t *node);

struct chk_t;
node_t* chk_alloc(chk_t *chk);

chk_t *chk = NULL;
chk_t *last_chk = NULL;

#pragma pack (1)
typedef struct node_t
{
	node_t *next;
	vector<char> v_print_track;
	unsigned short a;
	unsigned short b;
	char ops;

	node_t()
	{
		a = 0;
		b = 0;
		next = NULL;
	}
	
	void extent()
	{
		for(int i = 0; i < 6; i++) {
			if(!need_extent(a,b,ops,i))
				continue;

			node_t *child = chk_alloc(chk);
			child->a = a;
			child->b = b;
			child->ops = i;
			child->eval();
			push_back(child);

			for(int j = 0; j < v_print_track.size(); j++) {
				child->v_print_track.push_back(v_print_track[j]);
			}
			child->v_print_track.push_back(i);
		}
	}
	
	void eval()
	{
		cmds[ops](a,b);
	}

	bool finish(int target)
	{
		return target == b;
	}
	
}node_t;
#pragma pack()

struct node_free_list_t
{
	node_t	*head;
	node_t  *tail;
};

node_free_list_t flist = {NULL,NULL};

#define CHUNK_SIZE   512

struct chk_t
{
	chk_t *next;
	chk_t() {cnt = 0; next = NULL; }
	node_t nodes[CHUNK_SIZE];
	int cnt;
};

chk_t *chk_init()
{
	return new chk_t;
}

node_t* chk_alloc(chk_t *chk)
{
	if(flist.head) {
		node_t *node = flist.head;
		flist.head = node->next;
		return node;
	}
	if(chk->cnt < CHUNK_SIZE) {
		return &chk->nodes[chk->cnt++];
	}
	
	if(last_chk == NULL || last_chk->cnt >= CHUNK_SIZE) {
		chk_t *next_chk = new chk_t;
		if(last_chk == NULL) {
			last_chk = next_chk;
			chk->next = last_chk;
		} else {
			last_chk->next = next_chk;
			last_chk = next_chk;
		}
	}
	return &last_chk->nodes[last_chk->cnt++];
}

void chk_free(node_t *node)
{
	node->next = NULL;
	node->v_print_track.clear();
	if(flist.head == NULL) {
		flist.tail = flist.head = node;
	} else {
		flist.tail->next = node;
		flist.tail = node;
	}

}

void chk_clean(chk_t *chk)
{
	while(chk != NULL) {
		chk_t *tmp = chk;
		chk = tmp->next;
		delete tmp;
	}
	last_chk = NULL;
	flist.head = flist.tail = NULL;
}

static node_t* pop_front()
{
	node_t *cur = head;
	if(cur)
		head = cur->next;
	return cur;
}


static void push_back(node_t *node)
{
	if(tail) {
		tail->next = node;
		tail = node;
	}
}

static void init() 
{
	chk = chk_init();

	head = chk_alloc(chk);
	head->ops = 0;
	head->eval();
	head->v_print_track.push_back(0);

	tail = head;
	node_t *t = chk_alloc(chk);
	t->ops = 2;
	t->v_print_track.push_back(2);
	push_back(t);
	t->eval();
}

static void print(node_t *curnode)
{
	for(int i = 0; i < curnode->v_print_track.size(); i++) {
		switch(curnode->v_print_track[i]) {
		case 0:cout << "fill A" << endl; break;
		case 1:cout << "pour A B" << endl; break;
		case 2:cout << "fill B" << endl; break;
		case 3:cout << "pour B A" << endl; break;
		case 4:cout << "empty A" << endl; break;
		case 5:cout << "empty B" << endl; break;
		default:cout << "unknown" << endl;break;
		}
	}
	cout << "success" << endl;
	chk_clean(chk);
}

static void BFS()
{
	init();
	node_t *p = pop_front();

	while(p != NULL) {
		if(p->finish(target)) {
			print(p);
			return;
		}
		
		p->extent();
		chk_free(p);
		p = pop_front();
	}

}

int main(int argc,char *argv[])
{
	while(cin >> ca >> cb >> target) {
		BFS();
	}
	return 0;
}