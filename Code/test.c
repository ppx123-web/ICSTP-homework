#include "debug.h"
#include "data.h"

#define TEST(A) test_ ## A
#define ASSIGN(A) .A = TEST(A),

static void test_Struct(Node_t * cur,int deep) {

}

static void test_main() {
    //test->symbol_table();
    //test_Struct(tree->root,0);
}

static void test_display_symbol_table() {

}

static void test_display_symbol_stack() {
    SymbolStack_ele_t * cur = symbol_stack->first.next;
    while (cur != &symbol_stack->last) {
        for(unit_t * node = cur->head.scope_next;node != &cur->tail;node = node->scope_next) {
            symbol_table->display_node(node);
            printf("\n");
        }
        cur = cur->next;
    }
}

static void test_symbol_table() {
    int size = 1000;
    for(int i = 0;i < size;i++) {
        unit_t * n1 = new(unit_t);
        n1->deep = 1;
        n1->type = NULL;
        sprintf(n1->name,"node%d",i);
        symbol_table->insert(n1);
    }
    char name[32];
    for(int i = 3;i < size;i += 20) {
        sprintf(name,"node%d",i);
        //printf("%s\n",symbol_table->find(name)->name);
        assert(strcmp(symbol_table->find(name)->name,name) == 0);
    }
    assert(symbol_table->cnt == 0);
}



MODULE_DEF(Test_t,test) = {
        ASSIGN(main)
        ASSIGN(Struct)
        ASSIGN(display_symbol_table)
        ASSIGN(display_symbol_stack)
        ASSIGN(symbol_table)
};