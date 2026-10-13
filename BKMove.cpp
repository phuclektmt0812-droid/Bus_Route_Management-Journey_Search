#include "BKMove.h"

namespace {

static int compareRouteResults(RouteResult& lhs, RouteResult& rhs) {
    if (lhs.hopCount != rhs.hopCount) {
        return lhs.hopCount < rhs.hopCount ? -1 : 1;
    }
    if (lhs.routeId != rhs.routeId) {
        return lhs.routeId < rhs.routeId ? -1 : 1;
    }
    if (lhs.direction != rhs.direction) {
        return lhs.direction < rhs.direction ? -1 : 1;
    }
    return 0;
}

static int compareJourneyResults(JourneyResult& lhs, JourneyResult& rhs) {
    if (lhs.transfers != rhs.transfers) {
        return lhs.transfers < rhs.transfers ? -1 : 1;
    }
    if (lhs.totalHops != rhs.totalHops) {
        return lhs.totalHops < rhs.totalHops ? -1 : 1;
    }
    if (lhs.firstRouteId != rhs.firstRouteId) {
        return lhs.firstRouteId < rhs.firstRouteId ? -1 : 1;
    }
    if (lhs.firstDirection != rhs.firstDirection) {
        return lhs.firstDirection < rhs.firstDirection ? -1 : 1;
    }
    if (lhs.transferStopId != rhs.transferStopId) {
        return lhs.transferStopId < rhs.transferStopId ? -1 : 1;
    }
    if (lhs.secondRouteId != rhs.secondRouteId) {
        return lhs.secondRouteId < rhs.secondRouteId ? -1 : 1;
    }
    if (lhs.secondDirection != rhs.secondDirection) {
        return lhs.secondDirection < rhs.secondDirection ? -1 : 1;
    }
    return 0;
}

}

void BKMove::addRoute(BusRoute* route) {
    if (route == nullptr) throw invalid_argument("route must not be null");
    routes.push_back(route);
}

int BKMove::getRouteCount() const {
    return static_cast<int>(routes.size());
}

BusRoute* BKMove::getRoute(int index) {
    if (index < 0 || index >= static_cast<int>(routes.size())) {
        throw out_of_range("BKMove route index is out of range");
    }
    return routes[index];
}

vector<RouteResult> BKMove::findDirectRoutes(string fromStopId, string toStopId) {
    // TODO Q5.1
    vector<RouteResult> direct;
    for (int i = 0; i < getRouteCount(); ++i) {
        BusRoute* route = getRoute(i);
        for (int d = OUTBOUND; d <= INBOUND; ++d) {
            Direction direction = static_cast<Direction>(d);
            int hopCount = route->getHopCount(fromStopId, toStopId, direction);
            if (hopCount >= 0) {
                direct.push_back(RouteResult(route->getId(), direction, hopCount));
            }
        }
    }
    if (!direct.empty()) {
        QuickSort<RouteResult> sorter;
        sorter.sort(direct.data(), static_cast<int>(direct.size()), compareRouteResults);
    }
    return direct;
}

vector<JourneyResult> BKMove::findJourneys(string fromStopId, string toStopId) {
    // TODO Q5.2
    vector<JourneyResult> journeys;
    vector<RouteResult> direct = findDirectRoutes(fromStopId, toStopId);
    for (const RouteResult& result : direct) {
        journeys.push_back(JourneyResult(result.routeId, result.direction, result.hopCount));
    }
    for (int i = 0; i < getRouteCount(); ++i) {
        BusRoute* firstRoute = getRoute(i);
        for (int d = OUTBOUND; d <= INBOUND; ++d) {
            Direction firstDirection = static_cast<Direction>(d);
            int firstCount = firstRoute->getStopCount(firstDirection);

            for (int j = 0; j < firstCount; ++j) {
                string transferStopId = firstRoute->getStop(j, firstDirection).getId();
                if (transferStopId == fromStopId || transferStopId == toStopId) continue;
                int firstHopCount = firstRoute->getHopCount(fromStopId, transferStopId, firstDirection);
                if (firstHopCount < 0) continue;
                for (int k = 0; k < getRouteCount(); ++k) {
                    if (k == i) continue;
                    BusRoute* secondRoute = getRoute(k);
                    for (int s = OUTBOUND; s <= INBOUND; ++s) {
                        Direction secondDirection = static_cast<Direction>(s);
                        int secondHopCount = secondRoute->getHopCount(transferStopId, toStopId, secondDirection);
                        if (secondHopCount < 0) continue;
                        JourneyResult candidate(
                            firstRoute->getId(),
                            firstDirection,
                            transferStopId,
                            secondRoute->getId(),
                            secondDirection,
                            firstHopCount + secondHopCount);
                        bool duplicate = false;
                        for (const JourneyResult& existing : journeys) {
                            if (existing == candidate) {
                                duplicate = true;
                                break;
                            }
                        }
                        if (!duplicate) {
                            journeys.push_back(candidate);
                        }
                    }
                }
            }
        }
    }
    if (!journeys.empty()) {
        QuickSort<JourneyResult> sorter;
        sorter.sort(journeys.data(), static_cast<int>(journeys.size()), compareJourneyResults);
    }
    return journeys;
}
