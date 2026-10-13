#ifndef BKMOVE_H
#define BKMOVE_H

#include "main.h"
#include "BusRoute.h"
#include "QuickSort.h"

#include <vector>
using namespace std;

class BKMove {
private:
    vector<BusRoute*> routes;

public:
    BKMove() = default;

    void addRoute(BusRoute* route);
    int getRouteCount() const;
    BusRoute* getRoute(int index);

    vector<RouteResult> findDirectRoutes(string fromStopId, string toStopId);
    vector<JourneyResult> findJourneys(string fromStopId, string toStopId);
};

#endif
