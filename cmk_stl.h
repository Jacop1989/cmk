/*
   cmk_stl.h — ตัวเขียนไฟล์ STL (Binary) สำหรับ CMK

   ใช้สำหรับบันทึก Mesh เป็นไฟล์ STL แบบไบนารีมาตรฐาน
   ฟังก์ชันจะคำนวณ normal แบบง่ายจากสามจุดของสามเหลี่ยม และเขียนเป็น float32
*/

#ifndef CMK_STL_H
#define CMK_STL_H

#include "cmk_mesh.h"  /* ต้องใช้ชนิด Mesh */

/*
   mesh_write_stl_binary(path, m)
   - เขียนไฟล์ STL แบบไบนารีไปยังพาธที่กำหนด
   - คืนค่า 1 เมื่อสำเร็จ, 0 เมื่อเกิดข้อผิดพลาด (จะพิมพ์ข้อความไป stderr)
*/
int mesh_write_stl_binary(const char* path, const Mesh* m);

#endif /* CMK_STL_H */

