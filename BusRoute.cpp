#include "BusRoute.h"

BusRoute::BusRoute(string routeId)
    : routeId(routeId), outboundCount(0), built(false) {}

string BusRoute::getId() const {
    return routeId;
}

bool BusRoute::isBuilt() const {
    return built;
}

int BusRoute::getStopCount(Direction direction) {
    // TODO Q3
    if (!built) return 0;
    if (direction == OUTBOUND) {
        return outboundCount;
    }
    if (direction == INBOUND) {
        return stops.size() - outboundCount + 1;
    }
    return 0;
}

int BusRoute::physicalIndex(int index, Direction direction) {
    // TODO Q3
    if (!built) throw out_of_range("BusRoute is not built");
    if (direction == OUTBOUND) {
        if (index < 0 || index >= outboundCount) {
            throw out_of_range("BusRoute outbound index is out of range");
        }
        return index;
    }
    if (direction == INBOUND) {
        int inboundCount = getStopCount(INBOUND);
        if (index < 0 || index >= inboundCount) {
            throw out_of_range("BusRoute inbound index is out of range");
        }
        if (outboundCount == 0) return index;
        return outboundCount - 1 + index;
    }
    throw out_of_range("BusRoute direction is out of range");
}

BusStop& BusRoute::getStop(int index, Direction direction) {
    if (!built) throw out_of_range("BusRoute is not built");
    return stops.get(physicalIndex(index, direction));
}

void BusRoute::build(SLinkedList<BusStop>& outbound, SLinkedList<BusStop>& inbound) {
    // TODO Q3
    stops.clear();
    outboundCount = outbound.size();
    built = true;
    for (int i = 0; i < outboundCount; ++i) {
        stops.add(outbound.get(i));
    }

    for (int i = 1; i < inbound.size(); ++i) {
        stops.add(inbound.get(i));
    }
}
int BusRoute::getHopCount(string fromStopId, string toStopId, Direction direction) {
    // TODO Q3
    if (!built) return -1;
    int count = getStopCount(direction);
    if (count <= 0) return -1;
    int fromIndex = -1;
    int toIndex = -1;
    for (int i = 0; i < count; ++i) {
        if (getStop(i, direction).getId() == fromStopId) fromIndex = i;
        if (getStop(i, direction).getId() == toStopId) toIndex = i;
    }
    if (fromStopId == toStopId) {
        return (fromIndex >= 0) ? 0 : -1;
    }
    if (fromIndex < 0 || toIndex < 0) return -1;
    if (fromIndex > toIndex) return -1;
    return toIndex - fromIndex;
}
