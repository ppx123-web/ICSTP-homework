#include "data.h"
#include "debug.h"

/*
 * 遍历语法分析树，遇到ExtDef和Def时构建类型，加入symbol table中
 *              遇到Exp检查是否有语义错误
 */

#define Assign(A) .A =  Semantic_Check_ ## A,


//extern


//extern
static void ErrorHandling(int type,int line) {
    switch (type) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            printf("Error type 15 at Line %d: redefinition\n", line);
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        case 10:
            break;
        case 11:
            break;
        case 12:
            break;
        case 13:
            break;
        case 14:
            break;
        case 15:
            printf("Error type 15 at Line %d: struct field\n", line);
            break;
        case 16:
            break;
        case 17:
            break;
        default:
            panic("Wrong error type");
    }
}


static void Semantic_Check_init();
static void Semantic_Check_main(Node_t * root);
static FieldList * Semantic_Check_gettype(Node_t * cur);

static void Semantic_Check_Exp(Node_t * root);

static void Semantic_Check_CompSt(Node_t * root);

static void Semantic_Check_ExtDefList(Node_t * root);
static void Semantic_Check_ExtDef(Node_t * root);
static FieldList * Semantic_Check_ExtDecList(Node_t * root,const FieldList * );
static FieldList * Semantic_Check_ExtDec(Node_t *,const FieldList * );
static unit_t * Semantic_Check_FunDec(Node_t *,const FieldList * field);
static FieldList * Semantic_Check_VarList(Node_t * root);
static FieldList * Semantic_Check_ParamDec(Node_t * root);


static FieldList * Semantic_Check_DefList(Node_t * cur);
static FieldList * Semantic_Check_Def(Node_t * cur);
static FieldList * Semantic_Check_DecList(Node_t * cur,const FieldList * );
static FieldList * Semantic_Check_Dec(Node_t * cur,const FieldList * );
static FieldList * Semantic_Check_VarDec(Node_t * root,const FieldList * );
static FieldList * Semantic_Check_Struct(Node_t * root);


static Semantic_Check_t Semantic_Check = {
        Assign(init)
        Assign(main)
        Assign(gettype)

        Assign(Struct)
        .ErrorHandling = ErrorHandling,
};

Semantic_Check_t * semantic_check = &Semantic_Check;

static void Semantic_Check_init() {
    symbol_table->init(0x3ff);
    symbol_stack->init();
    symbol_stack->push(symbol_stack->node_alloc(GLOB_FIELD));

//    type_table->init();
}

/*
Specifier : TYPE
     | StructSpecifier
     ;
StructSpecifier : STRUCT OptTag LC DefList RC
    | STRUCT Tag
    ;
OptTag : ID
    |
    ;
Tag : ID
*/
static Type Int_Type = {
        .kind = BASIC,
        .u.basic = 0,
};

static FieldList Int_Field = {
        .name[0] = '\0',
        .type = &Int_Type,
        .tail = NULL,
};

static Type Float_Type = {
        .kind = BASIC,
        .u.basic = 1,
};

static FieldList Float_Field = {
        .name[0] = '\0',
        .type = &Float_Type,
        .tail = NULL,
};


static FieldList *  Semantic_Check_gettype(Node_t * cur) {
    FieldList * ret;
    if(type(cur->lchild,"TYPE")) {
        if(strcmp(cur->lchild->text,"int") == 0) {
            ret = & Int_Field;
        } else if(strcmp(cur->lchild->text,"float") == 0) {
            ret = & Float_Field;
        } else {
            assert(0);
        }
    } else if(type(cur->lchild->lchild,"STRUCT")) {
        if(type(cur->lchild->rchild,"RC")) {
            symbol_stack->push(symbol_stack->node_alloc(STRUCT_FIELD));
            ret = Semantic_Check_Struct(cur->lchild);
            symbol_stack->pop();
        } else {
            unit_t * temp = symbol_table->find(cur->rchild->lchild->right->lchild->text);
            if(temp == NULL) {
                panic("Not Find definition");
            } else {
                ret = temp->field;
            }
        }
    } else {
        panic("Wrong type");
    }
    return ret;
}

