/*
   cmk_obj.h — ตัวเขียนไฟล์ Wavefront OBJ สำหรับ CMK

   ฟังก์ชันสำหรับบันทึก Mesh เป็นไฟล์ OBJ แบบข้อความ ด้วยรูปแบบ:
     - จุดยอด:  v x y z
     - หน้าสามเหลี่ยม: f i j k   (index เริ่มที่ 1 ตามสเปก OBJ)

   ไม่รองรับ normal/material/uv ในเวอร์ชันนี้เพื่อความเรียบง่าย
*/

#ifndef CMK_OBJ_H
#define CMK_OBJ_H

#include "cmk_mesh.h"  /* ใช้ชนิด Mesh */

/* เขียนไฟล์ OBJ; คืนค่า 1 เมื่อสำเร็จ, 0 เมื่อผิดพลาด */
int mesh_write_obj(const char* path, const Mesh* m);

#endif /* CMK_OBJ_H */

