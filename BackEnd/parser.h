#pragma once

#ifndef PARSER_H
#define PARSER_H

#include <string>

typedef enum { typeCon, 
               typeVec, 
               typeId, 
               typeOpr, 
               typeFun, 
               typeArg,
               typeIndexRange,
               typeString
} nodeEnum;

typedef float OM_SUPPORT_TYPE;

/* constants */
typedef struct {
    OM_SUPPORT_TYPE value;                  /* value of constant */
} conNodeType;

typedef struct {
    int dim;
    int *dims;
    OM_SUPPORT_TYPE *elements;
} vecNodeType;

/* identifiers */
typedef struct {
  char *id;                      /* subscript to ident array */
  int list_size;
  struct nodeTypeTag *next;  
} idNodeType;

/* operators */
typedef struct {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    struct nodeTypeTag *op[1];  /* operands (expandable) */
} oprNodeType;

/* function */
typedef struct {
    char *func;                   /* function */
  int nortns;                     /* number of returns */
    int noargs;                   /* number of operands */
    struct nodeTypeTag *arglist;  /* operands (expandable) */
} funNodeType;

/* argument list */
typedef struct argNodeTypeTag{
  int noargs;
  struct nodeTypeTag *arg;  /* operands (expandable) */
  struct nodeTypeTag *next;
  //void *next;
} argNodeType;

typedef struct indexRangeNodeTypeTag{
  struct nodeTypeTag *parent;
  OM_SUPPORT_TYPE start;
  OM_SUPPORT_TYPE end;
  OM_SUPPORT_TYPE step;
  bool free; // is it only ":"?
  short argIdx;
} indexRangeNodeType;

typedef struct stringNodeTypeTag
{
  char *string;
}stringNodeType;

typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

  void *myMatrix;
    /* union must be last entry in nodeType */
    /* because operNodeType may dynamically increase */
    union {
  conNodeType con;        /* constants */
  vecNodeType vec;	  /* matrix */
  idNodeType id;          /* identifiers */
  oprNodeType opr;        /* operators */
      funNodeType fun;
      argNodeType arg;
      indexRangeNodeType index;
      stringNodeType string;
    };
} nodeType;


#endif
