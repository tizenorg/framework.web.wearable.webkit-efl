/*
   Copyright (C) 2012 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "ewk_user_media.h"

#if ENABLE(TIZEN_MEDIA_STREAM)
#include "WKAPICast.h"
#include "WKUserMediaPermissionRequest.h"
#include "WKRetainPtr.h"
#include "ewk_security_origin_private.h"
#include "ewk_view_private.h"
#include <WebKit2/WKBase.h>

struct _Ewk_User_Media_Permission_Request {
    WKRetainPtr<WKUserMediaPermissionRequestRef> requestRef;
    Evas_Object* ewkView;
    bool isDecided;
    bool isSuspended;
    RefPtr<EwkSecurityOrigin> m_securityOrigin;

    _Ewk_User_Media_Permission_Request(Evas_Object* ewkView, WKRetainPtr<WKUserMediaPermissionRequestRef> userMediaPermissionRequestRef, WKSecurityOriginRef wkOrigin)
        : requestRef(userMediaPermissionRequestRef)
        , ewkView(ewkView)
        , isDecided(false)
        , isSuspended(false)
        , m_securityOrigin(EwkSecurityOrigin::create(wkOrigin))
    { }
};

const Ewk_Security_Origin* ewk_user_media_permission_request_origin_get(const Ewk_User_Media_Permission_Request* request)
{
#if ENABLE(TIZEN_MEDIA_STREAM)
    EINA_SAFETY_ON_NULL_RETURN_VAL(request, nullptr);

    return request->m_securityOrigin.get();
#else
    UNUSED_PARAM(request);

    return nullptr;
#endif
}

Ewk_User_Media_Permission_Request* ewkUserMediaPermissionRequestCreate(Evas_Object* ewkView, WKUserMediaPermissionRequestRef userMediaPermissionRequestRef, WKSecurityOriginRef wkOrigin)
{
    return new Ewk_User_Media_Permission_Request(ewkView, userMediaPermissionRequestRef, wkOrigin);
}

void ewkUserMediaDeletePermissionRequestList(Eina_List* requestList)
{
    void* data = 0;
    EINA_LIST_FREE(requestList, data)
        delete (static_cast<Ewk_User_Media_Permission_Request*>(data));
}

bool ewkUserMediaPermissionRequestDecided(Ewk_User_Media_Permission_Request* permissionRequest)
{
    return permissionRequest->isDecided;
}

bool ewkUserMediaPermissionRequestSuspended(Ewk_User_Media_Permission_Request* permissionRequest)
{
    return permissionRequest->isSuspended;
}
#endif

Eina_Bool ewk_user_media_permission_request_suspend(Ewk_User_Media_Permission_Request* permissionRequest)
{
#if ENABLE(TIZEN_MEDIA_STREAM)
    EINA_SAFETY_ON_NULL_RETURN_VAL(permissionRequest, false);
    EINA_SAFETY_ON_NULL_RETURN_VAL(permissionRequest->requestRef, false);

    permissionRequest->isSuspended = true;

    return true;
#else
    UNUSED_PARAM(permissionRequest);
    return false;
#endif
}

void ewk_user_media_permission_request_set(Ewk_User_Media_Permission_Request* permissionRequest, Eina_Bool allowed)
{
#if ENABLE(TIZEN_MEDIA_STREAM)
    EINA_SAFETY_ON_NULL_RETURN(permissionRequest);

    permissionRequest->isDecided = true;

    if (allowed)
        WKUserMediaPermissionRequestAllow(permissionRequest->requestRef.get());
    else
        WKUserMediaPermissionRequestDeny(permissionRequest->requestRef.get());

    ewkViewDeleteUserMediaPermissionRequest(permissionRequest->ewkView, permissionRequest);
    delete permissionRequest;
#else
    UNUSED_PARAM(permissionRequest);
    UNUSED_PARAM(allowed);
#endif
}
