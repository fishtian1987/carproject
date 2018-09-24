#ifndef ROUTEINFO_H
#define ROUTEINFO_H

class RouteInfo
{
public:
    RouteInfo();
    bool operator != (const RouteInfo & );
    RouteInfo& operator = (const RouteInfo &);
public:
    double lat;
    double log;
};

#endif // ROUTEINFO_H
