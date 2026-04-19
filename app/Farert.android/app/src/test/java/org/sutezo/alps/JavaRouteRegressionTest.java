package org.sutezo.alps;

import org.junit.Test;

public class JavaRouteRegressionTest {
    @Test
    public void routeRegressionMatchesReference() throws Exception {
        JavaTestMain.main(new String[] {"-exec"});
    }
}
