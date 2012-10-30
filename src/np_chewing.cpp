/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <npapi.h>
#include <npfunctions.h>
#include <npruntime.h>

#include "np_chewing_class.h"

static const char* sPluginName = "Chewing IME";
static const char* sPluginDescription = "Chewing Input Method Editor";
static const char* sMimeDescription = "application/x-chewing-ime::Chewing IME";
static const NPNetscapeFuncs* sBrowser;

const char*
NP_GetMIMEDescription(void)
{
  return sMimeDescription;
}

NPError
NP_Initialize(NPNetscapeFuncs* aNPNFuncs, NPPluginFuncs* aNPPFuncs)
{
  sBrowser = aNPNFuncs;

  NPChewingInitClass(aNPNFuncs);

  aNPPFuncs->newp     = NPP_New;
  aNPPFuncs->destroy  = NPP_Destroy;
  aNPPFuncs->getvalue = NPP_GetValue;
  aNPPFuncs->setvalue = NPP_SetValue;

  return NPERR_NO_ERROR;
}

NPError
NP_Shutdown(void)
{
  return NPERR_NO_ERROR;
}

NPError
NPP_New(NPMIMEType pluginType, NPP instance,
        uint16_t mode, int16_t argc, char* argn[],
        char* argv[], NPSavedData* saved)
{
  instance->pdata = NPChewingCreateInstance(sBrowser, instance);
  return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP instance, NPSavedData** save)
{
  return NPERR_NO_ERROR;
}

NPError
NP_GetValue(void* future, NPPVariable aVariable, void* aValue)
{
  switch (aVariable) {
    case NPPVpluginNameString:
      *((const char**)aValue) = sPluginName;
      break;
    case NPPVpluginDescriptionString:
      *((const char**)aValue) = sPluginDescription;
      break;
    default:
      return NPERR_INVALID_PARAM;
      break;
  }
  return NPERR_NO_ERROR;
}

NPError
NPP_GetValue(NPP instance, NPPVariable aVariable, void *value)
{
  switch (aVariable) {
    case NPPVpluginScriptableNPObject:
    {
      NPObject* object = (NPObject*)instance->pdata;
      sBrowser->retainobject(object);
      *((NPObject**)value) = object;
      break;
    }
    default:
      return NPERR_GENERIC_ERROR;
      break;
  }
  return NPERR_NO_ERROR;
}

NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
  return NPERR_GENERIC_ERROR;
}
