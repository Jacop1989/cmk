/*
   cmk_stl.c — นิยามฟังก์ชันเขียน STL (Binary) สำหรับ CMK

   รูปแบบไฟล์ STL ไบนารีประกอบด้วย:
   - ส่วนหัว 80 ไบต์ (ไม่บังคับความหมาย)
   - จำนวนสามเหลี่ยม (uint32)
   - ชุดของสามเหลี่ยม: [normal(3*f32), v0(3*f32), v1(3*f32), v2(3*f32), attr(u16)]

   หมายเหตุ:
   - คำนวณ normal จาก cross(b-a, c-a) และ normalize แบบง่าย
   - เขียนเป็น float32 เพื่อให้สอดคล้องสเปก STL
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cmk_math.h"
#include "cmk_mesh.h"
#include "cmk_stl.h"

/* เขียนค่าประเภทพื้นฐานลงไฟล์แบบไบนารี (ตัวช่วยภายใน) */
static void write_f32(FILE* f, float v){ fwrite(&v,4,1,f); }
static void write_u16(FILE* f, uint16_t v){ fwrite(&v,2,1,f); }

int mesh_write_stl_binary(const char* path, const Mesh* m){
    if (!m){ fprintf(stderr, "[STL] null mesh\n"); return 0; }
    if (m->tris.len%3!=0){
        fprintf(stderr,"[STL] Triangle index not multiple of 3\n");
        return 0;
    }
    FILE* f=fopen(path,"wb");
    if(!f){ perror("fopen"); return 0; }

    /* 80-byte header (เติมข้อความระบุเวอร์ชันสั้นๆ) */
    char hdr[80]; memset(hdr,0,sizeof(hdr)); snprintf(hdr,80,"CMK v0.1");
    fwrite(hdr,1,80,f);

    uint32_t tri_count=(uint32_t)(m->tris.len/3);
    fwrite(&tri_count,4,1,f);

    for(size_t t=0;t<m->tris.len;t+=3){
        uint32_t i0=(uint32_t)m->tris.data[t];
        uint32_t i1=(uint32_t)m->tris.data[t+1];
        uint32_t i2=(uint32_t)m->tris.data[t+2];
        vec3 a=m->verts.data[i0], b=m->verts.data[i1], c=m->verts.data[i2];
        vec3 n=v3_norm(v3_cross(v3_sub(b,a), v3_sub(c,a)));
        /* normal แล้วตามด้วยจุดยอดทั้งสาม จากนั้น attribute (0) */
        write_f32(f,(float)n.x); write_f32(f,(float)n.y); write_f32(f,(float)n.z);
        write_f32(f,(float)a.x); write_f32(f,(float)a.y); write_f32(f,(float)a.z);
        write_f32(f,(float)b.x); write_f32(f,(float)b.y); write_f32(f,(float)b.z);
        write_f32(f,(float)c.x); write_f32(f,(float)c.y); write_f32(f,(float)c.z);
        write_u16(f,0);
    }

    fclose(f);
    return 1;
}

