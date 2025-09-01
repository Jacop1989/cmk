/*
   cmk_test.c — Unit Test สำหรับโปรเจกต์ CMK (C Minimal Kernel)

   ทดสอบครอบคลุม:
   - make_box(): ตรวจจำนวนจุดยอด (vertex) และจำนวนสามเหลี่ยม
   - make_cylinder(): ตรวจจำนวนจุดยอดสัมพันธ์กับค่า segments
   - mesh_write_stl_binary(): ตรวจว่าไฟล์ STL ถูกสร้างและมีขนาดมากกว่า header เปล่า

   วิธีคอมไพล์ (ตัวอย่าง GCC/Clang):
     gcc -O2 -std=c11 cmk_test.c \
         cmk_math.c cmk_mesh.c cmk_prims.c cmk_stl.c \
         -o cmk_test -lm

   หมายเหตุ: บน MSVC ควรกำหนด /D_USE_MATH_DEFINES หากใช้ค่าคงที่จาก math.h
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmk_math.h"
#include "cmk_mesh.h"
#include "cmk_prims.h"
#include "cmk_stl.h"

static long file_size(const char* path){
    FILE* f = fopen(path, "rb");
    if(!f) return -1;
    if (fseek(f, 0, SEEK_END) != 0){ fclose(f); return -1; }
    long n = ftell(f);
    fclose(f);
    return n;
}

static void test_make_box(void){
    /* ทดสอบการสร้างกล่อง: ควรได้ 8 จุดยอด และ 12 สามเหลี่ยม (ดัชนี = 36) */
    Mesh m = make_box(200.0, 150.0, 10.0);
    assert(m.verts.len == 8);
    assert(m.tris.len  == 36);
    mesh_free(&m);
}

static void test_make_cylinder(void){
    /* ทดสอบทรงกระบอก: จุดยอด = 2*segments + 2, ดัชนี = 12*segments */
    int segments = 16;
    Mesh c = make_cylinder(3.0, 12.0, segments);
    assert(c.verts.len == (size_t)(2*segments + 2));
    assert(c.tris.len  == (size_t)(12*segments));
    mesh_free(&c);
}

static void test_stl_export(void){
    /* ทดสอบเขียน STL: ต้องสร้างไฟล์และมีขนาด > 84 ไบต์ (80 header + 4 tri-count) */
    const char* path = "cmk_test_box.stl";
    remove(path); /* ลบไฟล์เก่าหากมี */

    Mesh m = make_box(10.0, 10.0, 10.0);
    int ok = mesh_write_stl_binary(path, &m);
    assert(ok == 1);

    long sz = file_size(path);
    assert(sz > 84);

    mesh_free(&m);
    remove(path); /* ทำความสะอาดไฟล์ทดสอบ */
}

int main(void){
    test_make_box();
    test_make_cylinder();
    test_stl_export();
    printf("All CMK unit tests passed.\n");
    return 0;
}

