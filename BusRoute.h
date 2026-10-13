#ifndef BUSROUTE_H
#define BUSROUTE_H

#include "main.h"
#include "SLinkedList.h"
#include "CircularLinkedList.h"

class BusRoute {
private:
    string routeId;
    CircularLinkedList<BusStop> stops;
    int outboundCount;
    bool built;

private:
    int physicalIndex(int index, Direction direction);

public:
    explicit BusRoute(string routeId);

    BusRoute(const BusRoute&) = delete;
    BusRoute& operator=(const BusRoute&) = delete;

    string getId() const;
    bool isBuilt() const;

    void build(SLinkedList<BusStop>& outbound, SLinkedList<BusStop>& inbound);
    int getStopCount(Direction direction);
    BusStop& getStop(int index, Direction direction);
    int getHopCount(string fromStopId, string toStopId, Direction direction);
};

#endif
