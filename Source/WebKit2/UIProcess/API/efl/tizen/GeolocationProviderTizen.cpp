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
#include "GeolocationProviderTizen.h"

#if ENABLE(TIZEN_GEOLOCATION)
#include "EwkView.h"
#include "WKGeolocationManager.h"
#include "WKGeolocationPosition.h"

using namespace WebKit;

static inline GeolocationProviderTizen* toGeolocationProviderTizen(const void* clientInfo)
{
    return static_cast<GeolocationProviderTizen*>(const_cast<void*>(clientInfo));
}

GeolocationProviderTizen::GeolocationProviderTizen(WKContextRef context)
    : m_manager(WKContextGetGeolocationManager(context))
    , m_locationManager(0)
    , m_started(false)
{
    ASSERT(m_manager);

    WKGeolocationProvider provider = {
        kWKGeolocationProviderCurrentVersion,
        this,
        startUpdating,
        stopUpdating
    };

    WKGeolocationManagerSetProvider(m_manager.get(), &provider);
}

GeolocationProviderTizen::~GeolocationProviderTizen()
{
    WKGeolocationManagerSetProvider(m_manager.get(), 0);

    stopLocationManager();
}

void GeolocationProviderTizen::startUpdating(WKGeolocationManagerRef geolocationManager, const void* clientInfo)
{
    GeolocationProviderTizen* provider = toGeolocationProviderTizen(clientInfo);
    EwkView* view = EwkView::currentEwkView();

    bool valid = true;
    if (view)
        view->smartCallback<EwkViewCallbacks::GeolocationValid>().call(&valid);
    else
        valid = false;

    if (!valid || !provider->startLocationManager())
        WKGeolocationManagerProviderDidFailToDeterminePosition(geolocationManager);
}

void GeolocationProviderTizen::stopUpdating(WKGeolocationManagerRef, const void* clientInfo)
{
    GeolocationProviderTizen* provider = toGeolocationProviderTizen(clientInfo);

    provider->stopLocationManager();
}

static inline double kilometerPerHourToMeterPerSecond(double kilometerPerHour)
{
    return kilometerPerHour * 5 / 18;
}

void GeolocationProviderTizen::didUpdateServiceState(location_service_state_e state, void* userData)
{
    GeolocationProviderTizen* provider = toGeolocationProviderTizen(userData);

    if (state == LOCATIONS_SERVICE_DISABLED) {
        provider->stopLocationManager();
        WKGeolocationManagerProviderDidFailToDeterminePosition(provider->m_manager.get());
    }
}

void GeolocationProviderTizen::didChangePosition(double latitude, double longitude, double altitude, time_t timestamp, void* userData)
{
    GeolocationProviderTizen* provider = toGeolocationProviderTizen(userData);
    if (!timestamp)
        return;

    location_accuracy_level_e dummyLevel;
    double accuracy;
    double altitudeAccuracy;

    if (!location_manager_get_accuracy(provider->m_locationManager, &dummyLevel, &accuracy, &altitudeAccuracy)) {
        if (accuracy < 0)
            accuracy = 0;

        if (altitudeAccuracy < 0)
            altitudeAccuracy = 0;
    } else
        accuracy = altitudeAccuracy = std::numeric_limits<double>::max();

    double dummyClimb;
    double heading;
    double speed;
    time_t dummyTimestamp;

    if (location_manager_get_velocity(provider->m_locationManager, &dummyClimb, &heading, &speed, &dummyTimestamp))
        speed = 0;

    if (speed)
        speed = kilometerPerHourToMeterPerSecond(speed);
    else
        heading = std::numeric_limits<double>::quiet_NaN();

    WKRetainPtr<WKGeolocationPositionRef> position(AdoptWK, WKGeolocationPositionCreate_b(timestamp, latitude, longitude, accuracy, true, altitude, true, altitudeAccuracy, true, heading, true, speed));

    WKGeolocationManagerProviderDidChangePosition(provider->m_manager.get(), position.get());
}

bool GeolocationProviderTizen::startLocationManager()
{
    if (m_started)
        return true;

    if (location_manager_create(LOCATIONS_METHOD_HYBRID, &m_locationManager)
        || location_manager_set_service_state_changed_cb(m_locationManager, didUpdateServiceState, this)
        || location_manager_set_position_updated_cb(m_locationManager, didChangePosition, 1, this)
        || location_manager_start(m_locationManager)) {
        location_manager_destroy(m_locationManager);
        m_locationManager = 0;
        return false;
    }

    m_started = true;

    return true;
}

void GeolocationProviderTizen::stopLocationManager()
{
    if (!m_started)
        return;

    m_started = false;

    location_manager_stop(m_locationManager);
    location_manager_unset_position_updated_cb(m_locationManager);
    location_manager_destroy(m_locationManager);
    m_locationManager = 0;
}
#endif // ENABLE(TIZEN_GEOLOCATION)
