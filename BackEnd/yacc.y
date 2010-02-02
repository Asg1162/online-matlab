%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "parser.h"
#include <string.h>
#include <assert.h>
#include "include/Matlab.h"
#include "include/Matrix.h"
#include "include/ParseException.h"
#include "include/ExeException.h"
#include <regex.h>
#include <sstream>

  using namespace ONLINE_MATLAB;
/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id(char *i);
nodeType *con(OM_SUPPORT_TYPE value);
nodeType *matrix(char *m);
nodeType *matrix_list(char *m);
nodeType *index_range(char *m);
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
 extern bool gEnableOutput;
%}



%union {
  OM_SUPPORT_TYPE nuValue;                 /* integer value */
  char *matrix;
  char *matrix_list;
  char *varName;                /* symbol table index */
  nodeType *nPtr;             /* node pointer */
  char *index_range;

};

%token <nuValue> NUMBER
%token <matrix> MATRIX
%token <matrix_list> MATRIX_LIST
%token <varName> VARIABLE
%token <index_range> INDEX_RANGE

 //%token <nPtr> NUMBER VARIABLE 
%token COMMA

 /* %expect 6 */
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
function stmt         {
  Matrix *m;
  try {
    m = execute($2); 
  }catch(ParseException &e)
     {
       // free node
       freeNode($2);
       throw; 
     }
  if ($2->type == typeFun && strcmp($2->fun.func, "plot") == 0)
    {
      char *filepath = (char *)m; //static_cast<char *>(m);
      gOutput << filepath;
      delete [] filepath;
    }
  else 
    {
      if (m->getName() == 0) // if the output is not assigned to anyone
        {
          gMatlab->getUser(gCurUser)->updateVar("ans", m);  // set to ans
          //      if ($2->type == typeOpr)
          //        $2->opr.op[0]->myMatrix = 0; // set to 0 in order not to free memory
          //      else if ($2->type == typeFun)
          $2->myMatrix = 0;
        }

      freeNode($2);
      // generate the output
      if (gEnableOutput)
        {
          Matrix *next = m;
          while(next)  {
            next->streamOut(gOutput);
            next = next->getNext();
          }
        }
    }

}
  // Matrix *tmp = execute($2); 
  //  updateVar("ans")(tmp);//}
        | /* NULL */
        ;

//num_seq:
//num_seq, NUMBER  {}

stmt:
          ';'                            { 
            $$ = opr(';', 2, NULL, NULL); 
          }
        | expr ';'                       { 
          $$ = $1; 
          }
        | expr {
          gEnableOutput = true;
          $$ = $1; 
        }
// TODO combine the following two cases
        | VARIABLE '=' expr ';'          {  
          $$ = opr('=', 2, id($1), $3); 
          }
        | MATRIX_LIST '=' expr ';'        { 
          $$ = opr('=', 2, matrix_list($1), $3);
          }
        | VARIABLE '=' expr         { 
          $$ = opr('=', 2, id($1), $3); 
          gEnableOutput = true;
          }
        | MATRIX_LIST '=' expr     { 
          $$ = opr('=', 2, matrix_list($1), $3);
          gEnableOutput = true;
          }

        ;

/*stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;
*/


expr:
        MATRIX		{ 
          $$ = matrix($1);   
          free($1);  
          }
        | MATRIX_LIST   { 
          $$ = matrix_list($1); 
          free($1); 
          }
        | VARIABLE '\''    { 
          $$ = opr('\'', 1, id($1));
          free($1); 
          }
        | VARIABLE     { 
          $$ = id($1); 
          free($1); 
          }
        | '-' expr %prec UMINUS {
          $$ = opr(UMINUS, 1, $2);
          }
        | expr '+' expr { 
          $$ = opr('+', 2, $1, $3); 
          }
        | expr '-' expr  { 
          $$ = opr('-', 2, $1, $3);
          }
        | expr '*' expr  { 
          $$ = opr('*', 2, $1, $3); 
          }
        | expr '/' expr  { 
          $$ = opr('/', 2, $1, $3); 
          }
        | VARIABLE '(' ')'  {
          assert(0);  // TODO
          $$ = func($1, 0);
         }
        | VARIABLE '(' arg_list ')'  { 
          $$ = func($1, $3);
          g_arguments = 0;
          } // function
        | INDEX_RANGE {
          $$ = index_range($1);
          free($1);
          }
        | NUMBER      { 
            $$ = con($1); 
          }


/*      | expr '<' expr         { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr         { $$ = opr('>', 2, $1, $3); }
        | expr GE expr          { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr          { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr          { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr          { $$ = opr(EQ, 2, $1, $3); }
        | '(' expr ')'          { $$ = $2; }
        ;*/

