/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <npapi.h>
#include <npruntime.h>
#include <npfunctions.h>

#include "np_chewing_class.h"

#define ARRAY_LENGTH(a) (sizeof(a)/sizeof(a[0]))

static const NPUTF8* sMethodSymbols[] = {
  "handleDefault",
  "handleEnter"
};
static NPIdentifier sMethodInternedSymbols[ARRAY_LENGTH(sMethodSymbols)];
static NPClass sChewingClass;

bool
NPChewingHasMethod(NPObject *npobj, NPIdentifier name)
{
  return true;
}

bool
NPChewingInvoke(NPObject *npobj, NPIdentifier name,
                const NPVariant *args, uint32_t argCount,
                NPVariant *result)
{
  return true;
}

bool
NPChewingConstruct(NPObject *npobj,
                   const NPVariant *args,
                   uint32_t argCount,
                   NPVariant *result)
{
  return true;
}

void
NPChewingInitClass(NPNetscapeFuncs* aBrowser)
{
  aBrowser->getstringidentifiers(sMethodSymbols,
                                 ARRAY_LENGTH(sMethodSymbols),
                                 sMethodInternedSymbols);

  sChewingClass.structVersion = 3;
  sChewingClass.hasMethod = NPChewingHasMethod;
  sChewingClass.invoke = NPChewingInvoke;
  sChewingClass.construct = NPChewingConstruct;
}
