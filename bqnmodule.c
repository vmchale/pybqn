#include<Python.h>
#include<numpy/arrayobject.h>
#include<bqnffi.h>

#define R return
#define C case
#define BR break;
#define Sw switch

#undef I

typedef int I;typedef PyArrayObject* NPA;typedef PyObject* PO;typedef void* U;typedef double F;typedef size_t S;typedef long J;

#define ERR(str) PyErr_SetString(PyExc_RuntimeError, str);
#define DO(i,n,a) {I i;for(i=0;i<n;i++){a;}}

BQNV bqn_npy(PO o) {
    if(PyFloat_Check(o)){
        F x=PyFloat_AsDouble(o);
        R bqn_makeF64(x);
    }
    if(PyLong_Check(o)){
        ERR("Integer arguments are not supported.")
        R 0;
    }
    if(PyUnicode_Check(o)){
        S l=PyUnicode_GET_LENGTH(o);
        I k=PyUnicode_KIND(o);
        BQNV res;
        uint8_t* s8;uint16_t* s16;uint32_t* s32;
        Sw(k){
            C PyUnicode_1BYTE_KIND:
                s8=PyUnicode_1BYTE_DATA(o);
                res=bqn_makeC8Vec(l,s8);
                BR
            C PyUnicode_2BYTE_KIND:
                s16=PyUnicode_2BYTE_DATA(o);
                res=bqn_makeC16Vec(l,s16);
                BR
            C PyUnicode_4BYTE_KIND:
                s32=PyUnicode_4BYTE_DATA(o);
                res=bqn_makeC32Vec(l,s32);
                BR
            default:
                ERR("???") BR
        };
        R res;
    }
    NPA a=(NPA)o;
    I t=PyArray_TYPE(a);
    I rnk=PyArray_NDIM(a);
    S srnk=(S)rnk;
    npy_intp* dims=PyArray_DIMS(a);
    S* bqndims=malloc(sizeof(S)*rnk);
    DO(i,rnk,bqndims[i]=(S)dims[i]);free(dims);
    U data=PyArray_DATA(a);
    BQNV res;
    Sw(t) {
        C NPY_BYTE:
            res=bqn_makeI8Arr(srnk,bqndims,data);BR
        C NPY_SHORT:
            res=bqn_makeI16Arr(srnk,bqndims,data);BR
        C NPY_INT:
            res=bqn_makeI32Arr(srnk,bqndims,data);BR
        C NPY_DOUBLE:
            res=bqn_makeF64Arr(srnk,bqndims,data);BR
        default:
            ERR("Type not supported. 🤷")BR
    }
    free(bqndims);
    R res;
}

PO npy_bqn(BQNV x) {
    if(bqn_type(x)==1){
        R PyFloat_FromDouble(bqn_toF64(x));
    }
    S rnk=bqn_rank(x);
    J* dims=malloc(sizeof(J)*rnk);
    S* bqndims=malloc(sizeof(S)*rnk);
    bqn_shape(x,bqndims);
    DO(i,rnk,dims[i]=(J)bqndims[i]);free(bqndims);
    S n=bqn_bound(x);
    BQNElType t=bqn_directArrType(x);
    PO res;
    int8_t* data8;int16_t* data16;int32_t* data32;F* datad;
    Sw(t) {
        C elt_i8:
            data8=malloc(n);
            bqn_readI8Arr(x,data8);
            res=PyArray_SimpleNewFromData(rnk,dims,NPY_INT8,data8);BR
        C elt_i16:
            data16=malloc(n*2);
            bqn_readI16Arr(x,data16);
            res=PyArray_SimpleNewFromData(rnk,dims,NPY_INT16,data16);BR
        C elt_i32:
            data32=malloc(n*4);
            bqn_readI32Arr(x,data32);
            res=PyArray_SimpleNewFromData(rnk,dims,NPY_INT32,data32);BR
        C elt_f64:
            datad=malloc(n*8);
            bqn_readF64Arr(x,datad);
            res=PyArray_SimpleNewFromData(rnk,dims,NPY_DOUBLE,datad);BR
        default:
            ERR("Return type not supported.")BR
    }
    free(dims);
    PyArray_ENABLEFLAGS((PyArrayObject*)res,NPY_ARRAY_OWNDATA);
    R res;
}

static PO bqn_bqn(PO self, PO args) {
    const char* inp;PO arg0=NULL;PO arg1=NULL;
    PyArg_ParseTuple(args, "s|OO", &inp, &arg0, &arg1);
    BQNV f=bqn_evalCStr(inp);
    if(arg0==NULL){
        PO res=npy_bqn(f);
        bqn_free(f);
        R res;
    };
    if(arg1==NULL){
        BQNV x0=bqn_npy(arg0);
        BQNV bqnres=bqn_call1(f,x0);
        PO res=npy_bqn(bqnres);
        bqn_free(x0);bqn_free(bqnres);bqn_free(f);
        R res;
    }
    else{
        BQNV x0=bqn_npy(arg0);
        BQNV x1=bqn_npy(arg1);
        BQNV bqnres=bqn_call2(f,x0,x1);
        PO res=npy_bqn(bqnres);
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
