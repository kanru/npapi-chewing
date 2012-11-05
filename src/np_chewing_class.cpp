/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <npapi.h>
#include <npruntime.h>
#include <npfunctions.h>

#include <string.h>

#include "chewing_dso.h"
#include "np_chewing_class.h"

enum ChewingClassMethod {
  HANDLE_DEFAULT,
  HANDLE_ENTER,
  BUFFER_STRING,
  COMMIT_STRING,
  METHOD_LENGTH
};

static struct SymbolIdentifierMap {
  NPIdentifier id;
  uint8_t symbol;
  const NPUTF8* symbol_name;
} sSymbolIdentifierMap[METHOD_LENGTH] = {
  0, HANDLE_DEFAULT, "handleDefault",
  0, HANDLE_ENTER, "handleEnter",
  0, BUFFER_STRING, "bufferString",
  0, COMMIT_STRING, "commitString",
};

static NPClass sChewingClass;

static void* (*npn_malloc)(uint32_t size);
static void (*npn_free)(void* ptr);

struct chewing_pfns chewing;
struct NPChewingObject : NPObject {
  ChewingContext* ctx;
};

static NPObject*
NPChewingAllocate(NPP instance, NPClass* aClass)
{
  NPChewingObject* object = (NPChewingObject*)npn_malloc(sizeof(NPChewingObject));
  if (object) {
    memset(object, 0, sizeof(NPChewingObject));
    object->ctx = chewing.chewing_new();
    if (object->ctx) {
      chewing.chewing_set_candPerPage(object->ctx, 9);
      chewing.chewing_set_maxChiSymbolLen(object->ctx, 16);
    }
  }
  return object;
}

static void
NPChewingDeallocate(NPObject* object)
{
  npn_free(object);
}

static bool
NPChewingHasMethod(NPObject *npobj, NPIdentifier name)
{
  for (int i = 0; i < METHOD_LENGTH; ++i) {
    if (sSymbolIdentifierMap[i].id == name) {
      return true;
    }
  }
  return false;
}

static bool
NPChewingInvoke(NPObject *npobj, NPIdentifier name,
                const NPVariant *args, uint32_t argCount,
                NPVariant *result)
{
  uint8_t symbol = -1;
  for (int i = 0; i < METHOD_LENGTH; ++i) {
    if (sSymbolIdentifierMap[i].id == name) {
      symbol = sSymbolIdentifierMap[i].symbol;
      break;
    }
  }

  bool rv = false;
  NPChewingObject* obj = (NPChewingObject*)npobj;
  switch (symbol) {
    case HANDLE_DEFAULT:
    {
      if (argCount != 1) {
        break;
      }
      if (!NPVARIANT_IS_INT32(args[0])) {
        break;
      }
      int32_t key = NPVARIANT_TO_INT32(args[0]);
      int32_t r = chewing.chewing_handle_Default(obj->ctx, key);
      INT32_TO_NPVARIANT(r, *result);
      rv = true;
      break;
    }
    case HANDLE_ENTER:
    {
      if (argCount != 0) {
        break;
      }
      int32_t r = chewing.chewing_handle_Enter(obj->ctx);
      INT32_TO_NPVARIANT(r, *result);
      rv = true;
      break;
    }
    case BUFFER_STRING:
    {
      if (argCount != 0) {
        break;
      }
      if (!chewing.chewing_buffer_Check(obj->ctx)) {
        NULL_TO_NPVARIANT(*result);
        rv = true;
        break;
      }
      char* bs = chewing.chewing_buffer_String(obj->ctx);
      char* r = (char*)npn_malloc(strlen(bs)+1);
      strcpy(r, bs);
      chewing.chewing_free(bs);
      STRINGZ_TO_NPVARIANT(r, *result);
      rv = true;
      break;
    }
    case COMMIT_STRING:
    {
      if (argCount != 0) {
        break;
      }
      if (!chewing.chewing_commit_Check(obj->ctx)) {
        NULL_TO_NPVARIANT(*result);
        rv = true;
        break;
      }
      char* bs = chewing.chewing_commit_String(obj->ctx);
      char* r = (char*)npn_malloc(strlen(bs)+1);
      strcpy(r, bs);
      chewing.chewing_free(bs);
      STRINGZ_TO_NPVARIANT(r, *result);
      rv = true;
      break;
    }
    default:
      break;
  }

  return rv;
}

static bool
NPChewingConstruct(NPObject *npobj,
                   const NPVariant *args,
                   uint32_t argCount,
                   NPVariant *result)
{
  return true;
}

void
NPChewingInitClass(const NPNetscapeFuncs* aBrowser)
{
  for (int i = 0; i < METHOD_LENGTH; ++i) {
    sSymbolIdentifierMap[i].id =
      aBrowser->getstringidentifier(sSymbolIdentifierMap[i].symbol_name);
  }

  npn_malloc = aBrowser->memalloc;
  npn_free = aBrowser->memfree;

  sChewingClass.structVersion = 3;
  sChewingClass.hasMethod = NPChewingHasMethod;
  sChewingClass.invoke = NPChewingInvoke;
  sChewingClass.construct = NPChewingConstruct;
  sChewingClass.allocate = NPChewingAllocate;
  sChewingClass.deallocate = NPChewingDeallocate;

  chewing_dso_open(&chewing);
}

void
NPChewingShutdownClass()
{
  chewing_dso_close(&chewing);
}

NPObject*
NPChewingCreateInstance(const NPNetscapeFuncs* aBrowser, NPP instance)
{
  return aBrowser->createobject(instance, &sChewingClass);
}
