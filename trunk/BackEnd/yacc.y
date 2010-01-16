%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "parser.h"
#include <string.h>
#include <assert.h>
#include "include/Matlab.h"
#include "include/Matrix.h"
#include <regex.h>
#include <sstream>

  using namespace ONLINE_MATLAB;
/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id(char *i);
nodeType *con(OM_SUPPORT_TYPE value);
nodeType *matrix(char *m);
nodeType *matrix_list(char *m);
 nodeType *func(const char *fun, nodeType *firstArg);
 nodeType *arg(nodeType *node);

void freeNode(nodeType *p);
Matrix *execute(nodeType *p);
int yylex(void);

 void debug(char *s);
void yyerror(char *s);
// TODO remove int sym[26];                    /* symbol table */
extern int yy_flex_debug;
// int g_arg_list_size = 0;
 nodeType *g_arguments(0);

extern Matlab *gMatlab;
 extern std::string gCurUser;
 extern std::stringstream gOutput;
%}



%union {
  OM_SUPPORT_TYPE nuValue;                 /* integer value */
  char *matrix;
  char *matrix_list;
  char *varName;                /* symbol table index */
    nodeType *nPtr;             /* node pointer */

};

%token <nuValue> NUMBER
%token <matrix> MATRIX
%token <matrix_list> MATRIX_LIST
%token <varName> VARIABLE
 //%token <nPtr> NUMBER VARIABLE 
%token COMMA

 /*
%token WHILE IF PRINT
%nonassoc IFX
%nonassoc ELSE*/

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <nPtr> stmt expr arg_list

%%

program:
function                { ; }
        ;

function:
function stmt         { Matrix *m = execute($2); printf("free the tree.\n");    freeNode($2);
  Matrix *next = m;
  while(next)  {
    printf("streamout matrix %p.\n", next);
    next->streamOut(gOutput);
    next = m->getNext();
  }

}
  // Matrix *tmp = execute($2); 
  //  updateVar("ans")(tmp);//}
        | /* NULL */
        ;

//num_seq:
//num_seq, NUMBER  {}

