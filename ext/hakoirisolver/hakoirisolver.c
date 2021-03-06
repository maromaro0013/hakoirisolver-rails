#include <ruby.h>
#include "hakoirisolver.h"

static char               panel_limit_dp[cPANEL_HASH_MAX][eDIR_MAX];
static FIELD_INFO         g_field_info;
static FIELD              g_field;
static unsigned char*     g_field_hashs[cFIELD_HASH_MAX];
static FIELD_HASH         g_field_hash_root;
MESSAGE_STACK             g_message_stack;

VALUE set_field_info(VALUE self, VALUE w, VALUE h, VALUE end_x, VALUE end_y);
VALUE add_panel_to_field(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h, VALUE type);
VALUE get_panel_count(VALUE self);
VALUE data_validate(VALUE self);
VALUE init_solver(VALUE self);
VALUE solve_field(VALUE self);
VALUE delete_solver(VALUE self);

VALUE pop_message(VALUE self);

void Init_hakoirisolver() {
  g_message_stack.message_count = 0;

  VALUE csolver;
  csolver = rb_define_class("HakoiriSolver", rb_cObject);
  rb_define_method(csolver, "set_field_info", set_field_info, 4);
  rb_define_method(csolver, "add_panel_to_field", add_panel_to_field, 5);
  rb_define_method(csolver, "get_panel_count", get_panel_count, 0);
  rb_define_method(csolver, "data_validate", data_validate, 0);
  rb_define_method(csolver, "init_solver", init_solver, 0);
  rb_define_method(csolver, "solve_field", solve_field, 0);
  rb_define_method(csolver, "delete_solver", delete_solver, 0);

  rb_define_method(csolver, "pop_message", pop_message, 0);
}

VALUE set_field_info(VALUE self, VALUE w, VALUE h, VALUE end_x, VALUE end_y) {
  FIELD_INFO* info = &g_field_info;

  info->width = FIX2INT(w);
  info->height = FIX2INT(h);
  info->end_x = FIX2INT(end_x);
  info->end_y = FIX2INT(end_y);

  info->target_idx = -1;
  info->panel_count = 0;
  info->field_hash_count = 0;

  return 0;
}

void init_field_hash(void) {
  memset(g_field_hashs, 0, sizeof(g_field_hashs));
}
unsigned char* create_field_hash(FIELD_INFO* info, FIELD* f) {
  int hash_length = info->panel_count*cPANEL_HASH_LENGTH;
  unsigned char* ret = (unsigned char*)malloc(hash_length);
  int i = 0;
  int j = 0;

  unsigned char hash = 0;
  unsigned char tmp_hash = 0;

  for (i = 0; i < info->panel_count; i++) {
    PANEL* p = &f->panels[i];
    ret[i] = (p->x << 5) | (p->y << 2) | ((p->width - 1) << 1) | (p->height - 1);
  }
  for (i = 0; i < info->panel_count - 1; i++) {
    for (j = 0; j < info->panel_count - 1 - i; j++) {
      if (ret[j] > ret[j + 1]) {
        tmp_hash = ret[j];
        ret[j] = ret[j + 1];
        ret[j + 1] = tmp_hash;
      }
    }
  }

  return ret;
}

void add_field_hash(FIELD_INFO* info, unsigned char* hash) {
  g_field_hashs[info->field_hash_count] = hash;
  info->field_hash_count++;
}
void delete_field_hashs(FIELD_INFO* info) {
  int i = 0;
  for (i = 0; i < info->field_hash_count; i++) {
    free(g_field_hashs[i]);
    g_field_hashs[i] = NULL;
  }
}
// ---------------------------

void push_message(char* message) {
  int count = g_message_stack.message_count;

  strcpy(g_message_stack.messages[count], message);
  g_message_stack.message_count++;
}

VALUE pop_message(VALUE self) {
  int count = g_message_stack.message_count;

  if (count <= 0) {
    return rb_str_new(0, 0);
  }

  g_message_stack.message_count--;
  count = g_message_stack.message_count;
  return rb_str_new(g_message_stack.messages[count], strlen(g_message_stack.messages[count]));
}

void copy_field(FIELD* source, FIELD* dest) {
  memcpy((void*)&dest->panels[0], &source->panels[0], sizeof(dest->panels));
}

