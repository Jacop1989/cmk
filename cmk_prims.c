/*
   cmk_prims.c — นิยามพรีมิติฟเรขาคณิตพื้นฐานสำหรับ CMK

   รวมฟังก์ชันสร้าง Mesh สำหรับ:
   - กล่อง (box)
   - ทรงกระบอก (cylinder) แกน Z
*/

#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "cmk_math.h"
#include "cmk_mesh.h"

/*------------------------ Box --------------------------------*/
/* สร้างกล่องแกนตรง (axis-aligned) ศูนย์กลางที่จุดกำเนิด ขนาด (sx,sy,sz) */
Mesh make_box(double sx,double sy,double sz){
    Mesh m; mesh_init(&m);
    double x=sx*0.5, y=sy*0.5, z=sz*0.5;
    /* 8 มุม */
    uint32_t v000=mesh_add_vertex(&m,v3(-x,-y,-z));
    uint32_t v100=mesh_add_vertex(&m,v3( x,-y,-z));
    uint32_t v110=mesh_add_vertex(&m,v3( x, y,-z));
    uint32_t v010=mesh_add_vertex(&m,v3(-x, y,-z));
    uint32_t v001=mesh_add_vertex(&m,v3(-x,-y, z));
    uint32_t v101=mesh_add_vertex(&m,v3( x,-y, z));
    uint32_t v111=mesh_add_vertex(&m,v3( x, y, z));
    uint32_t v011=mesh_add_vertex(&m,v3(-x, y, z));
    /* 12 สามเหลี่ยม (CCW มองจากด้านนอก) */
    /* -Z bottom */
    mesh_add_tri(&m, v000,v100,v110); mesh_add_tri(&m, v000,v110,v010);
    /* +Z top */
    mesh_add_tri(&m, v001,v111,v101); mesh_add_tri(&m, v001,v011,v111);
    /* -Y front */
    mesh_add_tri(&m, v000,v101,v100); mesh_add_tri(&m, v000,v001,v101);
    /* +Y back */
    mesh_add_tri(&m, v010,v110,v111); mesh_add_tri(&m, v010,v111,v011);
    /* -X left */
    mesh_add_tri(&m, v000,v010,v011); mesh_add_tri(&m, v000,v011,v001);
    /* +X right */
    mesh_add_tri(&m, v100,v101,v111); mesh_add_tri(&m, v100,v111,v110);
    return m;
}

/*------------------------ Cylinder ---------------------------*/
/* สร้างทรงกระบอกตามแกน Z ศูนย์กลางที่จุดกำเนิด */
Mesh make_cylinder(double radius,double height, int segments){
    if (segments<8) segments=8;
    Mesh m; mesh_init(&m);
    double hz = height*0.5;
    /* จุดยอดของวงล่าง/บน */
    uint32_t* bot = (uint32_t*)malloc(sizeof(uint32_t)*segments);
    uint32_t* top = (uint32_t*)malloc(sizeof(uint32_t)*segments);
    for(int i=0;i<segments;i++){
        double a = (2.0*M_PI*i)/segments;
        double cx = radius*cos(a), cy = radius*sin(a);
        bot[i]=mesh_add_vertex(&m, v3(cx,cy,-hz));
        top[i]=mesh_add_vertex(&m, v3(cx,cy, hz));
    }
    /* จุดศูนย์กลางสำหรับฝา */
    uint32_t cbot = mesh_add_vertex(&m, v3(0,0,-hz));
    uint32_t ctop = mesh_add_vertex(&m, v3(0,0, hz));
    /* ผิวข้าง (แปลงสี่เหลี่ยมเป็นสองสามเหลี่ยม) */
    for(int i=0;i<segments;i++){
        int j=(i+1)%segments;
        mesh_add_tri(&m, bot[i], bot[j], top[j]);
        mesh_add_tri(&m, bot[i], top[j], top[i]);
    }
    /* ฝาบน/ล่าง (fan) */
    for(int i=0;i<segments;i++){
        int j=(i+1)%segments;
        /* ล่าง: มองจากด้านนอก (-Z) จะเป็นทิศตามเข็มในระนาบ XY จึงสลับ */
        mesh_add_tri(&m, cbot, bot[j], bot[i]);
        /* บน: CCW ใน XY */
        mesh_add_tri(&m, ctop, top[i], top[j]);
    }
    free(bot); free(top);
    return m;
}