stmt:
          ';'                            { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                       { $$ = $1; }
// TODO combine the following two cases
| VARIABLE '=' expr ';'          { printf("***** found assignment for %s *****\n", $1); $$ = opr('=', 2, id($1), $3);  }
| MATRIX_LIST '=' expr ';'        { printf("*** found assignment for %s. ****\n", $1);  $$ = opr('=', 2, matrix_list($1), $3);}

/*        | PRINT expr ';'                 { $$ = opr(PRINT, 1, $2); } */

/*        | WHILE '(' expr ')' stmt        { $$ = opr(WHILE, 2, $3, $5); } */
/*        | IF '(' expr ')' stmt %prec IFX { $$ = opr(IF, 2, $3, $5); } */
/*        | IF '(' expr ')' stmt ELSE stmt { $$ = opr(IF, 3, $3, $5, $7); } */
/*        | '{' stmt_list '}'              { $$ = $2; } */
        ;

/*stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;
*/


expr:
          NUMBER               { printf("convert number %f.\n", $1) ;$$ = con($1); }
        | MATRIX		{ $$ = matrix($1);   free($1);  }
| MATRIX_LIST   { $$ = matrix_list($1); free($1); }
| VARIABLE              { printf(" convert variable %s.\n", $1) ;$$ = id($1); free($1); }
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
        | expr '+' expr         { printf(" adding %s and %s.\n", $1->id.id, $3->con.value);  $$ = opr('+', 2, $1, $3); }
        | expr '-' expr         { $$ = opr('-', 2, $1, $3); }
        | expr '*' expr         { $$ = opr('*', 2, $1, $3); }
        | expr '/' expr         { $$ = opr('/', 2, $1, $3); }
	|VARIABLE '(' ')'       {
				  assert(0);
				  $$ = func($1, 0);
				 }
	| VARIABLE '(' arg_list ')'  { printf("found function %s", $1); 
        		 		   /*nodeType *n = g_arguments;
				            for (int i =0 ; i != g_arguments->arg.noargs; i++)
   				           {
			                printf("the %d the argument: %f.\n", i, n->arg.arg->con.value);
         			       n = n->arg.next;
				                }*/
 				           $$ = func($1, $3);
  				          g_arguments = 0;
			            //            | [ num_seq; num_seq ]
                                     } // function

/*      | expr '<' expr         { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr         { $$ = opr('>', 2, $1, $3); }
        | expr GE expr          { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr          { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr          { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr          { $$ = opr(EQ, 2, $1, $3); }
        | '(' expr ')'          { $$ = $2; }
        ;*/

arg_list:
	expr                  {
       				 printf("first expr %f \n", $1->con.value);
			        //        g_arg_list_size = 1; 
                     $$ =  arg($1);
			      }
        | arg_list ',' expr     { printf("found ',' \n"); 
                $$ = arg($3);
				        // TODO		  $$ = opr(',', 2, $1, $3); g_arg_list_size++;
        			}
        ;


%%


#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)

nodeType *allocateNode(int size, nodeEnum type)
{
  nodeType *p;
  if ((p = (nodeType *)malloc(size)) == NULL)
    yyerror((char *)"out of memory");

  p->myMatrix = 0;
  p->type = type;
  return p;
}

nodeType *con(OM_SUPPORT_TYPE value) {
  nodeType *p = allocateNode(sizeof(nodeType), typeCon);

    /* copy information */
    p->con.value = value;  // pass the text pointer
    return p;
}


nodeType *matrix(char *m){
  std::string smatrix(m);
  int numRows = 1;

  int strt_pointer = 0;

  regmatch_t pmatch[1];
  regex_t  *scomma = (regex_t *)malloc(sizeof(regex_t));
  regex_t  *numeric = (regex_t *)malloc(sizeof(regex_t));

  int ret = regcomp(scomma, ";", REG_EXTENDED);
  ret = regcomp(numeric, "[-+]?[0-9]*\\.?[0-9]+", REG_EXTENDED);
  char *strToRead = m;
  while(1)
    {
      int result = regexec(scomma, strToRead, 1, pmatch, 0);
      if((result != 0) || (pmatch[0].rm_so == -1))
            break;

      strt_pointer += pmatch[0].rm_eo;
      strToRead = m + strt_pointer;
      numRows++;
    }
  
  int totalNum(0);
  strToRead = m;
  strt_pointer = 0;
  while(1)
    {
      int result = regexec(numeric, strToRead, 1, pmatch, 0);
      if((result != 0) || (pmatch[0].rm_so == -1))
            break;

      strt_pointer += pmatch[0].rm_eo;
      strToRead = m + strt_pointer;
      totalNum++;
    }

  
  printf("%s there are %d rows.%d total num\n", m, numRows, totalNum);
  int numCol = totalNum/numRows;

  nodeType *p  = allocateNode(sizeof(nodeType), typeVec);

  p->vec.dim = numRows>1 ? 2: 1;
  p->vec.dims = (int *)malloc(sizeof(int)* p->vec.dim);
  if (p->vec.dim == 1)
    p->vec.dims[0] = numCol;
  else
    {
      p->vec.dims[0] = numRows;      
      p->vec.dims[1] = numCol;
    }

  p->vec.elements = (OM_SUPPORT_TYPE *)calloc(totalNum, sizeof(double));
  OM_SUPPORT_TYPE *element = p->vec.elements;

  strToRead = m;
  strt_pointer = 0;
  while(1)
    {
      int result = regexec(numeric, strToRead, 1, pmatch, 0);
      if((result != 0) || (pmatch[0].rm_so == -1))
            break;

      strt_pointer = pmatch[0].rm_eo;
      strToRead[strt_pointer] = '\0';
      *element = atof(strToRead+pmatch[0].rm_so);
      ++element;
      strToRead += strt_pointer + 1;
    }

  free(scomma); 
  free(numeric);


  return p;
}

nodeType *matrix_list(char *m){
  std::string smatrix(m);
  int numMatrix = 0;

  int strt_pointer = 0;

  regmatch_t pmatch[1];
  regex_t  *left_mark = (regex_t *)malloc(sizeof(regex_t));
  regex_t  *right_mark = (regex_t *)malloc(sizeof(regex_t));
  regex_t  *comma = (regex_t *)malloc(sizeof(regex_t));

  regex_t  *variable = (regex_t *)malloc(sizeof(regex_t));


  int ret = regcomp(comma, ",", REG_EXTENDED);
  ret = regcomp(left_mark, "\\[", REG_EXTENDED);
  ret = regcomp(right_mark, "\\]", REG_EXTENDED);
  ret = regcomp(variable, "[a-zA-Z]+", REG_EXTENDED);  // TODO make it more robust

  char *strToRead = m;
  int result = regexec(left_mark, strToRead, 1, pmatch, 0);
  assert(result == 0);
  strToRead[pmatch[0].rm_eo-1] = ' ';
  printf("after matching left mark string = %s.\n", strToRead);

  result = regexec(right_mark, strToRead, 1, pmatch, 0);
  assert(result == 0);
  strToRead[pmatch[0].rm_eo-1] = ' ';
  printf("after matching right mark string = %s.\n", strToRead);

  while(1)
    {
      int result = regexec(comma, strToRead, 1, pmatch, 0);
      if((result != 0) || (pmatch[0].rm_so == -1))
        break;

      strToRead[strt_pointer + pmatch[0].rm_eo-1] = ' ';
      printf("after matching semi comma string = %s.\n", strToRead);
      strt_pointer += pmatch[0].rm_eo;
      strToRead = m + strt_pointer;
    }

  strToRead = m;
  strt_pointer = 0;

  nodeType *p(0);
  nodeType *cached_p(0);
  while(1)
    {
      printf("checking variable from %s.\n", strToRead);
      int result = regexec(variable, strToRead, 1, pmatch, 0);
      if((result != 0) || (pmatch[0].rm_so == -1))
            break;

      numMatrix++;
      strt_pointer = pmatch[0].rm_eo;
      printf("converting %c to %c.\n", strToRead[strt_pointer], '\0');
      strToRead[strt_pointer] = '\0';

      if (p != 0) 
        cached_p = p;

      p = id(strToRead+pmatch[0].rm_so); // deep copy?
      p->id.next = cached_p;  // reverse order       
      strToRead += strt_pointer+1;
    }
  printf("setting list_size to %d.\n", numMatrix);
  p->id.list_size = numMatrix;
  
  free(left_mark);
  free(right_mark);
  free(comma);
  free(variable);

  /* allocate node */
  return p;
}

nodeType *id(char *i) {

    /* allocate node */
    nodeType *p  = allocateNode(sizeof(nodeType), typeId);

    /* copy information */
    p->id.id = strdup(i);
    printf("MEMORYDBG: allocate %p for %s.\n", p->id.id, p->id.id);
    p->id.list_size = 1;
    p->id.next = 0;

    gMatlab->getUser(gCurUser)->newVar(i);
    return p;
}

nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    int i;
    nodeType * p;
    /* allocate node */
    int nodeSize = SIZEOF_NODETYPE + sizeof(oprNodeType) +
        (nops - 1) * sizeof(nodeType*);

    p  = allocateNode(nodeSize, typeOpr);

    /* copy information */
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

nodeType *func(const char *fun, nodeType *firstArg) {
    size_t nodeSize;
    int i;
    nodeType *p;
    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(funNodeType);
    p = allocateNode(nodeSize, typeFun);

    /* copy information */
    p->fun.func = strdup(fun);
    p->fun.nortns = 1; // by default return only one matrix
    if (firstArg != 0)
      {
        p->fun.noargs = firstArg->arg.noargs;
        p->fun.arglist = firstArg;
      }
    else
      {
        p->fun.noargs = 0;
      }
    return p;
}

nodeType *arg(nodeType *node){
    nodeType *p;

    /* allocate node */
    int nodeSize = SIZEOF_NODETYPE + sizeof(argNodeType);
    p  = allocateNode(nodeSize, typeArg);

    /* copy information */
    p->arg.arg = node;
    if (g_arguments == NULL)
      {
        p->arg.noargs = 1;
        p->arg.next = 0;
      }
    else
      {
        p->arg.noargs = g_arguments->arg.noargs+1;
        p->arg.next = g_arguments;
      }
    g_arguments = p;
    return p;
}

void freeNode(nodeType *p) {
  printf("free node here.\n");
    int i;

    if (!p) return;

    switch(p->type) {
    case typeCon: 
      if (p->myMatrix)
        {
          printf(" *************** deleting a constant matrix ********** \n");
          if (p->myMatrix)
            delete (Matrix *)(p->myMatrix);
        }
      ;  // nothing to do
      break;
    case typeVec:
      free(p->vec.dims);
      free(p->vec.elements);
      if (p->myMatrix)
        delete (Matrix *)(p->myMatrix);
      break;
    case typeId:
      printf("MEMORYDBG: free %p for %s.\n", p->id.id, p->id.id);
      free(p->id.id);
      break;
    case typeOpr:
      for (i = 0; i < p->opr.nops; i++)
        freeNode(p->opr.op[i]);
      break;
    case typeFun:
      {
        nodeType *cur = p->fun.arglist;
        for (i = 0; i < p->fun.noargs; i++)
          {
            nodeType *cur1 = cur;
            printf("MEMORYDBG: free arglist\n");
            cur = cur1->arg.next;
            freeNode(cur1);
          }
        free(p->fun.func);
        break;
      }
      break;
    case typeArg:
      printf("MEMORYDBG: free arg.\n");
      freeNode(p->arg.arg);
      break;
    default:
      assert(0);
    }

    // free myself
    free (p);
}

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}