VALUE add_panel_to_field(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h, VALUE type) {
  FIELD_INFO* info = &g_field_info;
  PANEL* p = &g_field.panels[info->panel_count];
  memset(p, 0, sizeof(PANEL));

  p->width = FIX2INT(w);
  p->height = FIX2INT(h);
  p->x = FIX2INT(x);
  p->y = FIX2INT(y);
  p->type = FIX2INT(type);

  if (p->type == cPANELTYPE_TARGET) {
    info->target_idx = info->panel_count;
  }
  info->panel_count++;

  return 0;
}

int chk_and_append_field_hash(unsigned char *hashs, int len) {
  int i = 0;
  int ret = FALSE;
  FIELD_HASH* next = &g_field_hash_root;
  unsigned char hash = hashs[0];

  for (i = 0; i < len; i++) {
    hash = hashs[i];
    if (next->next[hash] == NULL) {
      next->next[hash] = (FIELD_HASH*)malloc(sizeof(FIELD_HASH));
      memset((void*)next->next[hash], 0, sizeof(FIELD_HASH));
      if (ret == FALSE) {
        g_field_info.field_hash_count += 1;
        ret = TRUE;
      }
    }
    next = next->next[hash];
  }
  return ret;
}
void destroy_field_hashs(FIELD_HASH* f) {
  int i = 0;

  for (i = 0; i < cPANEL_HASH_MAX; i++) {
    if (f->next[i] != NULL) {
      destroy_field_hashs(f->next[i]);
      free(f->next[i]);
    }
  }
}

int panel_collision(PANEL* p0, PANEL* p1) {
  int x0 = p0->x;
  int x1 = p0->x + p0->width;
  int x2 = p1->x;
  int x3 = p1->x + p1->width;

  int y0 = p0->y;
  int y1 = p0->y + p0->height;
  int y2 = p1->y;
  int y3 = p1->y + p1->height;

  if (x0 >= x2 && x0 < x3) {
    if (y0 >= y2 && y0 < y3) {
      return TRUE;
    }
    if (y1 > y2 && y1 <= y3) {
      return TRUE;
    }
  }
  if (x1 > x2 && x1 <= x3) {
    if (y1 > y2 && y1 <= y3) {
      return TRUE;
    }
    if (y0 >= y2 && y0 < y3) {
      return TRUE;
    }
  }
  return FALSE;
}

// 移動判定のメモ化配列の初期化
void init_panel_limit_dp() {
  memset(panel_limit_dp, -1, sizeof(panel_limit_dp));
}

int chk_panel_move(FIELD* field, int panel_idx, int dir) {
  FIELD_INFO* info = &g_field_info;

  if (panel_idx >= info->panel_count) {
    return FALSE;
  }

  PANEL* p = &field->panels[panel_idx];
  PANEL tmp_panel = *p;

  unsigned char hash = (p->x << 5) | (p->y << 2) | ((p->width - 1) << 1) | (p->height - 1);
  int dir_move_arr[eDIR_MAX][2] = {
    {0, -1},
    {0, +1},
    {-1, 0},
    {+1, 0}
  };
  tmp_panel.x += dir_move_arr[dir][0];
  tmp_panel.y += dir_move_arr[dir][1];

  if (panel_limit_dp[hash][dir] == -1) {
    if (tmp_panel.y < 0) {
      panel_limit_dp[hash][dir] = 0;
    }
    else if (tmp_panel.y + tmp_panel.height > info->height) {
      panel_limit_dp[hash][dir] = 0;
    }
    else if (tmp_panel.x < 0) {
      panel_limit_dp[hash][dir] = 0;
    }
    else if (tmp_panel.x + tmp_panel.width > info->width) {
      panel_limit_dp[hash][dir] = 0;
    }
    else {
      panel_limit_dp[hash][dir] = 1;
    }
  }
  if (panel_limit_dp[hash][dir] == 0) {
    return FALSE;
  }

  int i = 0;
  for (i = 0; i < info->panel_count; i++) {
      if (i == panel_idx) {
        continue;
      }
      PANEL *target = &field->panels[i];
      if (panel_collision(&tmp_panel, target)) {
        return FALSE;
      }
  }
  return TRUE;
}

