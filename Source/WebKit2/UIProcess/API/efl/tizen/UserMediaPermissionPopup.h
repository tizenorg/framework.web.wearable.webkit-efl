/*
   Copyright (C) 2014 Samsung Electronics

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

#ifndef UserMediaPermissionPopup_h
#define UserMediaPermissionPopup_h

#if ENABLE(TIZEN_MEDIA_STREAM)

#include "PermissionPopup.h"
#include "ewk_user_media.h"

namespace WebKit {

class UserMediaPermissionPopup : public PermissionPopup {
public:
    UserMediaPermissionPopup(Ewk_User_Media_Permission_Request*, const Ewk_Security_Origin*, const String&);
    ~UserMediaPermissionPopup() { };

    virtual void sendDecidedPermission(Evas_Object*, bool isDecided) OVERRIDE;

private:
    Ewk_User_Media_Permission_Request* m_request;
};

} // namespace WebKit

#endif // ENABLE(TIZEN_MEDIA_STREAM)

#endif // UserMediaPermissionPopup_h
