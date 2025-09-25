#include<Python.h>
#include<numpy/arrayobject.h>
#include<bqnffi.h>

#define R return
#define C(i,a) case i:{a;}break;
#define _ static inline
#define K const

typedef PyArrayObject* NPA;typedef PyObject* PO;typedef void* U;typedef double F;typedef size_t S;

#define PyE(str) PyErr_SetString(PyExc_RuntimeError, str);R 0;
#define D(str) default: PyE(str);
#define IX(i,n,a) {int i;for(i=0;i<n;i++){a;}}
#define $(p,a,b)if(p){a;}else{b;}

_ BQNV bqn_npy(K PO o) {
    $(PyFloat_Check(o),F x=PyFloat_AsDouble(o);R bqn_makeF64(x),);
    $(PyLong_Check(o),PyE("Integer arguments are not supported."),);
    if(PyUnicode_Check(o)){
        S l=PyUnicode_GET_LENGTH(o);
        int k=PyUnicode_KIND(o);
        BQNV res;
        switch(k){
            C(PyUnicode_1BYTE_KIND,uint8_t* s8;s8=PyUnicode_1BYTE_DATA(o);res=bqn_makeC8Vec(l,s8);)
            C(PyUnicode_2BYTE_KIND,uint16_t* s16;s16=PyUnicode_2BYTE_DATA(o);res=bqn_makeC16Vec(l,s16);)
            C(PyUnicode_4BYTE_KIND,uint32_t* s32;s32=PyUnicode_4BYTE_DATA(o);res=bqn_makeC32Vec(l,s32);)
            D("???")
        };
        R res;
    }
    NPA a=(NPA)o;
    int t=PyArray_TYPE(a);
    int rnk=PyArray_NDIM(a);
    S srnk=(S)rnk;
    npy_intp* dims=PyArray_DIMS(a);
    S* bqndims=malloc(sizeof(S)*rnk);
    IX(i,rnk,bqndims[i]=(S)dims[i]);free(dims);
    U data=PyArray_DATA(a);
    BQNV res;
    switch(t) {
        C(NPY_BYTE,res=bqn_makeI8Arr(srnk,bqndims,data))
        C(NPY_SHORT,res=bqn_makeI16Arr(srnk,bqndims,data))
        C(NPY_INT,res=bqn_makeI32Arr(srnk,bqndims,data))
        C(NPY_DOUBLE,res=bqn_makeF64Arr(srnk,bqndims,data))
        D("Type not supported. 🤷")
    }
    free(bqndims);
    R res;
}

_ PO npy_bqn(K BQNV x) {
    if(bqn_type(x)==1){
        R PyFloat_FromDouble(bqn_toF64(x));
    }
    S rnk=bqn_rank(x);
    npy_intp* dims=malloc(sizeof(npy_intp)*rnk);
    S* bqndims=malloc(sizeof(S)*rnk);
    bqn_shape(x,bqndims);
    IX(i,rnk,dims[i]=(npy_intp)bqndims[i]);free(bqndims);
    S n=bqn_bound(x);
    BQNElType t=bqn_directArrType(x);
    PO res;
    switch(t) {
        C(elt_i8,int8_t* data8=malloc(n);bqn_readI8Arr(x,data8);res=PyArray_SimpleNewFromData(rnk,dims,NPY_INT8,data8);)
        C(elt_i16,int16_t* data16=malloc(n*2);bqn_readI16Arr(x,data16);res=PyArray_SimpleNewFromData(rnk,dims,NPY_INT16,data16))
        C(elt_i32,int32_t* data32=malloc(n*4);bqn_readI32Arr(x,data32);res=PyArray_SimpleNewFromData(rnk,dims,NPY_INT32,data32))
        C(elt_f64,F* datad=malloc(n*8);bqn_readF64Arr(x,datad);res=PyArray_SimpleNewFromData(rnk,dims,NPY_DOUBLE,datad))
        D("Return type not supported.")
    }
    free(dims);
    PyArray_ENABLEFLAGS((NPA)res,NPY_ARRAY_OWNDATA);
    R res;
}

static PO bqn_bqn(K PO self, K PO args) {
    const char* inp;PO arg0=NULL;PO arg1=NULL;
    PyArg_ParseTuple(args, "s|OO", &inp, &arg0, &arg1);
    BQNV f=bqn_evalCStr(inp);
    $(arg0==NULL,K PO res=npy_bqn(f);bqn_free(f);R res,);
    if(arg1==NULL){
        BQNV x0=bqn_npy(arg0);
        BQNV bqnres=bqn_call1(f,x0);
        K PO res=npy_bqn(bqnres);
        bqn_free(x0);bqn_free(bqnres);bqn_free(f);
        R res;
    }
    else{
        BQNV x0=bqn_npy(arg0);
        BQNV x1=bqn_npy(arg1);
        BQNV bqnres=bqn_call2(f,x0,x1);
        K PO res=npy_bqn(bqnres);
        bqn_free(x0);bqn_free(x1);bqn_free(bqnres);bqn_free(f);
        R res;
    }
}

static PyMethodDef BqnMethods[] = {
    {"bqn", bqn_bqn, METH_VARARGS, "Call a BQN expression"},
    {NULL,NULL,0,NULL}
};

static struct PyModuleDef bqnmodule = {PyModuleDef_HEAD_INIT, "bqn", NULL, -1, BqnMethods};

PyMODINIT_FUNC PyInit_bqn(void) {bqn_init(); import_array(); R PyModule_Create(&bqnmodule);}
