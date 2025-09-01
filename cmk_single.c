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

/*------------------------ Basic types ------------------------*/
typedef uint32_t u32;
typedef struct { double x, y, z; } vec3;
typedef struct { double m[16]; } mat4; /* column-major 4x4 */

/*------------------------ Math utils -------------------------*/
static inline vec3 v3(double x,double y,double z){ vec3 a={x,y,z}; return a; }
static inline vec3 v3_add(vec3 a, vec3 b){ return v3(a.x+b.x, a.y+b.y, a.z+b.z); }
static inline vec3 v3_sub(vec3 a, vec3 b){ return v3(a.x-b.x, a.y-b.y, a.z-b.z); }
static inline vec3 v3_scale(vec3 a, double s){ return v3(a.x*s, a.y*s, a.z*s); }
static inline double v3_dot(vec3 a, vec3 b){ return a.x*b.x + a.y*b.y + a.z*b.z; }
static inline vec3 v3_cross(vec3 a, vec3 b){
    return v3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}
static inline double v3_len(vec3 a){ return sqrt(v3_dot(a,a)); }
static inline vec3 v3_norm(vec3 a){ double L=v3_len(a); return (L>0)? v3_scale(a,1.0/L) : a; }

static inline mat4 m4_identity(void){
    mat4 M={{1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1}}; return M;
}
static inline mat4 m4_translate(double tx,double ty,double tz){
    mat4 M=m4_identity(); M.m[12]=tx; M.m[13]=ty; M.m[14]=tz; return M;
}
static inline mat4 m4_scale(double sx,double sy,double sz){
    mat4 M=m4_identity(); M.m[0]=sx; M.m[5]=sy; M.m[10]=sz; return M;
}
static inline mat4 m4_rotateZ(double rad){
    mat4 M=m4_identity();
    double c=cos(rad), s=sin(rad);
    M.m[0]=c; M.m[4]=-s; M.m[1]=s; M.m[5]=c; return M;
}
static inline mat4 m4_mul(mat4 A, mat4 B){
    mat4 R={{0}};
    for(int c=0;c<4;c++) for(int r=0;r<4;r++){
        double s=0; for(int k=0;k<4;k++) s += A.m[k*4+r]*B.m[c*4+k];
        R.m[c*4+r]=s;
    }
    return R;
}
static inline vec3 m4_mul_point(mat4 M, vec3 p){
    double x=M.m[0]*p.x + M.m[4]*p.y + M.m[8] *p.z + M.m[12];
    double y=M.m[1]*p.x + M.m[5]*p.y + M.m[9] *p.z + M.m[13];
    double z=M.m[2]*p.x + M.m[6]*p.y + M.m[10]*p.z + M.m[14];
    double w=M.m[3]*p.x + M.m[7]*p.y + M.m[11]*p.z + M.m[15];
    if (w!=0.0){ x/=w; y/=w; z/=w; } return v3(x,y,z);
}

/*------------------------ Tiny dynamic arrays ----------------*/
typedef struct { vec3* data; size_t len, cap; } vec3v;
typedef struct { u32*  data; size_t len, cap; } u32v;

static void vec3v_free(vec3v* a){ free(a->data); a->data=NULL; a->len=a->cap=0; }
static void u32v_free(u32v* a){ free(a->data); a->data=NULL; a->len=a->cap=0; }

static void* _reserve(void* p,size_t* cap,size_t elemsz,size_t need){
    size_t c=*cap; if (need<=c) return p;
    size_t nc = c? c*2: 64; if (nc<need) nc=need;
    void* np = realloc(p, nc*elemsz); if(!np){ fprintf(stderr,"OOM\n"); exit(1); }
    *cap=nc; return np;
}
static void vec3v_push(vec3v* a, vec3 v){
    a->data = (vec3*)_reserve(a->data,&a->cap,sizeof(vec3),a->len+1);
    a->data[a->len++]=v;
}
static void u32v_push(u32v* a, u32 v){
    a->data = (u32*) _reserve(a->data,&a->cap,sizeof(u32), a->len+1);
    a->data[a->len++]=v;
}

