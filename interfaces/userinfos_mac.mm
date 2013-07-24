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
