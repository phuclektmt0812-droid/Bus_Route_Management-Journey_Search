#ifndef __MAIN_H__
#define __MAIN_H__

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "utils.h"

using namespace std;

enum Direction {
    OUTBOUND = 0,
    INBOUND = 1
};

class BusStop {
private:
    string id;
    string name;

public:
    BusStop(string id = "", string name = "") : id(id), name(name) {}

    string getId() const { return id; }
    string getName() const { return name; }

    bool operator==(const BusStop& other) const { return id == other.id; }
    bool operator!=(const BusStop& other) const { return !(*this == other); }

    string toString() const {
        if (name.empty()) return id;
        return id + " (" + name + ")";
    }
};

inline ostream& operator<<(ostream& os, const BusStop& stop) {
    os << stop.toString();
    return os;
}

struct RouteResult {
    string routeId;
    Direction direction;
    int hopCount;

    RouteResult(string routeId = "", Direction direction = OUTBOUND, int hopCount = -1)
        : routeId(routeId), direction(direction), hopCount(hopCount) {}

    bool operator<(const RouteResult& other) const {
        if (routeId != other.routeId) return routeId < other.routeId;
        if (direction != other.direction) return direction < other.direction;
        return hopCount < other.hopCount;
    }
    bool operator>(const RouteResult& other) const { return other < *this; }

    bool operator==(const RouteResult& other) const {
        return routeId == other.routeId &&
               direction == other.direction &&
               hopCount == other.hopCount;
    }

    string toString() const {
        stringstream ss;
        ss << routeId << " | "
           << (direction == OUTBOUND ? "OUTBOUND" : "INBOUND")
           << " | " << hopCount << " hops";
        return ss.str();
    }
};

struct JourneyResult {
    int transfers;

    string firstRouteId;
    Direction firstDirection;

    string transferStopId;

    string secondRouteId;
    Direction secondDirection;

    int totalHops;

    JourneyResult()
        : transfers(0), firstDirection(OUTBOUND),
          secondDirection(OUTBOUND), totalHops(-1) {}

    JourneyResult(string routeId, Direction direction, int totalHops)
        : transfers(0), firstRouteId(routeId), firstDirection(direction),
          transferStopId(""), secondRouteId(""), secondDirection(OUTBOUND),
          totalHops(totalHops) {}

    JourneyResult(string firstRouteId,
                  Direction firstDirection,
                  string transferStopId,
                  string secondRouteId,
                  Direction secondDirection,
                  int totalHops)
        : transfers(1), firstRouteId(firstRouteId), firstDirection(firstDirection),
          transferStopId(transferStopId), secondRouteId(secondRouteId),
          secondDirection(secondDirection), totalHops(totalHops) {}

    bool operator<(const JourneyResult& other) const {
        if (firstRouteId != other.firstRouteId) return firstRouteId < other.firstRouteId;
        if (secondRouteId != other.secondRouteId) return secondRouteId < other.secondRouteId;
        if (transferStopId != other.transferStopId) return transferStopId < other.transferStopId;
        return totalHops < other.totalHops;
    }
    bool operator>(const JourneyResult& other) const { return other < *this; }

    bool operator==(const JourneyResult& other) const {
        return transfers == other.transfers &&
               firstRouteId == other.firstRouteId &&
               firstDirection == other.firstDirection &&
               transferStopId == other.transferStopId &&
               secondRouteId == other.secondRouteId &&
               secondDirection == other.secondDirection &&
               totalHops == other.totalHops;
    }

    string toString() const {
        stringstream ss;
        if (transfers == 0) {
            ss << firstRouteId << " | "
               << (firstDirection == OUTBOUND ? "OUTBOUND" : "INBOUND")
               << " | direct | " << totalHops << " hops";
        } else {
            ss << firstRouteId << " | "
               << (firstDirection == OUTBOUND ? "OUTBOUND" : "INBOUND")
               << " -> " << transferStopId << " -> "
               << secondRouteId << " | "
               << (secondDirection == OUTBOUND ? "OUTBOUND" : "INBOUND")
               << " | " << totalHops << " hops";
        }
        return ss.str();
    }
};

#endif // __MAIN_H__
