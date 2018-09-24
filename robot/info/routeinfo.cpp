#include "routeinfo.h"

RouteInfo::RouteInfo():lat(0),log(0)
{
    this->lat=0;
    this->log=0;
}

bool RouteInfo::operator != (const RouteInfo &data)
{
    return (lat!=data.lat)||(log!=data.log);
}

RouteInfo& RouteInfo::operator = (const RouteInfo &data)
{
    if (this == &data)
        return *this;
    this->lat=data.lat;
    this->log=data.log;
    return *this;
}
