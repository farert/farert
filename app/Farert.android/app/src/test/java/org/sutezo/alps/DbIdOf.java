package org.sutezo.alps;

public class DbIdOf {
    public static final DbIdOf INSTANCE = new DbIdOf();

    private DbIdOf() {
    }

    public int station(String name) {
        return RouteUtil.GetStationId(name);
    }

    public int line(String name) {
        return RouteUtil.GetLineId(name);
    }
}
