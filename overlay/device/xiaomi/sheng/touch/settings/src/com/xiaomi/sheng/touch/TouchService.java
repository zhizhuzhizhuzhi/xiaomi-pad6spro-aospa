package com.xiaomi.sheng.touch;
import android.app.Service;
import android.content.Intent;
import android.database.ContentObserver;
import android.os.*;
import android.provider.Settings;
import android.util.Log;
public class TouchService extends Service {
 private Handler worker;private ContentObserver observer;private int applied=-1;
 private void apply(){int v=TouchControl.value(this);if(v==applied)return;try{TouchControl.apply(this);applied=v;}catch(RuntimeException e){Log.e("ShengTouchControl","THP apply failed",e);}}
 public void onCreate(){super.onCreate();HandlerThread thread=new HandlerThread("ShengTouchControl");thread.start();worker=new Handler(thread.getLooper());observer=new ContentObserver(worker){public void onChange(boolean self){apply();}};getContentResolver().registerContentObserver(Settings.System.getUriFor(TouchControl.KEY),false,observer,UserHandle.USER_ALL);worker.post(this::apply);}
 public int onStartCommand(Intent i,int flags,int id){worker.post(this::apply);return START_STICKY;}
 public void onDestroy(){getContentResolver().unregisterContentObserver(observer);worker.getLooper().quitSafely();super.onDestroy();}
 public IBinder onBind(Intent i){return null;}
}
