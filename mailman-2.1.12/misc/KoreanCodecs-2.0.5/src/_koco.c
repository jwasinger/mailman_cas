/*
 * _koco.c - $Revision: 1.21 $
 *
 * KoreanCodecs C Implementations
 *
 * Author  : Hye-Shik Chang <perky@FreeBSD.org>
 * Date    : $Date: 2002/07/19 00:01:53 $
 * Created : 15 March 2002
 *
 * This file is part of KoreanCodecs.
 *
 * KoreanCodecs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * KoreanCodecs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with KoreanCodecs; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

static char *version =
"$Id: _koco.c,v 1.21 2002/07/19 00:01:53 perky Exp $";

#define UNIFIL 0xfffd

#include "Python.h"

typedef int state_t;
typedef struct _streaminfo {
    int state;
    PyObject* (*decoder)(state_t*, char*, int slen, int errtype, PyObject* (*finalizer)(const Py_UNICODE *, int));
} streaminfo;
#define STATE_EXIST     0x100
#define HAS_STATE(c)    ((c)&STATE_EXIST)
#define GET_STATE(c)    (unsigned char)((c)&0xFF)
#define RESET_STATE(c) ((c)&=0xFE00)
#define SET_STATE(c, v) ((c)=STATE_EXIST|(v))

#ifndef max
#define max(a, b) ((a)<(b) ? (b) : (a))
#endif

#include "_koco_ksc5601.h"
#include "_koco_uhc.h"

/* error object and tuple creator code was stolen from Tamito's JapaneseCodecs */
static PyObject *ErrorObject;

enum { error_strict, error_ignore, error_replace, error_undef };

static PyObject *
codec_tuple(PyObject *unicode, int len)
{
    PyObject *v, *w;

    if (unicode == NULL)
        return NULL;
    v = PyTuple_New(2);
    if (v == NULL) {
        Py_DECREF(unicode);
        return NULL;
    }
    PyTuple_SET_ITEM(v, 0, unicode);
    w = PyInt_FromLong(len);
    if (w == NULL) {
        Py_DECREF(v);
        return NULL;
    }
    PyTuple_SET_ITEM(v, 1, w);
    return v;
}

static int
error_type(const char *errors) 
{
  if (errors == NULL || strcmp(errors, "strict") == 0) {
    return error_strict;
  }
  else if (strcmp(errors, "ignore") == 0) {
    return error_ignore;
  }
  else if (strcmp(errors, "replace") == 0) {
    return error_replace;
  }
  else {
    PyErr_Format(PyExc_ValueError,
                 "unknown error handling code: %.400s",
                 errors);
    return error_undef;
  }
}

static PyObject *
PyClass_New_WithMethods(const char *name, PyMethodDef *methods)
{
    PyMethodDef *def;

    PyObject *classDict = PyDict_New();
    PyObject *className = PyString_FromString(name);
    PyObject *newClass = PyClass_New(NULL, classDict, className);
    Py_DECREF(classDict);
    Py_DECREF(className);

    for (def = methods; def->ml_name != NULL; def++) {
        PyObject *func = PyCFunction_New(def, NULL);
        PyObject *method = PyMethod_New(func, NULL, newClass);
        PyDict_SetItemString(classDict, def->ml_name, method);
        Py_DECREF(method);
        Py_DECREF(func);
    }

    return newClass;
}

#include "euckr_codec.h"
#include "cp949_codec.h"
#include "koco_stream.h"

/* List of methods defined in the module */

#define meth(name, func, doc) {name, (PyCFunction)func, METH_VARARGS, doc}

static struct PyMethodDef _koco_methods[] = {
  meth("euc_kr_decode", euc_kr_decode, euc_kr_decode__doc__),
  meth("euc_kr_encode", euc_kr_encode, euc_kr_encode__doc__),
  meth("cp949_decode", cp949_decode, cp949_decode__doc__),
  meth("cp949_encode", cp949_encode, cp949_encode__doc__),
  {NULL, NULL},
};

/* Initialization function for the module */

void
init_koco(void)
{
    PyObject *m, *d, *t;

    /* Create the module and add the functions */
    m = Py_InitModule("_koco", _koco_methods);

    /* Add some symbolic constants to the module */
    d = PyModule_GetDict(m);

    t = PyClass_New_WithMethods("StreamReader", StreamReader_methods);
    PyDict_SetItemString(d, "StreamReader", t);
    Py_DECREF(t);

    t = PyString_FromString(version);
    PyDict_SetItemString(d, "version", t);
    Py_DECREF(t);

    ErrorObject = PyErr_NewException("_koco.error", NULL, NULL);
    PyDict_SetItemString(d, "error", ErrorObject);
    Py_DECREF(ErrorObject);

    /* Check for errors */
    if (PyErr_Occurred())
        Py_FatalError("can't initialize the _koco module");
}
