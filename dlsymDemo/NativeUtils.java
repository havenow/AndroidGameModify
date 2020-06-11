package com.gameassist.plugin.nativeutils;

public class NativeUtils {
    static {
        System.loadLibrary("pgod");
    }

    public static native void init(int i);

    public static native void initNativePtr(int i, int i2);
    public static native int nativeProcessCheat(int i, int i2, int i3, int i4);

}