arg_list:
	expr     {
      $$ =  arg($1);
    }
    | arg_list ',' expr     { 
      $$ = arg($3);
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

  
  int numCol = totalNum/numRows;

  nodeType *p  = allocateNode(sizeof(nodeType), typeVec);

  //  p->vec.dim = numRows>1 ? 2: 1;
  p->vec.dim = 2; //numRows>1 ? 2: 1;
  p->vec.dims = (int *)malloc(sizeof(int)* p->vec.dim);
  //  if (p->vec.dim == 1)
  //    p->vec.dims[0] = numCol;
  //  else
    {
      p->vec.dims[0] = numCol;
      p->vec.dims[1] = numRows; 
    }

  p->vec.elements = (OM_SUPPORT_TYPE *)calloc(totalNum, sizeof(OM_SUPPORT_TYPE));
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

  result = regexec(right_mark, strToRead, 1, pmatch, 0);
  assert(result == 0);
  strToRead[pmatch[0].rm_eo-1] = ' ';

  while(1)
    {
      int result = regexec(comma, strToRead, 1, pmatch, 0);
      if((result != 0) || (pmatch[0].rm_so == -1))
        break;

      strToRead[strt_pointer + pmatch[0].rm_eo-1] = ' ';
      strt_pointer += pmatch[0].rm_eo;
      strToRead = m + strt_pointer;
    }

  strToRead = m;
  strt_pointer = 0;

  nodeType *p(0);
  nodeType *cached_p(0);
  while(1)
    {
      int result = regexec(variable, strToRead, 1, pmatch, 0);
      if((result != 0) || (pmatch[0].rm_so == -1))
            break;

      numMatrix++;
      strt_pointer = pmatch[0].rm_eo;
      strToRead[strt_pointer] = '\0';

      if (p != 0) 
        cached_p = p;

      p = id(strToRead+pmatch[0].rm_so); // deep copy?
      p->id.next = cached_p;  // reverse order       
      strToRead += strt_pointer+1;
    }
  p->id.list_size = numMatrix;
  
  free(left_mark);
  free(right_mark);
  free(comma);
  free(variable);

  /* allocate node */
  return p;
}

nodeType *index_range(char *m)
{
  /* allocate node */
  nodeType *p  = allocateNode(sizeof(nodeType), typeIndexRange);

  p->index.free = false;
  /* copy information */

  /* parsing the index_rangexs */
  // 1. find the number of ":"
  int numSemiColon = 0;
  for (int i = 0; i != strlen(m); i++)
    if (m[i] == ':') numSemiColon++;

  if (numSemiColon == 1)
    p->index.step = 1.0; // step is by default 1.0
  
  // find the numbers before the first ":"
  regmatch_t pmatch[1];
  regex_t  *numbers = (regex_t *)malloc(sizeof(regex_t));
  int ret = regcomp(numbers, "[-+]?[0-9]*\\.?[0-9]+", REG_EXTENDED);

  char *strToRead = m;
  int numNumbers = 0;
  OM_SUPPORT_TYPE fnumbers[3];
  while(1)
    {
      int result = regexec(numbers, strToRead, 1, pmatch, 0);
      if((result != 0) || (pmatch[0].rm_so == -1))
        break;
      int len = pmatch[0].rm_eo - pmatch[0].rm_so ;
      char number[len+1];
      memcpy(number, &strToRead[pmatch[0].rm_so], len);
      number[len] = '\0';
      fnumbers[numNumbers] = atof(number);
      strToRead += pmatch[0].rm_eo;
      numNumbers++;
    }

  if (numNumbers == 2)
    {
      p->index.step = 1.0; // step is by default 1.0
      p->index.start = fnumbers[0];
      p->index.end = fnumbers[1];
    }
  else if (numNumbers == 3)
    {
      p->index.start = fnumbers[0];
      p->index.step = fnumbers[1];
      if (p->index.step == 0.0f)
        throw ExeException("step size can't be zero.");
      p->index.end = fnumbers[2];
    }
  else
    {
      p->index.free = true;  // only one semi colon is found. it is a free matrix, depending on its parent's dimension
    }

  free(numbers);
  return p;
}

nodeType *id(char *i) {

    /* allocate node */
    nodeType *p  = allocateNode(sizeof(nodeType), typeId);

    /* copy information */
    p->id.id = strdup(i);
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
    int i;

    if (!p) return;

    switch(p->type) {
    case typeCon: 
      if (p->myMatrix)
        {
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
      free(p->id.id);
      break;
    case typeOpr:
      if (p->opr.oper == '*') 
        ;
      if (p->myMatrix)
        delete (Matrix *)(p->myMatrix);

      for (i = 0; i < p->opr.nops; i++)
        freeNode(p->opr.op[i]);
      break;
    case typeFun:
      {
        nodeType *cur = p->fun.arglist;
        for (i = 0; i < p->fun.noargs; i++)
          {
            nodeType *cur1 = cur;
            cur = cur1->arg.next;
            freeNode(cur1);
          }
        free(p->fun.func);
        break;
      }
      break;
    case typeArg:
      freeNode(p->arg.arg);
      break;
    case typeIndexRange:
      if (p->myMatrix)
        delete (Matrix *)(p->myMatrix);
      break;
    default:
      assert(0);
    }

    // free myself
    free (p);
}

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
    string reason(s);
    throw (ParseException(s));
}