//int main(void) {
//}


#include <stdio.h>
#include "yacc.tab.h"

Matrix *execute(nodeType *p) {
    if (!p) return 0;
    printf("in execution type = %d...\n", p->type);
    switch(p->type)
      {
        //    case typeCon: printf("#########  in exec: converting floating point %s.\n", p->con.value);
      case typeCon:      
        {
          string constant("constant");
          Matrix *m = new Matrix(constant.c_str(), 2, 1, 1);
           m->setScalaValue(p->con.value);
           p->myMatrix = (void *)m; // save it, or free in future
           return m;
        }
      case typeVec:
        {
          string tmp("tmp");
          printf("create a temp matrix.\n");
          Matrix *m = new Matrix(tmp.c_str(), p->vec.dim, p->vec.dims, p->vec.elements);
          p->myMatrix = (void *)m;  // save it, for free in future
          printf(" return a temp matrix\n");
          return m;
        }
      case typeId:
        {
          return gMatlab->getUser(gCurUser)->getVar(p->id.id);
        }
      case typeOpr:;
         switch(p->opr.oper) {
         case '=': 
           printf("inside the = execution, number of return is %d.\n", p->opr.op[0]->id.list_size);
           {
           /* MYTODO */
           assert(p->opr.op[0]->type == typeId);
           int noRtns = p->opr.op[0]->id.list_size;           
           if (noRtns == 1)
             {
               printf("here.\n");
               Matrix *rtn =  execute(p->opr.op[1]);
               gMatlab->getUser(gCurUser)->updateVar(p->opr.op[0]->id.id, rtn);
               printf("here2.\n");
               p->opr.op[0]->myMatrix = 0; // set to 0 in order not to free memory
               if (p->opr.op[1]->type == typeVec)
                 p->opr.op[1]->myMatrix = 0;
               return rtn;
             }
           else if (noRtns > 1) 
             {
               assert(p->opr.op[1]->type == typeFun);
               assert(noRtns < 5); // any function return more than 5 matrix?
               p->opr.op[1]->fun.nortns = noRtns;
               // must return from a function
               Matrix *rtns[noRtns];
               Matrix *res = execute(p->opr.op[1]);
               for (int i = 0; i != noRtns; i++)
                 {
                   rtns[noRtns-1-i] = res;
                   res = res->getNext();

                 }
               nodeType *outputs = p->opr.op[0];
               for (int i = 0; i != noRtns; i++)
                 {
                   printf("updating variable %s.\n",outputs->id.id);
                   gMatlab->getUser(gCurUser)->updateVar(outputs->id.id, rtns[i]);
                   outputs = outputs->id.next;
                 }
               p->opr.op[0]->myMatrix = 0; // set to 0 in order not to free memory
               return rtns[0];
             }

           }
         case '*': 
           return *(execute(p->opr.op[0])) * (*execute(p->opr.op[1]));
         default:
           // TODO add more operators
           printf("unknown operator %c.\n", p->opr.oper);
           assert(0);
         } 
      case typeArg:
        return execute(p->arg.arg);
      case typeFun:
        {
          Matrix *ms[p->fun.noargs];
          nodeType *firstArg = p->fun.arglist;
          
          for (int i = 0; i < p->fun.noargs; ++i)
            {
              // reverse the order
              ms[p->fun.noargs-i-1] = execute(firstArg);
              assert(firstArg->type == typeArg);
              firstArg = firstArg->arg.next;
            }
          p->myMatrix = (Matrix *)gMatlab->getUser(gCurUser)->runFunction(p->fun.func, p->fun.nortns, p->fun.noargs, &(ms[0])); //, matrix);
          return (Matrix *)p->myMatrix;
        }

      default:
        printf("unknown type %d.\n", p->type);
        assert(0);
      }
    return 0;
}

