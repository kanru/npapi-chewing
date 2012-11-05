/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <dlfcn.h>

#include "chewing_dso.h"

#define LIBCHEWING_DSO "libchewing.so.3.0.1"

bool
chewing_dso_open(struct chewing_pfns* pfns)
{
  void *h;

  h = pfns->_handle = dlopen(LIBCHEWING_DSO, RTLD_LAZY);
  if (!h)
    return false;

#define LOAD_SYMBOL(sym)                                \
  do {                                                  \
    pfns->sym = (__typeof__ pfns->sym)dlsym(h, #sym);   \
    if (!pfns->sym) {                                   \
      dlclose(h);                                       \
      return false;                                     \
    }                                                   \
  } while(0)

  LOAD_SYMBOL(chewing_new);
  LOAD_SYMBOL(chewing_delete);
  LOAD_SYMBOL(chewing_free);
  LOAD_SYMBOL(chewing_handle_Default);
  LOAD_SYMBOL(chewing_handle_Enter);
  LOAD_SYMBOL(chewing_handle_Down);
  LOAD_SYMBOL(chewing_handle_Esc);
  LOAD_SYMBOL(chewing_commit_Check);
  LOAD_SYMBOL(chewing_commit_String);
  LOAD_SYMBOL(chewing_buffer_Check);
  LOAD_SYMBOL(chewing_buffer_Len);
  LOAD_SYMBOL(chewing_buffer_String);
  LOAD_SYMBOL(chewing_zuin_Check);
  LOAD_SYMBOL(chewing_zuin_String);
  LOAD_SYMBOL(chewing_cand_TotalPage);
  LOAD_SYMBOL(chewing_cand_CurrentPage);
  LOAD_SYMBOL(chewing_cand_Enumerate);
  LOAD_SYMBOL(chewing_cand_hasNext);
  LOAD_SYMBOL(chewing_cand_String);
  LOAD_SYMBOL(chewing_set_candPerPage);
  LOAD_SYMBOL(chewing_set_maxChiSymbolLen);
}

void
chewing_dso_close(struct chewing_pfns* pfns)
{
  if (pfns->_handle)
    dlclose(pfns->_handle);
}
