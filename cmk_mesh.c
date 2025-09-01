/*
   cmk_mesh.c — นิยามโครงสร้างและฟังก์ชัน Mesh สำหรับ CMK

   รวม dynamic array แบบย่อสำหรับ vec3 และ uint32_t
   และฟังก์ชันพื้นฐานของ Mesh: init/free/add/transform/merge
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cmk_mesh.h"

/*------------------------ dynamic arrays ---------------------*/
static void vec3v_free(vec3v* a){ free(a->data); a->data=NULL; a->len=a->cap=0; }
static void u32v_free(u32v* a){ free(a->data); a->data=NULL; a->len=a->cap=0; }

static void* _reserve(void* p,size_t* cap,size_t elemsz,size_t need){
    size_t c=*cap; if (need<=c) return p;
    size_t nc = c? c*2: 64; if (nc<need) nc=need;
    void* np = realloc(p, nc*elemsz);
    if(!np){ fprintf(stderr,"OOM\n"); exit(1); }
    *cap=nc; return np;
}
static void vec3v_push(vec3v* a, vec3 v){
    a->data = (vec3*)_reserve(a->data,&a->cap,sizeof(vec3),a->len+1);
    a->data[a->len++]=v;
}
static void u32v_push(u32v* a, uint32_t v){
    a->data = (uint32_t*) _reserve(a->data,&a->cap,sizeof(uint32_t), a->len+1);
    a->data[a->len++]=v;
}

/*------------------------ Mesh API ---------------------------*/
void mesh_init(Mesh* m){ memset(m,0,sizeof(*m)); }
void mesh_free(Mesh* m){ vec3v_free(&m->verts); u32v_free(&m->tris); }

uint32_t mesh_add_vertex(Mesh* m, vec3 p){ vec3v_push(&m->verts,p); return (uint32_t)(m->verts.len-1); }
void     mesh_add_tri(Mesh* m, uint32_t a,uint32_t b,uint32_t c){
    u32v_push(&m->tris,a); u32v_push(&m->tris,b); u32v_push(&m->tris,c);
}
void     mesh_transform(Mesh* m, mat4 T){
    for(size_t i=0;i<m->verts.len;i++) m->verts.data[i] = m4_mul_point(T, m->verts.data[i]);
}
void     mesh_merge(Mesh* dst, const Mesh* src){
    uint32_t base = (uint32_t)dst->verts.len;
    for(size_t i=0;i<src->verts.len;i++) vec3v_push(&dst->verts, src->verts.data[i]);
    for(size_t i=0;i<src->tris.len;i++)   u32v_push(&dst->tris, src->tris.data[i]+base);
}

