/*
   cmk_math.c — นิยามฟังก์ชันคณิตศาสตร์สำหรับ CMK (C Minimal Kernel)

   รวมฟังก์ชันเวกเตอร์ 3 มิติ และเมทริกซ์ 4x4 สำหรับงานเรขาคณิตพื้นฐาน
   หมายเหตุ: เมทริกซ์จัดเก็บแบบคอลัมน์เมเจอร์ (column-major) ตามธรรมเนียมกราฟิกส์
*/

#include <math.h>
#include "cmk_math.h"

/*------------------------ Vec3 -------------------------------*/
vec3 v3(double x,double y,double z){ vec3 a={x,y,z}; return a; }
vec3 v3_add(vec3 a, vec3 b){ return v3(a.x+b.x, a.y+b.y, a.z+b.z); }
vec3 v3_sub(vec3 a, vec3 b){ return v3(a.x-b.x, a.y-b.y, a.z-b.z); }
vec3 v3_scale(vec3 a, double s){ return v3(a.x*s, a.y*s, a.z*s); }
double v3_dot(vec3 a, vec3 b){ return a.x*b.x + a.y*b.y + a.z*b.z; }
vec3 v3_cross(vec3 a, vec3 b){
    return v3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}
double v3_len(vec3 a){ return sqrt(v3_dot(a,a)); }
vec3 v3_norm(vec3 a){ double L=v3_len(a); return (L>0)? v3_scale(a,1.0/L) : a; }

/*------------------------ Mat4 -------------------------------*/
mat4 m4_identity(void){
    mat4 M={{1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1}}; return M;
}
mat4 m4_translate(double tx,double ty,double tz){
    mat4 M=m4_identity(); M.m[12]=tx; M.m[13]=ty; M.m[14]=tz; return M;
}
mat4 m4_scale(double sx,double sy,double sz){
    mat4 M=m4_identity(); M.m[0]=sx; M.m[5]=sy; M.m[10]=sz; return M;
}
mat4 m4_rotateZ(double rad){
    mat4 M=m4_identity();
    double c=cos(rad), s=sin(rad);
    M.m[0]=c; M.m[4]=-s; M.m[1]=s; M.m[5]=c; return M;
}
mat4 m4_mul(mat4 A, mat4 B){
    mat4 R={{0}};
    for(int c=0;c<4;c++) for(int r=0;r<4;r++){
        double s=0; for(int k=0;k<4;k++) s += A.m[k*4+r]*B.m[c*4+k];
        R.m[c*4+r]=s;
    }
    return R;
}
vec3 m4_mul_point(mat4 M, vec3 p){
    double x=M.m[0]*p.x + M.m[4]*p.y + M.m[8] *p.z + M.m[12];
    double y=M.m[1]*p.x + M.m[5]*p.y + M.m[9] *p.z + M.m[13];
    double z=M.m[2]*p.x + M.m[6]*p.y + M.m[10]*p.z + M.m[14];
    double w=M.m[3]*p.x + M.m[7]*p.y + M.m[11]*p.z + M.m[15];
    if (w!=0.0){ x/=w; y/=w; z/=w; }
    return v3(x,y,z);
}

