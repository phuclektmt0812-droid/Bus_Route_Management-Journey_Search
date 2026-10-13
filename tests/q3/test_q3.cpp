#include "BusRoute.h"
#include "tests/TestUtils.h"

static SLinkedList<BusStop> makeStops(const initializer_list<string>& ids) {
    SLinkedList<BusStop> stops;
    for (const string& id : ids) stops.add(BusStop(id));
    return stops;
}

int main(int argc, char* argv[]) {
    PublicTestSuite suite("Q3 - BusRoute");

    suite.add("Fresh route state", []() {
        BusRoute fresh("NEW");
        requireTrue(!fresh.isBuilt(), "new route is not built");
        requireEqual(fresh.getStopCount(OUTBOUND), 0, "unbuilt route has zero outbound stops");
        requireEqual(fresh.getStopCount(INBOUND), 0, "unbuilt route has zero inbound stops");
        requireOutOfRange([&]() { fresh.getStop(0, OUTBOUND); }, "getStop on unbuilt route is out of range");
        requireEqual(fresh.getHopCount("A", "B", OUTBOUND), -1, "unbuilt route has no valid journey");
    });

    suite.add("Build route and preserve input lists", []() {
        SLinkedList<BusStop> outbound = makeStops({"A", "B", "C", "D", "E"});
        SLinkedList<BusStop> inbound = makeStops({"E", "X", "C", "B", "A"});
        BusRoute route("R1");
        route.build(outbound, inbound);

        requireTrue(route.isBuilt(), "route marked built");
        requireEqual(outbound.size(), 5, "build keeps outbound size");
        requireEqual(inbound.size(), 5, "build keeps inbound size");
        requireEqual(outbound.get(0).getId(), string("A"), "build does not modify outbound");
        requireEqual(inbound.get(1).getId(), string("X"), "build does not modify inbound");
        requireEqual(route.getStopCount(OUTBOUND), 5, "outbound stop count");
        requireEqual(route.getStopCount(INBOUND), 5, "inbound stop count");
    });

    suite.add("Directional stop views", []() {
        SLinkedList<BusStop> outbound = makeStops({"A", "B", "C", "D", "E"});
        SLinkedList<BusStop> inbound = makeStops({"E", "X", "C", "B", "A"});
        BusRoute route("R1");
        route.build(outbound, inbound);

        requireEqual(route.getStop(0, OUTBOUND).getId(), string("A"), "outbound start");
        requireEqual(route.getStop(1, OUTBOUND).getId(), string("B"), "outbound second stop");
        requireEqual(route.getStop(4, OUTBOUND).getId(), string("E"), "outbound terminal");
        requireEqual(route.getStop(0, INBOUND).getId(), string("E"), "inbound start");
        requireEqual(route.getStop(1, INBOUND).getId(), string("X"), "inbound second stop");
        requireEqual(route.getStop(2, INBOUND).getId(), string("C"), "shared stop in inbound view");
        requireEqual(route.getStop(3, INBOUND).getId(), string("B"), "second shared stop in inbound view");
        requireEqual(route.getStop(4, INBOUND).getId(), string("A"), "inbound terminal wraps to route start");
    });

    suite.add("Stop index boundaries", []() {
        SLinkedList<BusStop> outbound = makeStops({"A", "B", "C", "D", "E"});
        SLinkedList<BusStop> inbound = makeStops({"E", "X", "C", "B", "A"});
        BusRoute route("R1");
        route.build(outbound, inbound);

        requireOutOfRange([&]() { route.getStop(-1, OUTBOUND); }, "negative route index");
        requireOutOfRange([&]() { route.getStop(5, OUTBOUND); }, "outbound index == count");
        requireOutOfRange([&]() { route.getStop(5, INBOUND); }, "inbound index == count");
    });

    suite.add("Directional hop counts", []() {
        SLinkedList<BusStop> outbound = makeStops({"A", "B", "C", "D", "E"});
        SLinkedList<BusStop> inbound = makeStops({"E", "X", "C", "B", "A"});
        BusRoute route("R1");
        route.build(outbound, inbound);

        requireEqual(route.getHopCount("A", "D", OUTBOUND), 3, "A to D outbound");
        requireEqual(route.getHopCount("D", "A", OUTBOUND), -1, "wrong outbound order");
        requireEqual(route.getHopCount("E", "B", INBOUND), 3, "E to B inbound");
        requireEqual(route.getHopCount("C", "A", INBOUND), 2, "C to A inbound");
        requireEqual(route.getHopCount("C", "C", OUTBOUND), 0, "same existing stop has zero hops");
        requireEqual(route.getHopCount("X", "X", OUTBOUND), -1, "same absent stop is not a zero-hop journey");
        requireEqual(route.getHopCount("X", "A", OUTBOUND), -1, "missing origin in selected direction");
        requireEqual(route.getHopCount("A", "X", OUTBOUND), -1, "missing destination in selected direction");
    });

    suite.add("Two-stop route and BusStop data", []() {
        SLinkedList<BusStop> namedOut;
        namedOut.add(BusStop("N1", "North"));
        namedOut.add(BusStop("N2", "South"));
        SLinkedList<BusStop> namedIn;
        namedIn.add(BusStop("N2", "South"));
        namedIn.add(BusStop("N1", "North"));

        BusRoute named("RN");
        named.build(namedOut, namedIn);

        requireEqual(named.getStopCount(OUTBOUND), 2, "two-stop outbound route");
        requireEqual(named.getStopCount(INBOUND), 2, "two-stop inbound route");
        requireEqual(named.getStop(0, OUTBOUND).getName(), string("North"), "build preserves BusStop data");
        requireEqual(named.getStop(0, INBOUND).getName(), string("South"), "inbound terminal preserves BusStop data");
        requireEqual(named.getHopCount("N1", "N2", OUTBOUND), 1, "two-stop outbound hop count");
        requireEqual(named.getHopCount("N2", "N1", INBOUND), 1, "two-stop inbound hop count");
    });

    suite.add("Rebuild replaces the previous route", []() {
        SLinkedList<BusStop> outbound = makeStops({"A", "B", "C", "D", "E"});
        SLinkedList<BusStop> inbound = makeStops({"E", "X", "C", "B", "A"});
        BusRoute route("R1");
        route.build(outbound, inbound);

        SLinkedList<BusStop> outbound2 = makeStops({"P", "Q", "R"});
        SLinkedList<BusStop> inbound2 = makeStops({"R", "Z", "P"});
        route.build(outbound2, inbound2);

        requireEqual(route.getStopCount(OUTBOUND), 3, "rebuild replaces outbound view");
        requireEqual(route.getStopCount(INBOUND), 3, "rebuild replaces inbound view");
        requireEqual(route.getStop(0, OUTBOUND).getId(), string("P"), "rebuild new outbound start");
        requireEqual(route.getStop(1, INBOUND).getId(), string("Z"), "rebuild new inbound content");
        requireEqual(route.getHopCount("A", "E", OUTBOUND), -1, "old route content removed on rebuild");
    });

    return suite.run(argc, argv);
}