int move_panel(FIELD* field, int panel_idx, int dir) {
  FIELD_INFO* info = &g_field_info;

  PANEL* p = &field->panels[panel_idx];

  int dir_move_arr[eDIR_MAX][2] = {
    {0, -1},
    {0, +1},
    {-1, 0},
    {+1, 0}
  };
  p->x += dir_move_arr[dir][0];
  p->y += dir_move_arr[dir][1];

  return TRUE;
}

VALUE data_validate(VALUE self) {
  FIELD_INFO* info = &g_field_info;
  FIELD* field = &g_field;

  if (info->target_idx < 0) {
    printf("target panel not found\n");
    return INT2FIX(FALSE);
  }

  if (info->width <= 0 || info->width > cFIELD_SIZE_MAX) {
    printf("illegal field size\n");
    return INT2FIX(FALSE);
  }
  if (info->height <= 0 || info->height > cFIELD_SIZE_MAX) {
    printf("illegal field size\n");
    return INT2FIX(FALSE);
  }

  int i = 0;
  for (i = 0; i < info->panel_count; i++) {
    PANEL* p = &field->panels[i];
    if (p->width <= 0 || p->width > cPANEL_SIZE_MAX) {
      printf("illegal panel size\n");
      return INT2FIX(FALSE);
    }
    if (p->height <= 0 || p->height > cPANEL_SIZE_MAX) {
      printf("illegal panel size\n");
      return INT2FIX(FALSE);
    }

    if (p->x < 0 || p->x > info->width) {
      printf("illegal panel position\n");
      return INT2FIX(FALSE);
    }
    if (p->y < 0 || p->y > info->height) {
      printf("illegal panel position\n");
      return INT2FIX(FALSE);
    }
  }

  return INT2FIX(TRUE);
}

int chk_clear_field(FIELD* f) {
  FIELD_INFO* info = &g_field_info;
  int idx = info->target_idx;

  PANEL* p = &f->panels[idx];

  if ((p->x+p->width == info->end_x) && (p->y+p->height == info->end_y)) {
    return TRUE;
  }
  return FALSE;
}

void init_solve_leaf(SOLVE_TREE* leaf, int depth) {
  leaf->depth = depth;
  leaf->leaves_count = 0;
  int i = 0;
  for (i = 0; i < cSOLVE_LEAVES_MAX; i++) {
    leaf->leaves[i] = NULL;
  }
}

void destroy_solve_tree(SOLVE_TREE* leaf) {
  int i = 0;
  if (leaf == NULL) {
    return;
  }

  for (i = 0; i < leaf->leaves_count; i++) {
    destroy_solve_tree(leaf->leaves[i]);
  }
  free(leaf);
}

int chk_hash_and_append(FIELD_INFO* info, FIELD* f) {
  unsigned char* hash = create_field_hash(info, f);
  int hash_length = info->panel_count*cPANEL_HASH_LENGTH;
  int ret = chk_and_append_field_hash(hash, hash_length);

  free(hash);
  return !ret;
/*
  unsigned char* hash = create_field_hash(info, f);
  unsigned char* diff_hash = NULL;
  int i = 0;
  int hash_length = info->panel_count*cPANEL_HASH_LENGTH;

  for (i = 0; i < info->field_hash_count; i++) {
    diff_hash = g_field_hashs[i];
    if (memcmp(diff_hash, hash, hash_length) == 0) {
      free(hash);
      return TRUE;
    }
  }

  add_field_hash(info, hash);
  return FALSE;
*/
}

/*
int count_leaves_from_depth(SOLVE_TREE* leaf, int depth) {
  int ret = leaf->leaves_count;
  int i = 0;

  if (leaf->depth >= depth) {
    return ret;
  }

  for (i = 0; i < leaf->leaves_count; i++) {
    ret += count_leaves_from_depth(leaf->leaves[i], depth);
  }
  return ret;
}
*/

void push_solve_message(SOLVE_TREE* leaf) {
  char message[256];
  if (leaf->depth > 0) {
    switch (leaf->dir) {
      case eDIR_UP:
        sprintf(message, "%d,UP", leaf->panel_idx);
      break;
      case eDIR_DOWN:
        sprintf(message, "%d,DOWN", leaf->panel_idx);
      break;
      case eDIR_LEFT:
        sprintf(message, "%d,LEFT", leaf->panel_idx);
      break;
      case eDIR_RIGHT:
        sprintf(message, "%d,RIGHT", leaf->panel_idx);
      break;
    }
    push_message(message);
  }
}

