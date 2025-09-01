/*
   cmk_mesh.h — โครงสร้างและฟังก์ชันตาข่าย (Mesh) สำหรับ CMK

   เนื้อหา:
   - ประกาศโครงสร้าง Mesh และ dynamic array แบบย่อสำหรับจุดยอด/ดัชนี
   - ประกาศฟังก์ชันพื้นฐาน: init/free/add/transform/merge

   หมายเหตุ:
   - ใช้ร่วมกับ "cmk_math.h" เนื่องจาก Mesh อ้างถึงชนิด vec3 และ mat4
   - ฟิลด์ภายในเปิดเผยเพื่อความเรียบง่ายของซิงเกิลเคอร์เนล (เช่นผู้เขียนไฟล์ STL/OBJ)
*/

#ifndef CMK_MESH_H
#define CMK_MESH_H

#include <stddef.h>   /* size_t */
#include <stdint.h>   /* uint32_t */
#include "cmk_math.h" /* vec3, mat4 */

/* dynamic arrays แบบย่อ (สำหรับใช้งานภายใน/ภายนอกตามความจำเป็น) */
typedef struct { vec3*    data; size_t len, cap; } vec3v;
typedef struct { uint32_t* data; size_t len, cap; } u32v;

/* โครงสร้าง Mesh: เก็บจุดยอดไม่ซ้ำ และดัชนีสามเหลี่ยมแบบแบน */
typedef struct {
    vec3v verts;  /* อาร์เรย์จุดยอด */
    u32v  tris;   /* อาร์เรย์ดัชนี: [i0,i1,i2, i0,i1,i2, ...] */
} Mesh;

/* ฟังก์ชันพื้นฐานของ Mesh */
void     mesh_init(Mesh* m);
void     mesh_free(Mesh* m);
uint32_t mesh_add_vertex(Mesh* m, vec3 p);
void     mesh_add_tri(Mesh* m, uint32_t a, uint32_t b, uint32_t c);
void     mesh_transform(Mesh* m, mat4 T);
void     mesh_merge(Mesh* dst, const Mesh* src);

#endif /* CMK_MESH_H */