#if 0
        case WHILE:     while(execute(p->opr.op[0])) execute(p->opr.op[1]); return 0;
        case IF:        if (execute(p->opr.op[0]))
                            execute(p->opr.op[1]);
                        else if (p->opr.nops > 2)
                            execute(p->opr.op[2]);
                        return 0;
        case PRINT:     printf("%d\n", execute(p->opr.op[0])); return 0;
        case ';':       execute(p->opr.op[0]); return execute(p->opr.op[1]);
        case '=':       return sym[p->opr.op[0]->id.i] = execute(p->opr.op[1]);
        case UMINUS:    return -execute(p->opr.op[0]);
        case '+':       return execute(p->opr.op[0]) + execute(p->opr.op[1]);
        case '-':       return execute(p->opr.op[0]) - execute(p->opr.op[1]);
        case '*':       return execute(p->opr.op[0]) * execute(p->opr.op[1]);
        case '/':       return execute(p->opr.op[0]) / execute(p->opr.op[1]);
        case '<':       return execute(p->opr.op[0]) < execute(p->opr.op[1]);
        case '>':       return execute(p->opr.op[0]) > execute(p->opr.op[1]);
        case GE:        return execute(p->opr.op[0]) >= execute(p->opr.op[1]);
        case LE:        return execute(p->opr.op[0]) <= execute(p->opr.op[1]);
        case NE:        return execute(p->opr.op[0]) != execute(p->opr.op[1]);
        case EQ:        return execute(p->opr.op[0]) == execute(p->opr.op[1]);
#endif

 void debug(char *s)
 {
   printf("%s", s);
   /* testing...  std::string input = "x = 12.345 + a * rand(1, 2, 3) + rand(4, 5);";

        YY_BUFFER_STATE my_string_buffer  = yy_scan_string(input.c_str());*/

 }
