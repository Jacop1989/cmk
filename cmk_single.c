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
#include "cmk_stl.h"
#include "cmk_obj.h"

/*------------------------ Basic types ------------------------*/
typedef uint32_t u32;

/*------------------------ Math utils -------------------------*/
/* ย้ายไปอยู่ใน cmk_math.{h,c} แล้ว - ส่วนนี้คงเหลือไว้เพื่อรวมผ่าน header เท่านั้น */

/*------------------------ Mesh utils -------------------------*/
/* ย้ายไปอยู่ใน cmk_mesh.{h,c} แล้ว - รวมผ่าน header เท่านั้น */

/*------------------------ Primitives -------------------------*/
/* ย้ายไปอยู่ใน cmk_prims.{h,c} แล้ว - รวมผ่าน header เท่านั้น */

/*------------------------ STL writer -------------------------*/
/* ย้ายไปอยู่ใน cmk_stl.{h,c} แล้ว - รวมผ่าน header เท่านั้น */

/*------------------------ OBJ writer -------------------------*/
/* ย้ายไปอยู่ใน cmk_obj.{h,c} แล้ว - รวมผ่าน header เท่านั้น */

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
