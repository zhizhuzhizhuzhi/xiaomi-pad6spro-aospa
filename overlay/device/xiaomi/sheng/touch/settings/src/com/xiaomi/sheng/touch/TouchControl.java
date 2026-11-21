package com.xiaomi.sheng.touch;
import android.content.Context;
import android.os.UserHandle;
import android.provider.Settings;
import android.util.Log;
final class TouchControl {
 static {System.loadLibrary("shengtouchtoggle");}
 static final String KEY="touch_polling_rate";
 static native int[] command(int cmd,int mode,int value);
 static int value(Context c){return Settings.System.getIntForUser(c.getContentResolver(),KEY,0,UserHandle.USER_CURRENT)==1?1:0;}
 static void apply(Context c){int v=value(c);int[] r=command(10,0,v);if(r[0]<0||r[1]<0)throw new IllegalStateException("THP profile rejected ("+r[0]+", "+r[1]+")");Log.i("ShengTouchControl","THP stock game profile requested="+v);}
}
