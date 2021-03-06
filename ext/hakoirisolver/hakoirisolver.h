#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FALSE (0)
#define TRUE  (1)

#define cPANELTYPE_COMMON (0)
#define cPANELTYPE_TARGET (1)

#define cFIELD_SIZE_MAX   (5)  // ハッシュ関数の都合で6より大きい値にできない
#define cPANEL_SIZE_MAX   (2)  // ハッシュ関数の都合で2以上の値にできない

#define cPANELS_MAX (cFIELD_SIZE_MAX*cFIELD_SIZE_MAX)
#define cPANEL_SIZE_PATTERNS (cPANEL_SIZE_MAX*cPANEL_SIZE_MAX)

#define cPANEL_HASH_LENGTH (1)
#define cFIELD_HASH_MAX    (0xffff)

#define cPANEL_HASH_MAX    (192)

#define cSOLVE_LEAVES_MAX  (16)

enum {
  eDIR_UP = 0,
  eDIR_DOWN,
  eDIR_LEFT,
  eDIR_RIGHT,
  eDIR_MAX
};

enum {
  eSOLVESTATE_CONTINUE = 0,
  eSOLVESTATE_FAILED,
  eSOLVESTATE_SUCCEED,
  eSOLVESTATE_MAX
};

typedef struct MESSAGE_STACK_t {
  int message_count;
  char messages[256][256];
}MESSAGE_STACK;

typedef struct PANEL_t {
  char width;
  char height;
  char x;
  char y;

  char type;
  //unsigned char hash;
  char padd[3];
}PANEL;

typedef struct FIELD_INFO_t {
  char width;
  char height;

  char end_x;
  char end_y;

  int panel_count;
  int target_idx;
  int field_hash_count;
}FIELD_INFO;

typedef struct FIELD_HASH_t {
  int flg;

  struct FIELD_HASH_t* next[cPANEL_HASH_MAX];
}FIELD_HASH;

typedef struct FIELD_t {
  PANEL panels[cPANELS_MAX];
}FIELD;

typedef struct SOLVE_TREE_t {
  char depth;
  char leaves_count;
  char dir;
  char panel_idx;

  FIELD field;
  struct SOLVE_TREE_t* leaves[cSOLVE_LEAVES_MAX];
}SOLVE_TREE;
