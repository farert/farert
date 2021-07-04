package org.sutezo.alps;

public class Fare {
    public int fare;
    public int sales_km;
    public int calc_km;
    public Fare() { fare = sales_km = calc_km = 0; }
    public Fare(int f, int sk, int ck) {
        set(f, sk, ck);
    }
    public void set(int f, int sk, int ck) {
        fare = f;
        sales_km = sk;
        calc_km = ck;
    }
    public void set(Fare other) {
        fare = other.fare;
        sales_km = other.sales_km;
        calc_km = other.calc_km;
    }
    public void clear() { fare = sales_km = calc_km = 0; }
}