static void Semantic_Check_main(Node_t * root) {
    if(type(root->lchild,"ExtDefList")) {
        Semantic_Check_ExtDefList(root->lchild);
    } else {
        panic("Wrong");
    }
}

static void Semantic_Check_Exp(Node_t * root) {
    panic("Not implemented");
}

static void Semantic_Check_CompSt(Node_t * root) {
    panic("Not implemented");
}


static void Semantic_Check_ExtDefList(Node_t * root) {
    Semantic_Check_ExtDef(root->lchild);
    if(root->lchild->right) {
        Semantic_Check_ExtDefList(root->rchild);
    }
}

static void Semantic_Check_ExtDef(Node_t * root) {
    Node_t * cur = root->lchild->right;
    Node_t * specifier = root->lchild;
    FieldList * field = Semantic_Check_gettype(specifier);
    if(type(cur,"ExtDecList")) {
        //Var Dec list
        FieldList * list = Semantic_Check_ExtDecList(cur,field);
        while (list) {
            unit_t * node = new(unit_t);
            symbol_table->node_init(node,list->name);
            node->field = list;
            symbol_table->insert(node);
            list = list->tail;
        }
    } else if(type(cur,"SEMI")) {
        //struct Defination
        if(type(root->lchild->lchild,"TYPE")) return;
        //int;
        //以结构体的名称作为索引插入符号表
        unit_t * node = symbol_table->node_alloc();
        symbol_table->node_init(node,field->name);
        node->field = type_ops->field_copy(field);
        symbol_table->insert(node);
    } else if(type(cur,"FunDec")) {
        Semantic_Check_FunDec(root->lchild->right,field);
        if(type(cur->right,"CompSt")) {
            Semantic_Check_CompSt(root->rchild);
        } else if(type(cur->right,"SEMI")) {
            //Func Declaration
            //TODO()
            panic("Not implemented");
        }
    } else {
        panic("Wrong");
    }
}

static FieldList * Semantic_Check_ExtDecList(Node_t * root,const FieldList * field) {
    FieldList * ret = Semantic_Check_VarDec(root->lchild,field);
    FieldList * temp = ret;
    while (temp->tail) {
        temp = temp->tail;
    }
    if(root->rchild != root->lchild) {
        temp->tail = Semantic_Check_ExtDecList(root->rchild,field);
    }
    return ret;
}

static FieldList * Semantic_Check_ExtDec(Node_t * root,const FieldList * field) {
    FieldList * ret = Semantic_Check_VarDec(root->lchild,field);
    if(root->lchild->right) {
        ret->tail = Semantic_Check_ExtDec(root->rchild,field);
    }
    return ret;
}

static unit_t * Semantic_Check_FunDec(Node_t * root,const FieldList * field) {
    unit_t * ret = new(unit_t);
    ret->field = new(FieldList);
    ret->field->type = new(Type);
    symbol_table->node_init(ret,root->lchild->text);
    ret->field->tail = NULL;
    strcpy(ret->field->name,root->lchild->text);
    ret->field->type->u.func.ret_type = type_ops->field_copy(field);
    if(type(root->rchild->left,"VarList")) {
        ret->field->type->u.func.var_list = Semantic_Check_VarList(root->rchild->left);
    } else {
        ret->field->type->u.func.var_list = NULL;
    }
}

static FieldList * Semantic_Check_VarList(Node_t * root) {
    FieldList * ret = Semantic_Check_ParamDec(root->lchild);
    if(root->lchild != root->rchild) {
        ret->tail = Semantic_Check_VarList(root->rchild);
    } else {
        ret->tail = NULL;
    }
}

static FieldList * Semantic_Check_ParamDec(Node_t * root) {
    FieldList * field = Semantic_Check_gettype(root->lchild);
    return Semantic_Check_VarDec(root->rchild,field);
}