/*------------------------ Mesh structure ---------------------*/
typedef struct {
    vec3v verts;   /* array of unique vertices */
    u32v  tris;    /* flat list of indices: [i0,i1,i2, i0,i1,i2, ...] */
} Mesh;

static void mesh_init(Mesh* m){ memset(m,0,sizeof(*m)); }
static void mesh_free(Mesh* m){ vec3v_free(&m->verts); u32v_free(&m->tris); }

static u32  mesh_add_vertex(Mesh* m, vec3 p){ vec3v_push(&m->verts,p); return (u32)(m->verts.len-1); }
static void mesh_add_tri(Mesh* m, u32 a,u32 b,u32 c){
    u32v_push(&m->tris,a); u32v_push(&m->tris,b); u32v_push(&m->tris,c);
}
static void mesh_transform(Mesh* m, mat4 T){
    for(size_t i=0;i<m->verts.len;i++) m->verts.data[i] = m4_mul_point(T, m->verts.data[i]);
}
static void mesh_merge(Mesh* dst, const Mesh* src){
    u32 base = (u32)dst->verts.len;
    for(size_t i=0;i<src->verts.len;i++) vec3v_push(&dst->verts, src->verts.data[i]);
    for(size_t i=0;i<src->tris.len;i++)   u32v_push(&dst->tris, src->tris.data[i]+base);
}

/*------------------------ Primitives -------------------------*/
/* Create axis-aligned box centered at origin with size (sx,sy,sz) */
static Mesh make_box(double sx,double sy,double sz){
    Mesh m; mesh_init(&m);
    double x=sx*0.5, y=sy*0.5, z=sz*0.5;
    /* 8 corners */
    u32 v000=mesh_add_vertex(&m,v3(-x,-y,-z));
    u32 v100=mesh_add_vertex(&m,v3( x,-y,-z));
    u32 v110=mesh_add_vertex(&m,v3( x, y,-z));
    u32 v010=mesh_add_vertex(&m,v3(-x, y,-z));
    u32 v001=mesh_add_vertex(&m,v3(-x,-y, z));
    u32 v101=mesh_add_vertex(&m,v3( x,-y, z));
    u32 v111=mesh_add_vertex(&m,v3( x, y, z));
    u32 v011=mesh_add_vertex(&m,v3(-x, y, z));
    /* 12 triangles (CCW outward) */
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

/* Approx cylinder along Z axis, centered at origin */
static Mesh make_cylinder(double radius,double height, int segments){
    if (segments<8) segments=8;
    Mesh m; mesh_init(&m);
    double hz = height*0.5;
    /* ring vertices */
    u32* bot = (u32*)malloc(sizeof(u32)*segments);
    u32* top = (u32*)malloc(sizeof(u32)*segments);
    for(int i=0;i<segments;i++){
        double a = (2.0*M_PI*i)/segments;
        double cx = radius*cos(a), cy = radius*sin(a);
        bot[i]=mesh_add_vertex(&m, v3(cx,cy,-hz));
        top[i]=mesh_add_vertex(&m, v3(cx,cy, hz));
    }
    /* centers for caps */
    u32 cbot = mesh_add_vertex(&m, v3(0,0,-hz));
    u32 ctop = mesh_add_vertex(&m, v3(0,0, hz));
    /* sides */
    for(int i=0;i<segments;i++){
        int j=(i+1)%segments;
        /* quad (bot[i], bot[j], top[j], top[i]) -> two tris, CCW outward */
        mesh_add_tri(&m, bot[i], bot[j], top[j]);
        mesh_add_tri(&m, bot[i], top[j], top[i]);
    }
    /* caps (fans) */
    for(int i=0;i<segments;i++){
        int j=(i+1)%segments;
        /* bottom: CCW seen from outside (-Z cap means clockwise in XY, so flip) */
        mesh_add_tri(&m, cbot, bot[j], bot[i]);
        /* top: CCW in XY */
        mesh_add_tri(&m, ctop, top[i], top[j]);
    }
    free(bot); free(top);
    return m;
}

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
