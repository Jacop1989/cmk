/*
   cmk_math.h — ส่วนคณิตศาสตร์สำหรับ CMK (C Minimal Kernel)

   เนื้อหา:
   - โครงสร้างเวกเตอร์ 3 มิติ (vec3) และเมทริกซ์ 4x4 แบบคอลัมน์เมเจอร์ (mat4)
   - ฟังก์ชันคณิตศาสตร์พื้นฐานสำหรับเวกเตอร์และเมทริกซ์

   การใช้งาน:
   - include ไฟล์นี้ในโมดูลที่ต้องใช้เรขาคณิตพื้นฐาน
   - ลิงก์กับ `cmk_math.c` ตอนคอมไพล์
*/

#ifndef CMK_MATH_H
#define CMK_MATH_H

/* นำเข้า math.h และกำหนดค่าคงที่ M_PI หากยังไม่มี (เช่นบน MSVC/GCC เก่า) */
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* โครงสร้างข้อมูลพื้นฐาน */
typedef struct { double x, y, z; } vec3;
typedef struct { double m[16]; } mat4; /* เมทริกซ์ 4x4 จัดเก็บแบบคอลัมน์เมเจอร์ */

/* เวกเตอร์ 3 มิติ */
vec3   v3(double x,double y,double z);
vec3   v3_add(vec3 a, vec3 b);
vec3   v3_sub(vec3 a, vec3 b);
vec3   v3_scale(vec3 a, double s);
double v3_dot(vec3 a, vec3 b);
vec3   v3_cross(vec3 a, vec3 b);
double v3_len(vec3 a);
vec3   v3_norm(vec3 a);

/* เมทริกซ์ 4x4 (แปลงแบบ affine/perspective) */
mat4 m4_identity(void);
mat4 m4_translate(double tx,double ty,double tz);
mat4 m4_scale(double sx,double sy,double sz);
mat4 m4_rotateZ(double rad);
mat4 m4_mul(mat4 A, mat4 B);
vec3 m4_mul_point(mat4 M, vec3 p);

#endif /* CMK_MATH_H */
