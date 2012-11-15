/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <npapi.h>
#include <npruntime.h>
#include <npfunctions.h>

#include <stdlib.h>
#include <string.h>

#include "chewing_dso.h"
#include "np_chewing_class.h"

enum ChewingClassMethod {
  HANDLE_DEFAULT,
  HANDLE_ENTER,
  BUFFER_STRING,
  COMMIT_STRING,
  ZHUYIN_STRING,
  CANDIDATES_LIST,
  SELECT_CANDIDATE,
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
  0, ZHUYIN_STRING, "zhuyinString",
  0, CANDIDATES_LIST, "candidatesList",
  0, SELECT_CANDIDATE, "selectCandidate",
};

static NPClass sChewingClass;

static NPN_GetStringIdentifierProcPtr npn_getStringIdentifier;
static NPN_GetValueProcPtr npn_getValue;
static NPN_InvokeProcPtr npn_invoke;
static NPN_MemAllocProcPtr npn_malloc;
static NPN_MemFreeProcPtr npn_free;
static NPN_ReleaseObjectProcPtr npn_releaseObject;
static NPN_ReleaseVariantValueProcPtr npn_releaseVariantValue;

struct chewing_pfns chewing;
struct NPChewingObject : NPObject {
  NPP npp;
  ChewingContext* ctx;
};

static NPObject*
NPChewingAllocate(NPP instance, NPClass* aClass)
{
  NPChewingObject* object = (NPChewingObject*)npn_malloc(sizeof(NPChewingObject));
  if (object) {
    memset(object, 0, sizeof(NPChewingObject));
    setenv("CHEWING_PATH", "/data/local/libchewing", 1);
    setenv("CHEWING_USER_PATH", "/data/local/libchewing", 1);
    object->npp = instance;
    fprintf(stderr, "try object new\n");
    object->ctx = chewing.chewing_new();
    fprintf(stderr, "object new success\n");
    if (object->ctx) {
      fprintf(stderr, "chewing ctx %p\n", object->ctx);
      chewing.chewing_set_candPerPage(object->ctx, 9);
      chewing.chewing_set_maxChiSymbolLen(object->ctx, 16);
      chewing.chewing_set_phraseChoiceRearward(object->ctx, 1);
      fprintf(stderr, "chewing ctx %p\n", object->ctx);
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
  if (!obj->ctx) {
    return false;
  }
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
      char* str = chewing.chewing_buffer_String(obj->ctx);
      char* r = (char*)npn_malloc(strlen(str)+1);
      strcpy(r, str);
      chewing.chewing_free(str);
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
      char* str = chewing.chewing_commit_String(obj->ctx);
      char* r = (char*)npn_malloc(strlen(str)+1);
      strcpy(r, str);
      chewing.chewing_free(str);
      STRINGZ_TO_NPVARIANT(r, *result);
      rv = true;
      break;
    }
    case ZHUYIN_STRING:
    {
      if (argCount != 0) {
        break;
      }
      if (!chewing.chewing_zuin_Check(obj->ctx)) {
        NULL_TO_NPVARIANT(*result);
        rv = true;
        break;
      }
      int count = 0;
      char* str = chewing.chewing_zuin_String(obj->ctx, &count);
      char* r = (char*)npn_malloc(strlen(str)+1);
      fprintf(stderr, "zhuyin_string %s\n", str);
      strcpy(r, str);
      chewing.chewing_free(str);
      STRINGZ_TO_NPVARIANT(r, *result);
      rv = true;
      break;
    }
    case CANDIDATES_LIST:
    {
      if (argCount != 0) {
        break;
      }
      // open candidates list
      chewing.chewing_handle_Down(obj->ctx);

      // create a array object
      NPObject* window = NULL;
      npn_getValue(obj->npp, NPNVWindowNPObject, &window);
      NPObject* array = NULL;
      NPVariant arrayVar;
      npn_invoke(obj->npp,
                 window,
                 npn_getStringIdentifier("Array"),
                 NULL,
                 0,
                 &arrayVar);
      array = NPVARIANT_TO_OBJECT(arrayVar);
      npn_releaseObject(window);

      // Push all candidates to array
      NPIdentifier push = npn_getStringIdentifier("push");
      chewing.chewing_cand_Enumerate(obj->ctx);
      fprintf(stderr, "get candidates\n");
      bool hasCandidates = false;
      while (chewing.chewing_cand_hasNext(obj->ctx)) {
        hasCandidates = true;
        char* str = chewing.chewing_cand_String(obj->ctx);
        char* r = (char*)npn_malloc(strlen(str)+1);
        fprintf(stderr, "push candidates %s\n", str);
        strcpy(r, str);
        chewing.chewing_free(str);
        NPVariant item;
        NPVariant unused;
        STRINGZ_TO_NPVARIANT(r, item);
        npn_invoke(obj->npp, array, push, &item, 1, &unused);
        npn_releaseVariantValue(&unused);
      }
      OBJECT_TO_NPVARIANT(array, *result);
      rv = true;

      if (hasCandidates)
        chewing.chewing_handle_Esc(obj->ctx);
      break;
    }
    case SELECT_CANDIDATE:
    {
      if (argCount != 1) {
        break;
      }
      if (!NPVARIANT_IS_INT32(args[0])) {
        break;
      }
      int32_t n = NPVARIANT_TO_INT32(args[0]);
      chewing.chewing_handle_Down(obj->ctx);
      chewing.chewing_cand_Select(obj->ctx, n);
      NULL_TO_NPVARIANT(*result);
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
  npn_free = aBrowser->memfree;
  npn_getStringIdentifier = aBrowser->getstringidentifier;
  npn_getValue = aBrowser->getvalue;
  npn_invoke = aBrowser->invoke;
  npn_malloc = aBrowser->memalloc;
  npn_releaseObject = aBrowser->releaseobject;
  npn_releaseVariantValue = aBrowser->releasevariantvalue;

  for (int i = 0; i < METHOD_LENGTH; ++i) {
    sSymbolIdentifierMap[i].id =
      npn_getStringIdentifier(sSymbolIdentifierMap[i].symbol_name);
  }

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
