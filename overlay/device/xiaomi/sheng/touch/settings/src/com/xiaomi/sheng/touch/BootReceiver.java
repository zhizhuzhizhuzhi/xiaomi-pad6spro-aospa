package com.xiaomi.sheng.touch;
import android.content.*;
public class BootReceiver extends BroadcastReceiver {public void onReceive(Context c,Intent i){c.startService(new Intent(c,TouchService.class));}}
