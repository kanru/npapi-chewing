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

#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

static const NPUTF8* sMethodSymbols[] = {
  "handleDefault",
  "handleEnter"
};
static NPIdentifier sMethodInternedSymbols[ARRAY_LENGTH(sMethodSymbols)];
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
  return true;
}

static bool
NPChewingInvoke(NPObject *npobj, NPIdentifier name,
                const NPVariant *args, uint32_t argCount,
                NPVariant *result)
{
  if (name == sMethodInternedSymbols[0]) {
    result->type = NPVariantType_Int32;
    result->value.intValue = 42;
    return true;
  }
  return false;
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
  aBrowser->getstringidentifiers(sMethodSymbols,
                                 ARRAY_LENGTH(sMethodSymbols),
                                 sMethodInternedSymbols);

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
