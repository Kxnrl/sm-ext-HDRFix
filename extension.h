#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

#include <server_class.h>
#include "smsdk_ext.h"

class HDRFix : public SDKExtension
{
public:
	virtual bool SDK_OnLoad(char *error, size_t maxlength, bool late);
	virtual void SDK_OnUnload();
	virtual bool SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late);
	virtual bool SDK_OnMetamodUnload(char* error, size_t maxlength);

	void OnChangeLevel(const char* s1, const char* s2);
};

#endif
