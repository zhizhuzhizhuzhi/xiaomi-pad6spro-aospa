#include <jni.h>
void *memset(void *p,int value,unsigned long n){volatile unsigned char *q=p;for(unsigned long i=0;i<n;i++)q[i]=(unsigned char)value;return p;}
void *memcpy(void *p,const void *s,unsigned long n){volatile unsigned char *q=p;const volatile unsigned char *r=s;for(unsigned long i=0;i<n;i++)q[i]=r[i];return p;}
static long sys(long n,long a,long b,long c){register long x0 __asm__("x0")=a;register long x1 __asm__("x1")=b;register long x2 __asm__("x2")=c;register long x8 __asm__("x8")=n;__asm__ volatile("svc #0":"+r"(x0):"r"(x1),"r"(x2),"r"(x8):"memory","cc");return x0;}
// sheng stock xiaomi_touch.ko checks _IOC_SIZE == 0x208 for COMMON_DATA_CMD.
struct common_data { signed char touch_id; unsigned char cmd; unsigned short mode; unsigned short data_len; unsigned short pad; int data[128]; };
_Static_assert(sizeof(struct common_data)==520,"THP ABI mismatch");
JNIEXPORT jintArray JNICALL Java_com_xiaomi_sheng_touch_TouchControl_command(JNIEnv *env,jclass cls,jint cmd,jint mode,jint value){
 struct common_data buf={0};int out[8]={0};buf.cmd=cmd;buf.mode=mode;buf.data_len=1;buf.data[0]=value;
 if((cmd!=10&&cmd!=1&&cmd!=5)||(mode!=0&&mode!=1&&mode!=9)||(cmd==10&&(mode!=0||(value!=0&&value!=1)))){out[0]=-22;}
 else{long fd=sys(56,-100,(long)"/dev/xiaomi-touch",2);out[0]=fd;if(fd>=0){out[1]=sys(29,fd,0x40005403UL,0);if(out[1]>=0)out[1]=sys(29,fd,0xc2085400UL,(long)&buf);sys(57,fd,0,0);for(int i=0;i<4;i++)out[2+i]=buf.data[i];out[6]=buf.mode;out[7]=buf.cmd;}}
 jintArray a=(*env)->NewIntArray(env,8);(*env)->SetIntArrayRegion(env,a,0,8,out);return a;
}
