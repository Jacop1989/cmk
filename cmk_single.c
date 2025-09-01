/* cmk_single.c  —  CMK (C Minimal Kernel) v0.1  —  Pure C, zero deps
   Build:
     Linux/Mac:   gcc -O2 cmk_single.c -o cmk_demo -lm
     Windows MSVC: cl /O2 cmk_single.c /Fe:cmk_demo.exe
*/

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "cmk_math.h"
#include "cmk_mesh.h"
#include "cmk_prims.h"

/*------------------------ Basic types ------------------------*/
typedef uint32_t u32;

/*------------------------ Math utils -------------------------*/
/* ย้ายไปอยู่ใน cmk_math.{h,c} แล้ว - ส่วนนี้คงเหลือไว้เพื่อรวมผ่าน header เท่านั้น */

/*------------------------ Mesh utils -------------------------*/
/* ย้ายไปอยู่ใน cmk_mesh.{h,c} แล้ว - รวมผ่าน header เท่านั้น */

/*------------------------ Primitives -------------------------*/
/* ย้ายไปอยู่ใน cmk_prims.{h,c} แล้ว - รวมผ่าน header เท่านั้น */

/*------------------------ STL (binary) writer ----------------*/
static void write_f32(FILE* f, float v){ fwrite(&v,4,1,f); }
static void write_u16(FILE* f, uint16_t v){ fwrite(&v,2,1,f); }
static int mesh_write_stl_binary(const char* path, const Mesh* m){
    if (m->tris.len%3!=0){ fprintf(stderr,"[STL] Triangle index not multiple of 3\n"); return 0; }
    FILE* f=fopen(path,"wb"); if(!f){ perror("fopen"); return 0; }
    /* 80-byte header */
    char hdr[80]; memset(hdr,0,sizeof(hdr)); snprintf(hdr,80,"CMK v0.1");
    fwrite(hdr,1,80,f);
    uint32_t tri_count=(uint32_t)(m->tris.len/3);
    fwrite(&tri_count,4,1,f);
    for(size_t t=0;t<m->tris.len;t+=3){
        u32 i0=m->tris.data[t], i1=m->tris.data[t+1], i2=m->tris.data[t+2];
        vec3 a=m->verts.data[i0], b=m->verts.data[i1], c=m->verts.data[i2];
        vec3 n=v3_norm(v3_cross(v3_sub(b,a), v3_sub(c,a)));
        /* normal then vertices as float32, then attr (0) */
        write_f32(f,(float)n.x); write_f32(f,(float)n.y); write_f32(f,(float)n.z);
        write_f32(f,(float)a.x); write_f32(f,(float)a.y); write_f32(f,(float)a.z);
        write_f32(f,(float)b.x); write_f32(f,(float)b.y); write_f32(f,(float)b.z);
        write_f32(f,(float)c.x); write_f32(f,(float)c.y); write_f32(f,(float)c.z);
        write_u16(f,0);
    }
    fclose(f);
    return 1;
}

/*------------------------ OBJ (text) writer ------------------*/
/*
   mesh_write_obj(path, m)
   - บันทึก Mesh เป็นไฟล์ Wavefront OBJ (ข้อความ)
   - เขียนจุดยอดด้วยรูปแบบ:  v x y z
   - เขียนหน้า (สามเหลี่ยม) ด้วยรูปแบบ: f i j k (index เริ่มที่ 1 ตามสเปก OBJ)
   - ไม่เขียน normal/material/uv เพื่อลดความซับซ้อน (สามารถเพิ่มได้ในอนาคต)
*/
static int mesh_write_obj(const char* path, const Mesh* m){
    if (!m){ fprintf(stderr, "[OBJ] null mesh\n"); return 0; }
    if (m->tris.len % 3 != 0){
        fprintf(stderr, "[OBJ] Triangle index not multiple of 3\n");
        return 0;
    }
    FILE* f = fopen(path, "w");
    if (!f){ perror("fopen"); return 0; }

    /* ส่วนหัวคอมเมนต์เพื่อบอกที่มา */
    fprintf(f, "# CMK (C Minimal Kernel) v0.1 OBJ export\n");

    /* เขียนจุดยอดทั้งหมด (ใช้ความแม่นยำแบบ %.9g เพื่อให้ไฟล์กระทัดรัด) */
    for (size_t i=0; i<m->verts.len; i++){
        vec3 p = m->verts.data[i];
        fprintf(f, "v %.9g %.9g %.9g\n", p.x, p.y, p.z);
    }

    /* เขียนหน้าเป็นสามเหลี่ยม โดย index ใน OBJ เริ่มที่ 1 */
    for (size_t t=0; t<m->tris.len; t+=3){
        u32 i0 = m->tris.data[t]   + 1;
        u32 i1 = m->tris.data[t+1] + 1;
        u32 i2 = m->tris.data[t+2] + 1;
        fprintf(f, "f %u %u %u\n", i0, i1, i2);
    }

    fclose(f);
    return 1;
}

/*------------------------ Demo / smoke test ------------------*/
static void demo_generate(){
    /* Plate 200x150x10 @ origin */
    Mesh plate = make_box(200.0,150.0,10.0);

    /* Four cylinders (M6-ish hole Ø6, h=12) — we only export them separately for now.
       Boolean subtract will come in next milestones. */
    Mesh holes; mesh_init(&holes);
    double ox = 200.0*0.5 - 20.0; /* corner offset X */
    double oy = 150.0*0.5 - 20.0; /* corner offset Y */
    double r  = 3.0;              /* radius */
    for(int sx=-1; sx<=1; sx+=2){
        for(int sy=-1; sy<=1; sy+=2){
            Mesh c = make_cylinder(r, 12.0, 32);
            mesh_transform(&c, m4_translate(sx*ox, sy*oy, 0.0));
            mesh_merge(&holes, &c);
            mesh_free(&c);
        }
    }

    /* Save two STL files */
    mesh_write_stl_binary("plate_only.stl", &plate);
    mesh_write_stl_binary("holes_only.stl", &holes);

    mesh_free(&holes);
    mesh_free(&plate);
}

int main(void){
    demo_generate();
    printf("Generated: plate_only.stl, holes_only.stl\n");
    printf("Note: No booleans yet — holes exported as separate mesh.\n");
    return 0;
}
