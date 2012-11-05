/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CHEWING_DSO_H
#define CHEWING_DSO_H

typedef struct ChewingContext ChewingContext;

struct chewing_pfns {
  void* _handle;
  ChewingContext* (*chewing_new)();
  void (*chewing_delete)(ChewingContext* ctx);
  void (*chewing_free)(void* ptr);

  int (*chewing_handle_Default)(ChewingContext* ctx, int key);
  int (*chewing_handle_Enter)(ChewingContext* ctx);
  int (*chewing_handle_Down)(ChewingContext* ctx);
  int (*chewing_handle_Esc)(ChewingContext* ctx);

  int (*chewing_commit_Check)(ChewingContext* ctx);
  char* (*chewing_commit_String)(ChewingContext* ctx);
  int (*chewing_buffer_Check)(ChewingContext* ctx);
  int (*chewing_buffer_Len)(ChewingContext* ctx);
  char* (*chewing_buffer_String)(ChewingContext* ctx);
  int (*chewing_zuin_Check)(ChewingContext* ctx);
  char* (*chewing_zuin_String)(ChewingContext* ctx, int* zuin_count);

  int (*chewing_cand_TotalPage)(ChewingContext* ctx);
  int (*chewing_cand_CurrentPage)(ChewingContext* ctx);
  void (*chewing_cand_Enumerate)(ChewingContext* ctx);
  int (*chewing_cand_hasNext)(ChewingContext* ctx);
  char* (*chewing_cand_String)(ChewingContext* ctx);

  int (*chewing_set_candPerPage)(ChewingContext* ctx, int n);
  int (*chewing_set_maxChiSymbolLen)(ChewingContext* ctx, int n);
};

bool chewing_dso_open(struct chewing_pfns* pfns);
void chewing_dso_close(struct chewing_pfns* pfns);

#endif
