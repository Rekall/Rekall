//
//  Delegate.m
//  CoreLocationCLI
//
//  Created by Full Decent on 10-7-30.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include "location.h"
#import <Cocoa/Cocoa.h>
#import <CoreLocation/CoreLocation.h>

void Location::start() {
    locationManager = [[CLLocationManager alloc] init];
    //((CLLocationManager*)locationManager).delegate = (NSObject<NSApplicationDelegate, CLLocationManagerDelegate>*)this;
    [locationManager startUpdatingLocation];
}

const QString Location::getGPS() {
    float latitude  = ((CLLocationManager*)locationManager).location.coordinate.latitude;
    float longitude = ((CLLocationManager*)locationManager).location.coordinate.longitude;
    float altitude  = ((CLLocationManager*)locationManager).location.altitude;

    if((latitude != 0) && (longitude != 0) && (altitude != 0))
        return QString("%1, %2, %3").arg(latitude).arg(longitude).arg(altitude);
    else
        return QString();
}