/*
 * 一个类型表
 * 类型表的复制、删除（删除暂时不用实现，struct的定义一定是全局定义）
 * 类型表的查询
 * 类型表的插入
 */
//需要的接口


/*
DefList : Def DefList
    |
    :
Def : Specifier DecList SEMI
    ;
DecList : Dec
    | Dec COMMA DecList
    ;
Dec : VarDec
    | VarDec ASSIGNOP Exp
    ;
 */

static FieldList * Semantic_Check_DefList(Node_t * cur) {
    if(cur == NULL) return NULL;
    FieldList * ret = Semantic_Check_Def(cur->lchild);
    FieldList * temp = ret;
    while (temp->tail) {
        temp = temp->tail;
    }
    temp->tail = Semantic_Check_DefList(cur->lchild->right);
    return ret;
}

static FieldList * Semantic_Check_Def(Node_t * cur) {
    FieldList * field = Semantic_Check_gettype(cur->lchild);
    FieldList * ret = Semantic_Check_DecList(cur->lchild->right,field);
    return ret;
}

static FieldList * Semantic_Check_DecList(Node_t * cur,const FieldList * field) {
    if(cur == NULL) return NULL;
    FieldList * ret = Semantic_Check_Dec(cur->lchild,field);
    if(cur->rchild != cur->lchild)
        ret->tail = Semantic_Check_DecList(cur->rchild,field);
    return ret;
}

static FieldList * Semantic_Check_Dec(Node_t * cur,const FieldList * field) {
    FieldList * ret = Semantic_Check_VarDec(cur->lchild,field);
    if(cur->lchild != cur->rchild && symbol_stack->top()->field_type == STRUCT_FIELD) {
        ErrorHandling(15,ret->line);
        //结构体内赋值初始化
    }
    unit_t * node = symbol_table->node_alloc();
    symbol_table->node_init(node,ret->name);
    if(symbol_stack->top()->field_type == STRUCT_FIELD) {
        node->field = type_ops->field_copy(ret);
    } else {
        node->field = ret;
    }
    symbol_table->insert(node);
    return ret;
}

/*
VarDec : ID
    | VarDec LB INT RB
*/
static FieldList * Semantic_Check_VarDec(Node_t * root,const FieldList * field) {
    FieldList * var_field = new(FieldList);
    var_field->type = new(Type);
    var_field->tail = NULL;
    int cnt = 0,nums[10] = {0};
    char * s;
    Node_t * cur;
    for(cur = root->lchild;cur->lchild != NULL;cur = cur->lchild) {
        nums[cnt++] = (int)strtol(cur->right->right->text,&s,0);
    }
    strcpy(var_field->name,cur->text);
    var_field->line = cur->line;
    Type * var_type = var_field->type;
    int i = 0;
    do {
        if(i == cnt) {
            if(field->type->kind == BASIC) {
                var_type->kind = BASIC;
                var_type->u.basic = field->type->u.basic;
            } else {
                var_type->kind = STRUCTURE;
                var_type->u.structure = type_ops->field_copy(field);
            }
        } else {
            var_type->kind = ARRAY;
            var_type->u.array.size = nums[cnt - i - 1];
            var_type->u.array.elem = new(Type);
            var_type = var_type->u.array.elem;
        }
        i++;
    } while (i <= cnt);
    return var_field;
}

/*
StructSpecifier : STRUCT OptTag LC DefList RC
*/
static FieldList * Semantic_Check_Struct(Node_t * root) {
    assert(type(root->lchild,"STRUCT") && type(root->rchild,"RC"));
    FieldList * field = new(FieldList);
    field->type = new(Type);
    field->tail = NULL;
    field->type->kind = STRUCTURE;
    field->type->u.structure = Semantic_Check_DefList(root->rchild->left);
    if(type(root->lchild->right,"LC")) {
        field->name[0] = '\0';
    } else {
        strcpy(field->name,root->lchild->right->lchild->text);
    }
    return field;
}