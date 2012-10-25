#include <npapi.h>
#include <npfunctions.h>
#include <npruntime.h>

static const char* sPluginName = "Chewing IME";
static const char* sPluginDescription = "Chewing Input Method Editor";
static const char* sMimeDescription = "application/x-chewing-ime::Chewing IME";
static const NPNetscapeFuncs* sNPNFuncs;

const char*
NP_GetMIMEDescription(void)
{
  return sMimeDescription;
}

NPError
NP_Initialize(NPNetscapeFuncs* aNPNFuncs, NPPluginFuncs* aNPPFuncs)
{
  sNPNFuncs = aNPNFuncs;

  aNPPFuncs->newp     = NPP_New;
  aNPPFuncs->destroy  = NPP_Destroy;
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
NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
  return NPERR_GENERIC_ERROR;
}
