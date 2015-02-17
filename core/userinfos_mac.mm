/*
    This file is part of Rekall.
    Copyright (C) 2013-2015

    Project Manager: Clarisse Bardiot
    Development & interactive design: Guillaume Jacquemin & Guillaume Marais (http://www.buzzinglight.com)

    This file was written by Guillaume Jacquemin.

    Rekall is a free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "userinfos.h"

#import <Cocoa/Cocoa.h>
#import <CoreLocation/CoreLocation.h>

void UserInfos::start_mac() {
    locationManager = [[CLLocationManager alloc] init];
    //((CLLocationManager*)locationManager).delegate = (NSObject<NSApplicationDelegate, CLLocationManagerDelegate>*)this;
    [locationManager startUpdatingLocation];
}

const QString UserInfos::getGPS_mac() {
    float latitude  = ((CLLocationManager*)locationManager).location.coordinate.latitude;
    float longitude = ((CLLocationManager*)locationManager).location.coordinate.longitude;
    float altitude  = ((CLLocationManager*)locationManager).location.altitude;

    if((latitude != 0) && (longitude != 0) && (altitude != 0))
        return QString("%1, %2, %3").arg(latitude).arg(longitude).arg(altitude);
    else
        return QString();
}

void UserInfos::setDockIcon_mac(bool toggle) const {
    ProcessSerialNumber psn = { 0, kCurrentProcess };
    if(toggle)
        TransformProcessType(&psn, kProcessTransformToForegroundApplication);
    else
        TransformProcessType(&psn, kProcessTransformToBackgroundApplication);
}

