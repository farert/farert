package org.sutezo.alps;

class TestFARE_INFO extends FARE_INFO {

    @Override
    FareResult roundTripFareWithCompanyLine() {
        FareResult fareW = new FareResult();
        fareW.isDiscount = false;
        int jrFareApplied = getFareForDisplay() - company_fare;
        fareW.fare = jrFareApplied * 2 + company_fare * 2;
        return fareW;
    }

    @Override
    int roundTripAcademicFareWithCompanyLine() {
        int fareBrt = brt_fare - brt_discount_fare;
        int fareS;
        if (1000 < total_jr_sales_km) {
            int jrFareApplied = getFareForDisplay() - company_fare;
            fareS = fare_discount((jrFareApplied - fareBrt), 2)
                    + fare_discount(fareBrt, 2);
        } else {
            fareS = getFareForDisplay() - company_fare;
        }
        return (fareS + (company_fare - company_fare_ac_discount)) * 2;
    }
}
