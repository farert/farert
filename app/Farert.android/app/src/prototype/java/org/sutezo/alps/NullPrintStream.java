package org.sutezo.alps;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.nio.charset.StandardCharsets;

import android.util.Log;

public class NullPrintStream extends PrintStream {

    public NullPrintStream() {
        super(new NullByteArrayOutputStream());
    }

    private static class NullByteArrayOutputStream extends ByteArrayOutputStream {

        byte [] buffer = new byte[1024];
        int offset;

        NullByteArrayOutputStream() {
            offset = 0;
        }

        @Override
        public void write(int b) {
            byte[] ba = { (byte)b };
            Log.d("farert", new String(ba, StandardCharsets.UTF_8));
        }

        @Override
        public void write(byte[] b, int off, int len) {
            for (int i = off; i < (off + len); i++) {
                if (b[i] == 10) {
                    Log.d("farert", new String(buffer, 0, offset, StandardCharsets.UTF_8));
                    offset = 0;
                } else {
                    if (offset < buffer.length) {
                        buffer[offset] = b[i];
                        offset++;
                    }
                }
            }
        }

        @Override
        public void writeTo(OutputStream out) throws IOException {
            Log.d("farert", out.toString());
        }
    }
}