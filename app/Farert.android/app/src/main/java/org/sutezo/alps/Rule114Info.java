package org.sutezo.alps;

public class Rule114Info {
    public Fare fare_114 = new Fare();
    public int apply_terminal_station;

    public Rule114Info() {}
    public Rule114Info(Rule114Info other) {
        set(other);
    }
    public Rule114Info(Fare fare_, int station_id_) {
        fare_114.set(fare_);
        apply_terminal_station = station_id_;
    }
    public void set(Rule114Info other) {
        fare_114.set(other.fare_114);
        apply_terminal_station = other.apply_terminal_station;
    }
    public void clear() {
        fare_114.clear();
        apply_terminal_station = 0;
    }
    public String stationName() { return RouteUtil.StationName(apply_terminal_station); }
    public int sales_km() { return fare_114.sales_km; }
    public int calc_km() { return fare_114.calc_km; }
    public int fare() { return fare_114.fare; }
    public boolean isAvailable() { return 0 < fare_114.fare; }
}
