/*
   test_cmk.c — Unit Test สำหรับโปรเจกต์ CMK (C Minimal Kernel)

   เนื้อหา:
   - ทดสอบ make_box(): ตรวจจำนวนจุดยอด (vertex) และจำนวนสามเหลี่ยม
   - ทดสอบ make_cylinder(): ตรวจจำนวนจุดยอดสัมพันธ์กับจำนวน segments
   - ทดสอบ STL export: ตรวจว่าไฟล์ถูกสร้างและมีขนาดมากกว่า header เปล่า

   หมายเหตุทางเทคนิค:
   - ไฟล์นี้ include ซอร์สหลักแบบตรงๆ และใช้เทคนิคแมโครรีเนม `main` ของเดโม
     เพื่อหลีกเลี่ยงการซ้ำซ้อนของสัญลักษณ์ `main` ระหว่างเดโมกับเทสต์
   - หากคอมไพล์ด้วย MSVC ให้กำหนด `/D_USE_MATH_DEFINES` ตาม README เพื่อรองรับ M_PI

   วิธีคอมไพล์ (ตัวอย่าง):
     gcc -O2 -std=c11 test_cmk.c cmk_math.c cmk_mesh.c -o test_cmk -lm
     ./test_cmk
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* รีเนม main ของเดโมก่อน include เพื่อไม่ให้ชนกับ main ของเทสต์ */
#define main cmk_demo_main_renamed
#include "cmk_single.c"
#undef main

static long file_size(const char* path){
    FILE* f = fopen(path, "rb");
    if(!f) return -1;
    if (fseek(f, 0, SEEK_END) != 0){ fclose(f); return -1; }
    long n = ftell(f);
    fclose(f);
    return n;
}

static void test_make_box(void){
    Mesh m = make_box(200.0, 150.0, 10.0);
    /* กล่องควรมีจุดยอด 8 จุด และสามเหลี่ยม 12 หน้า (ดัชนี = 36) */
    assert(m.verts.len == 8);
    assert(m.tris.len == 36);
    mesh_free(&m);
}

static void test_make_cylinder(void){
    int segments = 16;
    Mesh c = make_cylinder(3.0, 12.0, segments);
    /* ทรงกระบอกนี้สร้างจุดยอดเป็นวงล่าง+บน อย่างละ segments และจุดศูนย์กลาง 2 จุด
       ดังนั้นจำนวนจุดยอด = 2*segments + 2
       จำนวนสามเหลี่ยมทั้งหมด = ด้านข้าง 2*segments + ฝาบนล่าง 2*segments = 4*segments
       (จำนวนดัชนี = 3 * จำนวนสามเหลี่ยม) */
    assert(c.verts.len == (size_t)(2*segments + 2));
    assert(c.tris.len == (size_t)(12*segments));
    mesh_free(&c);
}

static void test_stl_export(void){
    const char* path = "test_box.stl";
    remove(path); /* ลบของเก่า (ถ้ามี) เพื่อให้ตรวจผลได้ชัดเจน */

    Mesh m = make_box(10.0, 10.0, 10.0);
    int ok = mesh_write_stl_binary(path, &m);
    assert(ok == 1);

    long sz = file_size(path);
    /* ไฟล์ STL แบบไบนารีต้องมีขนาดมากกว่า header 84 ไบต์ (80 + 4 for tri-count) */
    assert(sz > 84);

    mesh_free(&m);
    remove(path); /* ทำความสะอาดไฟล์ทดสอบ */
}

static void test_obj_export(void){
    const char* path = "test_box.obj";
    remove(path);

    Mesh m = make_box(10.0, 10.0, 10.0);
    int ok = mesh_write_obj(path, &m);
    assert(ok == 1);

    long sz = file_size(path);
    assert(sz > 0);

    /* นับจำนวนบรรทัดขึ้นต้นด้วย 'v ' และ 'f ' ให้ตรงกับเมชของกล่อง */
    int vcnt = 0, fcnt = 0;
    FILE* f = fopen(path, "r");
    assert(f != NULL);
    char buf[512];
    while (fgets(buf, sizeof(buf), f)){
        if (strncmp(buf, "v ", 2) == 0) vcnt++;
        else if (strncmp(buf, "f ", 2) == 0) fcnt++;
    }
    fclose(f);

    assert(vcnt == 8);   /* กล่องมีจุดยอด 8 จุด */
    assert(fcnt == 12);  /* กล่องมีสามเหลี่ยม 12 หน้า */

    mesh_free(&m);
    remove(path);
}

int main(void){
    test_make_box();
    test_make_cylinder();
    test_stl_export();
    test_obj_export();
    printf("All CMK tests passed.\n");
    return 0;
}


