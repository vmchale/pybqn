#include<Python.h>
#include<numpy/arrayobject.h>
#include<bqnffi.h>

#define R return
#define PO PyObject*
#define C case
#define BR break;

#define ERR(str) PyErr_SetString(PyExc_RuntimeError, str);
#define DO(i,n,a) {int i;for(i=0;i<n;i++){a;}}

BQNV bqn_npy(PO o) {
}

PO npy_bqn(BQNV x) {
    if(bqn_type(x)==1){
        R PyFloat_FromDouble(bqn_toF64(x));
    }
    size_t rnk=bqn_rank(x);
    long* dims=malloc(sizeof(long)*rnk);
    size_t* bqndims=malloc(sizeof(size_t)*rnk);
    bqn_shape(x,bqndims);
    DO(i,rnk,dims[i]=(long)bqndims[i]);free(bqndims);
    size_t n=bqn_bound(x);
    BQNElType t=bqn_directArrType(x);
    PO res;
    switch(t) {
        C elt_i8:
            int8_t* data8=malloc(n);
            bqn_readI8Arr(x,data8);
            res=PyArray_SimpleNewFromData(rnk,dims,NPY_INT8,data8);BR
        C elt_i16:
            int16_t* data16=malloc(n*2);
            bqn_readI16Arr(x,data16);
            res=PyArray_SimpleNewFromData(rnk,dims,NPY_INT16,data16);BR
        C elt_i32:
            int32_t* data32=malloc(n*4);
            bqn_readI32Arr(x,data32);
            res=PyArray_SimpleNewFromData(rnk,dims,NPY_INT32,data32);BR
        default:
            ERR("Return type not supported.")BR
    }
    free(dims);
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
    ERR("Expected nilad, monad, or dyad.")
}

static PyMethodDef BqnMethods[] = {
    {"bqn", bqn_bqn, METH_VARARGS, "Call a BQN expression"},
    {NULL,NULL,0,NULL}
};

static struct PyModuleDef bqnmodule = { PyModuleDef_HEAD_INIT, "bqn", NULL, -1, BqnMethods };

PyMODINIT_FUNC PyInit_bqn(void) { bqn_init(); import_array(); R PyModule_Create(&bqnmodule); }