int grow_solve_tree(SOLVE_TREE* root, SOLVE_TREE* leaf, int depth) {
  FIELD_INFO* info = &g_field_info;
  int i = 0;
  int j = 0;

  if (leaf->depth < depth) {
    int ret = eSOLVESTATE_FAILED;
    for (i = 0; i < leaf->leaves_count; i++) {
      int tmp = grow_solve_tree(root, leaf->leaves[i], depth);
      switch (tmp) {
        case eSOLVESTATE_CONTINUE:
          ret = eSOLVESTATE_CONTINUE;
        break;

        case eSOLVESTATE_FAILED:
          // 葉が広がらないなら削除
          destroy_solve_tree(leaf->leaves[i]);
          for (j = i; j < cSOLVE_LEAVES_MAX - 1; j++) {
            leaf->leaves[j] = leaf->leaves[j + 1];
          }
          leaf->leaves_count--;
          i--;
        break;

        case eSOLVESTATE_SUCCEED:
          push_solve_message(leaf);
          return eSOLVESTATE_SUCCEED;
      }
    }
    return ret;
  }

  if (chk_clear_field(&leaf->field) == TRUE) {
    push_solve_message(leaf);
    return eSOLVESTATE_SUCCEED;
  }

  for (i = 0; i < info->panel_count; i++) {
    PANEL* p = &leaf->field.panels[i];
    for (j = 0; j < eDIR_MAX; j++) {
      if (!chk_panel_move(&leaf->field, i, j)) {
        continue;
      }

      if ( (leaf->leaves_count+1) >= cSOLVE_LEAVES_MAX) {
        printf("error over leaves\n");
        return eSOLVESTATE_FAILED;
      }

      leaf->leaves[leaf->leaves_count] = (SOLVE_TREE*)malloc(sizeof(SOLVE_TREE));
      SOLVE_TREE* new_leaf = leaf->leaves[leaf->leaves_count];
      copy_field(&leaf->field, &new_leaf->field);
      move_panel(&new_leaf->field, i, j);

      new_leaf->depth = depth + 1;
      new_leaf->leaves_count = 0;
      new_leaf->panel_idx = i;
      new_leaf->dir = j;

      // compare hash
      if (chk_hash_and_append(&g_field_info, &new_leaf->field) == TRUE) {
        destroy_solve_tree(new_leaf);
        leaf->leaves[leaf->leaves_count] = NULL;
        continue;
      }
      leaf->leaves_count++;
    }
  }

  if (leaf->leaves_count == 0) {
    return eSOLVESTATE_FAILED;
  }

  return eSOLVESTATE_CONTINUE;
}

VALUE solve_field(VALUE self) {
  int i = 0;
  int depth = 0;
  int max_depth = 128;
  SOLVE_TREE* root = (SOLVE_TREE*)malloc(sizeof(SOLVE_TREE));

  memset((void*)&g_field_hash_root, 0, sizeof(g_field_hash_root));

  root->depth = 0;
  root->leaves_count = 0;
  copy_field(&g_field, &root->field);
  chk_hash_and_append(&g_field_info, &root->field);

  int ret = eSOLVESTATE_FAILED;
  int leaves_count = 0;

  for (i = 0; i < max_depth; i++) {
    ret = grow_solve_tree(root, root, i);
    if (ret == eSOLVESTATE_FAILED || ret == eSOLVESTATE_SUCCEED) {
      break;
    }
    //printf("%d:%d\n", i, g_field_info.field_hash_count);
  }

  destroy_solve_tree(root);
  destroy_field_hashs(&g_field_hash_root);

  return INT2FIX(ret);
}

VALUE get_panel_count(VALUE self) {
  return INT2FIX(g_field_info.panel_count);
}

VALUE init_solver(VALUE self) {
  init_panel_limit_dp();
  init_field_hash();

  return INT2FIX(0);
}

VALUE delete_solver(VALUE self) {
  delete_field_hashs(&g_field_info);

  return INT2FIX(0);
}
