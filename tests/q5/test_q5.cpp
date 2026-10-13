#include "BKMove.h"
#include "tests/TestUtils.h"

static void buildRoute(BusRoute& route,
                       const initializer_list<string>& outIds,
                       const initializer_list<string>& inIds) {
    SLinkedList<BusStop> out;
    SLinkedList<BusStop> in;
    for (const string& id : outIds) out.add(BusStop(id));
    for (const string& id : inIds) in.add(BusStop(id));
    route.build(out, in);
}

static int journeyIndex(const vector<JourneyResult>& journeys,
                        int transfers,
                        const string& firstRouteId,
                        Direction firstDirection,
                        const string& transferStopId,
                        const string& secondRouteId,
                        Direction secondDirection) {
    for (int i = 0; i < static_cast<int>(journeys.size()); ++i) {
        const JourneyResult& j = journeys[i];
        if (j.transfers == transfers &&
            j.firstRouteId == firstRouteId &&
            j.firstDirection == firstDirection &&
            j.transferStopId == transferStopId &&
            j.secondRouteId == secondRouteId &&
            j.secondDirection == secondDirection) {
            return i;
        }
    }
    return -1;
}

int main(int argc, char* argv[]) {
    PublicTestSuite suite("Q5 - BKMove");

    suite.add("Direct route discovery and hop-count ranking", []() {
        BusRoute r1("R1");
        buildRoute(r1, {"A", "B", "C", "D"}, {"D", "C", "B", "A"});
        BusRoute r2("R2");
        buildRoute(r2, {"A", "M", "D"}, {"D", "M", "A"});
        BusRoute r0("R0");
        buildRoute(r0, {"A", "C", "D", "E"}, {"E", "C", "D", "A"});

        BKMove map;
        map.addRoute(&r1);
        map.addRoute(&r2);
        map.addRoute(&r0);

        vector<RouteResult> direct = map.findDirectRoutes("A", "D");
        requireEqual(static_cast<int>(direct.size()), 3, "one result per valid route-direction");
        requireEqual(direct[0].routeId, string("R0"), "equal-hop direct results use route ID tie-break");
        requireEqual(direct[0].hopCount, 2, "R0 direct hops");
        requireEqual(direct[1].routeId, string("R2"), "second equal-hop route ID");
        requireEqual(direct[1].hopCount, 2, "R2 direct hops");
        requireEqual(direct[2].routeId, string("R1"), "larger hop count ranks later");
        requireEqual(direct[2].hopCount, 3, "R1 direct hops");
    });

    suite.add("Direct direction tie-break", []() {
        BusRoute r0("R0");
        buildRoute(r0, {"A", "C", "D", "E"}, {"E", "C", "D", "A"});

        BKMove map;
        map.addRoute(&r0);

        vector<RouteResult> direct = map.findDirectRoutes("C", "D");
        requireEqual(static_cast<int>(direct.size()), 2, "OUTBOUND and INBOUND are distinct direct results");
        requireTrue(direct[0].direction == OUTBOUND, "OUTBOUND ranks before INBOUND on a full direct tie");
        requireTrue(direct[1].direction == INBOUND, "INBOUND follows OUTBOUND on a full direct tie");
    });

    suite.add("Direct journeys and empty direct result", []() {
        BusRoute r1("R1");
        buildRoute(r1, {"A", "B", "C", "D"}, {"D", "C", "B", "A"});
        BusRoute r2("R2");
        buildRoute(r2, {"A", "M", "D"}, {"D", "M", "A"});
        BKMove map;
        map.addRoute(&r1);
        map.addRoute(&r2);

        vector<RouteResult> direct = map.findDirectRoutes("A", "D");
        vector<JourneyResult> journeys = map.findJourneys("A", "D");

        int zeroTransferCount = 0;
        for (const JourneyResult& j : journeys) {
            if (j.transfers == 0) ++zeroTransferCount;
        }
        requireEqual(zeroTransferCount, static_cast<int>(direct.size()), "findJourneys includes every direct route-direction");
        for (int i = 0; i < static_cast<int>(direct.size()); ++i) {
            requireEqual(journeys[i].transfers, 0, "all direct journeys rank before transfer journeys");
            requireEqual(journeys[i].firstRouteId, direct[i].routeId, "direct journey preserves direct ranking");
            requireTrue(journeys[i].firstDirection == direct[i].direction, "direct journey preserves direction");
            requireEqual(journeys[i].totalHops, direct[i].hopCount, "direct journey preserves hop count");
            requireTrue(journeys[i].transferStopId.empty(), "direct journey has no transfer stop");
            requireTrue(journeys[i].secondRouteId.empty(), "direct journey has no second route");
        }

        requireTrue(map.findDirectRoutes("UNKNOWN", "D").empty(), "no direct option returns empty vector");
    });

    suite.add("One-transfer discovery and transfer validity", []() {
        BusRoute directLong("R9");
        buildRoute(directLong,
                   {"A", "L1", "L2", "L3", "L4", "F"},
                   {"F", "L4", "L3", "L2", "L1", "A"});
        BusRoute first("R1");
        buildRoute(first,
                   {"A", "B", "C", "D", "E"},
                   {"E", "D", "C", "B", "A"});
        BusRoute second("R2");
        buildRoute(second,
                   {"X", "C", "Y", "D", "F"},
                   {"F", "D", "Y", "C", "X"});
        BusRoute firstTie("R3");
        buildRoute(firstTie,
                   {"A", "P", "C", "D", "E"},
                   {"E", "D", "C", "P", "A"});

        BKMove map;
        map.addRoute(&directLong);
        map.addRoute(&first);
        map.addRoute(&second);
        map.addRoute(&firstTie);

        vector<JourneyResult> journeys = map.findJourneys("A", "F");
        requireTrue(!journeys.empty(), "A to F has journeys");
        requireEqual(journeys[0].transfers, 0, "fewer transfers rank before fewer hops");
        requireEqual(journeys[0].firstRouteId, string("R9"), "direct journey is first");
        requireEqual(journeys[0].totalHops, 5, "direct journey hop count");

        bool foundR1C = false;
        bool foundR1D = false;
        bool foundR3C = false;
        bool invalidEndpointTransfer = false;
        bool sameRouteTransfer = false;

        for (const JourneyResult& j : journeys) {
            if (j.transfers == 1) {
                requireTrue(j.firstRouteId != j.secondRouteId, "transfer uses different route IDs");
                if (j.transferStopId == "A" || j.transferStopId == "F") invalidEndpointTransfer = true;
                if (j.firstRouteId == j.secondRouteId) sameRouteTransfer = true;
            }

            if (j.transfers == 1 && j.firstRouteId == "R1" && j.secondRouteId == "R2" &&
                j.transferStopId == "C" && j.totalHops == 5) foundR1C = true;
            if (j.transfers == 1 && j.firstRouteId == "R1" && j.secondRouteId == "R2" &&
                j.transferStopId == "D" && j.totalHops == 4) foundR1D = true;
            if (j.transfers == 1 && j.firstRouteId == "R3" && j.secondRouteId == "R2" &&
                j.transferStopId == "C" && j.totalHops == 5) foundR3C = true;
        }

        requireTrue(foundR1C, "each valid transfer stop creates a journey: C");
        requireTrue(foundR1D, "each valid transfer stop creates a journey: D");
        requireTrue(foundR3C, "different first route creates a distinct journey");
        requireTrue(!invalidEndpointTransfer, "transfer is not the origin or destination");
        requireTrue(!sameRouteTransfer, "no transfer within the same route ID");
    });

    suite.add("Journey ranking by total hops and first route ID", []() {
        BusRoute first("R1");
        buildRoute(first,
                   {"A", "B", "C", "D", "E"},
                   {"E", "D", "C", "B", "A"});
        BusRoute second("R2");
        buildRoute(second,
                   {"X", "C", "Y", "D", "F"},
                   {"F", "D", "Y", "C", "X"});
        BusRoute firstTie("R3");
        buildRoute(firstTie,
                   {"A", "P", "C", "D", "E"},
                   {"E", "D", "C", "P", "A"});

        BKMove map;
        map.addRoute(&first);
        map.addRoute(&second);
        map.addRoute(&firstTie);

        vector<JourneyResult> journeys = map.findJourneys("A", "F");
        int posR1D = journeyIndex(journeys, 1, "R1", OUTBOUND, "D", "R2", OUTBOUND);
        int posR1C = journeyIndex(journeys, 1, "R1", OUTBOUND, "C", "R2", OUTBOUND);
        int posR3C = journeyIndex(journeys, 1, "R3", OUTBOUND, "C", "R2", OUTBOUND);

        requireTrue(posR1D >= 0 && posR1C >= 0 && posR3C >= 0, "ranked transfer samples exist");
        requireTrue(posR1D < posR1C, "fewer total hops rank first among equal-transfer journeys");
        requireTrue(posR1C < posR3C, "first route ID breaks the next tie");
    });

    suite.add("First direction tie-break", []() {
        BusRoute first("R1");
        buildRoute(first, {"S", "A", "C", "T"}, {"T", "A", "C", "S"});
        BusRoute second("R2");
        buildRoute(second, {"X", "C", "F", "Y"}, {"Y", "F", "C", "X"});
        BKMove map;
        map.addRoute(&first);
        map.addRoute(&second);

        vector<JourneyResult> journeys = map.findJourneys("A", "F");
        int outPos = journeyIndex(journeys, 1, "R1", OUTBOUND, "C", "R2", OUTBOUND);
        int inPos = journeyIndex(journeys, 1, "R1", INBOUND, "C", "R2", OUTBOUND);
        requireTrue(outPos >= 0 && inPos >= 0, "both first-route directions can create distinct journeys");
        requireTrue(outPos < inPos, "first direction breaks a tie: OUTBOUND before INBOUND");
    });

    suite.add("Transfer stop ID tie-break", []() {
        BusRoute first("R1");
        buildRoute(first, {"S", "A", "B", "C", "T"}, {"T", "C", "B", "A", "S"});
        BusRoute second("R2");
        buildRoute(second, {"X", "B", "C", "F", "Y"}, {"Y", "F", "C", "B", "X"});
        BKMove map;
        map.addRoute(&first);
        map.addRoute(&second);

        vector<JourneyResult> journeys = map.findJourneys("A", "F");
        int bPos = journeyIndex(journeys, 1, "R1", OUTBOUND, "B", "R2", OUTBOUND);
        int cPos = journeyIndex(journeys, 1, "R1", OUTBOUND, "C", "R2", OUTBOUND);
        requireTrue(bPos >= 0 && cPos >= 0, "two equal-cost transfer-stop choices are both returned");
        requireEqual(journeys[bPos].totalHops, journeys[cPos].totalHops, "transfer-stop ranking sample has equal total hops");
        requireTrue(bPos < cPos, "transfer stop ID breaks a tie lexicographically");
    });

    suite.add("Second route ID tie-break", []() {
        BusRoute first("R1");
        buildRoute(first, {"S", "A", "C", "T"}, {"T", "C", "A", "S"});
        BusRoute second3("R3");
        buildRoute(second3, {"X", "C", "F", "Y"}, {"Y", "F", "C", "X"});
        BusRoute second2("R2");
        buildRoute(second2, {"X2", "C", "F", "Y2"}, {"Y2", "F", "C", "X2"});
        BKMove map;
        map.addRoute(&first);
        map.addRoute(&second3);
        map.addRoute(&second2);

        vector<JourneyResult> journeys = map.findJourneys("A", "F");
        int r2Pos = journeyIndex(journeys, 1, "R1", OUTBOUND, "C", "R2", OUTBOUND);
        int r3Pos = journeyIndex(journeys, 1, "R1", OUTBOUND, "C", "R3", OUTBOUND);
        requireTrue(r2Pos >= 0 && r3Pos >= 0, "two equal-cost second routes are both returned");
        requireTrue(r2Pos < r3Pos, "second route ID breaks a tie lexicographically");
    });

    suite.add("Second direction tie-break", []() {
        BusRoute first("R1");
        buildRoute(first, {"S", "A", "C", "T"}, {"T", "C", "A", "S"});
        BusRoute second("R2");
        buildRoute(second, {"X", "C", "F", "Y"}, {"Y", "C", "F", "X"});
        BKMove map;
        map.addRoute(&first);
        map.addRoute(&second);

        vector<JourneyResult> journeys = map.findJourneys("A", "F");
        int outPos = journeyIndex(journeys, 1, "R1", OUTBOUND, "C", "R2", OUTBOUND);
        int inPos = journeyIndex(journeys, 1, "R1", OUTBOUND, "C", "R2", INBOUND);
        requireTrue(outPos >= 0 && inPos >= 0, "both second-route directions can create distinct journeys");
        requireTrue(outPos < inPos, "second direction breaks a tie: OUTBOUND before INBOUND");
    });

    suite.add("No journey returns an empty vector", []() {
        BusRoute route("R1");
        buildRoute(route, {"A", "B", "C"}, {"C", "B", "A"});
        BKMove map;
        map.addRoute(&route);
        requireTrue(map.findJourneys("UNKNOWN", "F").empty(), "no journey returns empty vector");
    });

    return suite.run(argc, argv);
}
