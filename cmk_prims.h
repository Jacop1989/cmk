/*
   cmk_prims.h — พรีมิติฟเรขาคณิตพื้นฐานสำหรับ CMK

   เนื้อหา:
   - สร้างตาข่ายทรงกล่อง (axis-aligned box)
   - สร้างตาข่ายทรงกระบอก (cylinder) แกน Z

   การใช้งาน:
   - include ไฟล์นี้ แล้วลิงก์ร่วมกับ cmk_prims.c
*/

#ifndef CMK_PRIMS_H
#define CMK_PRIMS_H

#include "cmk_mesh.h" /* ประกาศ Mesh */

/* สร้างกล่องขนาด (sx,sy,sz) ศูนย์กลางที่จุดกำเนิด */
Mesh make_box(double sx,double sy,double sz);

/* สร้างทรงกระบอกรัศมี radius สูง height แกน Z (ศูนย์กลางที่จุดกำเนิด)
   segments คือจำนวนส่วนแบ่งรอบวง (อย่างน้อย 8) */
Mesh make_cylinder(double radius,double height, int segments);

#endif /* CMK_PRIMS_H */