//int main(void) {
//}


#include <stdio.h>
#include "yacc.tab.h"

Matrix *execute(nodeType *p) {
    if (!p) return 0;
    switch(p->type)
      {
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
          Matrix *m = new Matrix(NULL, p->vec.dim, p->vec.dims, p->vec.elements);
          p->myMatrix = (void *)m;  // save it, for free in future
          //          delete m;
          return m;
        }
      case typeId:
        {
          return gMatlab->getUser(gCurUser)->getVar(p->id.id);
        }
      case typeOpr:;
         switch(p->opr.oper) {
         case '=': 
           {
           /* MYTODO */
           assert(p->opr.op[0]->type == typeId);
           int noRtns = p->opr.op[0]->id.list_size;           
           if (noRtns == 1)
             {
               Matrix *rtn =  execute(p->opr.op[1]);
               int found = gMatlab->getUser(gCurUser)->updateVar(p->opr.op[0]->id.id, rtn);
               p->opr.op[0]->myMatrix = 0;

               p->opr.op[1]->myMatrix = 0; // set to 0 in order not to free memory
               //   if (p->opr.op[1]->type == typeVec || p->opr.op[1]->type == typeCon)
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
                   gMatlab->getUser(gCurUser)->updateVar(outputs->id.id, rtns[i]);
                   outputs = outputs->id.next;
                 }
               p->opr.op[0]->myMatrix = 0; // set to 0 in order not to free memory
               return rtns[noRtns-1];
             }

           }
         case '*': 
           {
             p->myMatrix = *(execute(p->opr.op[0])) * (*execute(p->opr.op[1])); // set to 0 in order not to free memory
             return (Matrix *)p->myMatrix;
           }
         case '+': 
           {
             p->myMatrix = *(execute(p->opr.op[0])) + (*execute(p->opr.op[1])); // set to 0 in order not to free memory
             return (Matrix *)p->myMatrix;
           }
         case '-': 
           {
             p->myMatrix = *(execute(p->opr.op[0])) - (*execute(p->opr.op[1])); // set to 0 in order not to free memory
             return (Matrix *)p->myMatrix;
           }
         case '\'':
           {
             p->myMatrix = (execute(p->opr.op[0]))->transpose();
             return (Matrix *)p->myMatrix;
           }
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
              if (firstArg->arg.arg->type == typeIndexRange)
                {
                  firstArg->arg.arg->index.parent = p; // set its parent
                  firstArg->arg.arg->index.argIdx = p->fun.noargs - i - 1; 
                }
              assert(firstArg->type == typeArg);
              ms[p->fun.noargs-i-1] = execute(firstArg);
              firstArg = firstArg->arg.next;
            }
          p->myMatrix = (Matrix *)gMatlab->getUser(gCurUser)->runFunction(p->fun.func, p->fun.nortns, p->fun.noargs, &(ms[0])); //, matrix);
          return (Matrix *)p->myMatrix;
        }
      case typeIndexRange:
        {
          if (!p->index.free)
            {
              int dim[2];
              dim[0] = 1;
              dim[1] = (int)((p->index.end - p->index.start + p->index.step/2)/p->index.step + 1);
              if (dim[1] < 0)
                throw ExeException("Matrix index setup error.");
              OM_SUPPORT_TYPE elements[dim[1]];
              elements[0] =  p->index.start;
              for (int i = 1; i < dim[1]; i++)
                elements[i] = elements[i-1] + p->index.step;
              
              const OM_SUPPORT_TYPE *ele = elements;
              p->myMatrix = (void *)new Matrix(NULL, 2, dim, ele);
              return (Matrix *)p->myMatrix;
            }
          else // else it is just ":", need to consult its parent
            {
              char *varName = p->index.parent->fun.func;
              Matrix *pMatrix = gMatlab->getUser(gCurUser)->getVar(varName);
              if (pMatrix == 0)
                {
                  std::string error("can't find variable ");
                  std::string var(varName);
                  throw ExeException(error+var);
                }
              short index = p->index.argIdx;
              int dim = pMatrix->getDimAt(index);
              if (dim != 0)
                {
                  OM_SUPPORT_TYPE elements[dim];
                  for (int i = 0; i != dim; i++)
                    elements[i] = (OM_SUPPORT_TYPE)(i + 1);
                  const OM_SUPPORT_TYPE *ele = elements;
                  int dims[2];
                  dims[0] = 1; dims[1] = dim;
                  p->myMatrix = new Matrix(NULL, 2, dims, ele);
                  return (Matrix *)p->myMatrix;
                }
              else // out of range
                return NULL;
            }
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
