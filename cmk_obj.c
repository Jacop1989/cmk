/*
   cmk_obj.c — นิยามฟังก์ชันเขียน Wavefront OBJ สำหรับ CMK

   รูปแบบที่ใช้:
     - v x y z    (จุดยอดแบบ float/double พิมพ์ด้วย %.9g เพื่อลดขนาดไฟล์)
     - f i j k    (ดัชนีเริ่มที่ 1 ตามสเปก OBJ)

   ไม่เขียน normal/material/uv ในเวอร์ชันนี้
*/

#include <stdio.h>
#include <string.h>
#include "cmk_math.h"
#include "cmk_mesh.h"
#include "cmk_obj.h"

int mesh_write_obj(const char* path, const Mesh* m){
    if (!m){ fprintf(stderr, "[OBJ] null mesh\n"); return 0; }
    if (m->tris.len % 3 != 0){
        fprintf(stderr, "[OBJ] Triangle index not multiple of 3\n");
        return 0;
    }
    FILE* f = fopen(path, "w");
    if (!f){ perror("fopen"); return 0; }

    /* ส่วนหัวคอมเมนต์ */
    fprintf(f, "# CMK (C Minimal Kernel) OBJ export\n");

    /* เขียนจุดยอดทั้งหมด */
    for (size_t i=0; i<m->verts.len; i++){
        vec3 p = m->verts.data[i];
        fprintf(f, "v %.9g %.9g %.9g\n", p.x, p.y, p.z);
    }

    /* เขียนหน้า (สามเหลี่ยม) โดย index เริ่มที่ 1 */
    for (size_t t=0; t<m->tris.len; t+=3){
        unsigned i0 = (unsigned)m->tris.data[t]   + 1;
        unsigned i1 = (unsigned)m->tris.data[t+1] + 1;
        unsigned i2 = (unsigned)m->tris.data[t+2] + 1;
        fprintf(f, "f %u %u %u\n", i0, i1, i2);
    }

    fclose(f);
    return 1;
}

