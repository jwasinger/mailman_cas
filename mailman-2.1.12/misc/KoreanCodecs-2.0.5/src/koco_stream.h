/*
 * euckr_stream.c - $Revision: 1.6 $
 *
 * KoreanCodecs EUC-KR StreamReader C Implementation
 *
 * Author  : Hye-Shik Chang <perky@FreeBSD.org>
 * Date    : $Date: 2002/07/19 00:01:53 $
 * Created : 28 April 2002
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

/*
 * TODO:
 *  __euc_kr_decode and __cp949_decode has so many big duplicated codes, now.
 */

static PyObject *
__euc_kr_decode(
    state_t *state, char *s, int slen, int errtype,
    PyObject* (*finalizer)(const Py_UNICODE *, int)
) {
    unsigned char *srccur, *srcend;
    Py_UNICODE *destptr, *destcur, *codemap, code;
    PyObject *r;

    destcur = destptr = PyMem_New(Py_UNICODE, slen+1);
    srccur = s;
    srcend = s + slen;

    if (HAS_STATE(*state)) {
        unsigned char c = GET_STATE(*state);

        if (c & 0x80) {
            if (slen > 0) {
                codemap = ksc5601_decode_map[c & 0x7F];
    
                if (!codemap)
                    goto invalid_state;
                if (ksc5601_decode_bottom <= *srccur && *srccur <= ksc5601_decode_top) {
                    code = codemap[*srccur - ksc5601_decode_bottom];
                    if (code == UNIFIL)
                        goto invalid_state;
                    *(destcur++) = code;
                    srccur++;
                } else {
invalid_state:      switch (errtype) {
                      case error_strict:
                        PyErr_Format(PyExc_UnicodeError,
                              "EUC-KR decoding error: invalid character \\x%02x%02x",
                              c, srccur[0]);
                        r = NULL;
                        goto out;
                      case error_replace:
                        *(destcur++) = UNIFIL;
                        break;
                      case error_ignore: break;
                    }
                    srccur++;
                }
            } else { /* keep state */
                r = PyUnicode_FromUnicode(NULL, 0);
                goto out;
            }
        } else
            *(destcur++) = c;

        RESET_STATE(*state);
    }

    for (; srccur < srcend; srccur++) {
        if (*srccur & 0x80) {
            if (srccur+1 >= srcend) /* state out */
                SET_STATE(*state, *srccur);
            else {
                codemap = ksc5601_decode_map[*srccur & 0x7F];
                if (!codemap)
                    goto invalid;
                if (ksc5601_decode_bottom <= srccur[1] && srccur[1] <= ksc5601_decode_top) {
                    code = codemap[srccur[1] - ksc5601_decode_bottom];
                    if (code == UNIFIL)
                        goto invalid;
                    *(destcur++) = code;
                    srccur++;
                } else {
invalid:            switch (errtype) {
                      case error_strict:
                        PyErr_Format(PyExc_UnicodeError,
                              "EUC-KR decoding error: invalid character \\x%02x%02x",
                              srccur[0], srccur[1]);
                        r = NULL;
                        goto out;
                      case error_replace:
                        *(destcur++) = UNIFIL;
                        break;
                      case error_ignore: break;
                    }
                    srccur++;
                }
            }
        } else
            *(destcur++) = *srccur;
    }

    r = finalizer(destptr, destcur-destptr);
out:
    PyMem_Del(destptr);
    return r;
}

static PyObject *
__cp949_decode(
    state_t *state, char *s, int slen, int errtype,
    PyObject* (*finalizer)(const Py_UNICODE *, int)
) {
    unsigned char *srccur, *srcend;
    Py_UNICODE *destptr, *destcur, *codemap, code;
    PyObject *r;

    destcur = destptr = PyMem_New(Py_UNICODE, slen+1);
    srccur = s;
    srcend = s + slen;

    if (HAS_STATE(*state)) {
        unsigned char c = GET_STATE(*state);

        if (c & 0x80) {
            if (slen > 0) {
                if (uhc_decode_hint[c]) { /* UHC page0 region */
                    codemap = uhc_decode_map[c & 0x7F];

                    if (uhc_page0_bottom <= *srccur && *srccur <= uhc_page0_top) {
                        code = codemap[*srccur - uhc_page0_bottom];
                        if (code == UNIFIL)
                            goto invalid;
                        *(destcur++) = code;
                        srccur++;
                    } else
                        goto invalid_state;
                } else if (uhc_decode_hint[*srccur]) { /* UHC page1 region */
                    codemap = uhc_decode_map[c & 0x7F];
                    if (!codemap)
                        goto invalid;

                    code = codemap[*srccur - uhc_page1_bottom];
                    if (code == UNIFIL)
                        goto invalid;
                    *(destcur++) = code;
                    srccur++;
                } else { /* KSC5601 */
                    codemap = ksc5601_decode_map[c & 0x7F];
        
                    if (!codemap)
                        goto invalid_state;
                    if (ksc5601_decode_bottom <= *srccur && *srccur <= ksc5601_decode_top) {
                        code = codemap[*srccur - ksc5601_decode_bottom];
                        if (code == UNIFIL)
                            goto invalid_state;
                        *(destcur++) = code;
                        srccur++;
                    } else {
invalid_state:          switch (errtype) {
                          case error_strict:
                            PyErr_Format(PyExc_UnicodeError,
                                  "CP949 decoding error: invalid character \\x%02x%02x",
                                  c, *srccur);
                            r = NULL;
                            goto out;
                          case error_replace:
                            *(destcur++) = UNIFIL;
                            break;
                          case error_ignore: break;
                        }
                        srccur++;
                    }
                }
            } else { /* keep state */
                r = PyUnicode_FromUnicode(NULL, 0);
                goto out;
            }
        } else
            *(destcur++) = c;

        RESET_STATE(*state);
    }

    for (; srccur < srcend; srccur++) {
        if (*srccur & 0x80) {
            if (srccur+1 >= srcend) /* state out */
                SET_STATE(*state, *srccur);
            else {
                if (uhc_decode_hint[*srccur]) { /* UHC page0 region */
                    codemap = uhc_decode_map[*srccur & 0x7F];
                    if (uhc_page0_bottom <= srccur[1] && srccur[1] <= uhc_page0_top) {
                        code = codemap[srccur[1] - uhc_page0_bottom];
                        if (code == UNIFIL)
                            goto invalid;
                        *(destcur++) = code;
                        srccur++;
                    } else
                        goto invalid;
                } else if (uhc_decode_hint[srccur[1]]) { /* UHC page1 region */
                    codemap = uhc_decode_map[*srccur & 0x7F];
                    if (!codemap)
                        goto invalid;
                    code = codemap[srccur[1] - uhc_page1_bottom];
                    if (code == UNIFIL)
                        goto invalid;
                    *(destcur++) = code;
                    srccur++;
                } else {
                    codemap = ksc5601_decode_map[*srccur & 0x7F];
                    if (!codemap)
                        goto invalid;
                    if (ksc5601_decode_bottom <= srccur[1] && srccur[1] <= ksc5601_decode_top) {
                        code = codemap[srccur[1] - ksc5601_decode_bottom];
                        if (code == UNIFIL)
                            goto invalid;
                        *(destcur++) = code;
                        srccur++;
                    } else {
invalid:                switch (errtype) {
                          case error_strict:
                            PyErr_Format(PyExc_UnicodeError,
                                  "CP949 decoding error: invalid character \\x%02x%02x",
                                  srccur[0], srccur[1]);
                            r = NULL;
                            goto out;
                          case error_replace:
                            *(destcur++) = UNIFIL;
                            break;
                          case error_ignore: break;
                        }
                        srccur++;
                    }
                }
            }
        } else
            *(destcur++) = *srccur;
    }

    r = finalizer(destptr, destcur-destptr);
out:
    PyMem_Del(destptr);
    return r;
}

PyObject* 
readline_finalizer(const Py_UNICODE *data, int datalen)
{
    PyObject *list, *uobj;
    const Py_UNICODE *linestart = data;

    if ((list = PyList_New(0)) == NULL) return NULL;

    for (;(datalen--) > 0; data++) {
        if (*data == '\n') {
append:     if ((uobj = PyUnicode_FromUnicode(linestart, data-linestart+1)) == NULL) {
                Py_DECREF(list);
                return NULL;
            }
            if (PyList_Append(list, uobj) == -1) {
                Py_DECREF(list);
                return NULL;
            }
            Py_DECREF(uobj);
            linestart = data+1;
        }
    }
    if (linestart < data) {
        data--;
        goto append; /* datalen < 0 here */
    }

    return list;
}

static void
streaminfo_destroy(void *obj)
{
    PyMem_Del(obj);
}

static char StreamReader___init____doc__[] = "StreamReader.__init__()";

static PyObject*
StreamReader___init__(PyObject *typeself, PyObject *args, PyObject *kwargs)
{
    PyObject *self, *stnfoobj, *encodingobj;
    PyObject *stream, *errors = NULL;
    streaminfo *stnfo;
    char *encoding;

    static char *kwlist[] = {"self", "stream", "errors", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs,
            "OO|O:__init__", kwlist, &self, &stream, &errors))
        return NULL;

    if ((encodingobj = PyObject_GetAttrString(self, "encoding")) == NULL)
        return NULL;
    if ((encoding = PyString_AsString(encodingobj)) == NULL)
        return NULL;

    stnfo = PyMem_New(streaminfo, 1);
    RESET_STATE(stnfo->state);

    if (!strcmp(encoding, "euc-kr"))
        stnfo->decoder = __euc_kr_decode;
    else if (!strcmp(encoding, "cp949"))
        stnfo->decoder = __cp949_decode;
    else {
        PyMem_Del(stnfo);
        PyErr_Format(PyExc_UnicodeError,
                     "can't initialize StreamReader: not supported encoding '%s'", encoding);
        return NULL;
    }

    stnfoobj = PyCObject_FromVoidPtr((void*)stnfo, streaminfo_destroy);
    PyObject_SetAttrString(self, "_streaminfo", stnfoobj);
    Py_DECREF(stnfoobj);

    PyObject_SetAttrString(self, "stream", stream);
    if (errors)
        PyObject_SetAttrString(self, "errors", errors);
    else {
        errors = PyString_FromString("strict");
        PyObject_SetAttrString(self, "errors", errors);
        Py_DECREF(errors);
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static char StreamReader_read__doc__[] = "StreamReader.read()";

static PyObject*
StreamReader_read(PyObject *typeself, PyObject *args)
{
    PyObject *self, *tmp = NULL, *r = NULL;
    PyObject *stream, *stnfoobj;
    streaminfo *stnfo;
    long size = -1;
    int  errtype;

    if (!PyArg_ParseTuple(args, "O|O:read", &self, &tmp))
        return NULL;

    if (tmp == Py_None || tmp == NULL)
        size = -1;
    else if (PyInt_Check(tmp))
        size = PyInt_AsLong(tmp);
    else {
        PyErr_SetString(PyExc_TypeError, "an integer is required");
        return NULL;
    }

    if (size == 0)
        return PyUnicode_FromUnicode(NULL, 0);

    if ((stream = PyObject_GetAttrString(self, "stream")) == NULL)
        return NULL;

    if ((tmp = PyObject_GetAttrString(self, "errors")) == NULL) {
        Py_DECREF(stream);
        return NULL;
    }
    errtype = error_type(PyString_AsString(tmp));
    Py_DECREF(tmp);
    if (errtype == error_undef)
        return NULL;

    if ((stnfoobj = PyObject_GetAttrString(self, "_streaminfo")) == NULL) {
        Py_DECREF(stream);
        return NULL;
    }
    if ((stnfo = (streaminfo*)PyCObject_AsVoidPtr(stnfoobj)) == NULL)
        goto out;

    if (size < 0)
        tmp = PyObject_CallMethod(stream, "read", NULL); /* without tuple */
    else
        tmp = PyObject_CallMethod(stream, "read", "i",
                HAS_STATE(stnfo->state) ? size : max(2, size) );
        
    if (tmp == NULL)
        goto out;

    r = stnfo->decoder(
        &(stnfo->state), PyString_AS_STRING(tmp), PyString_GET_SIZE(tmp), errtype,
        PyUnicode_FromUnicode
    );
    Py_DECREF(tmp);

out:
    Py_DECREF(stream);
    Py_DECREF(stnfoobj);
    return r;
}

static char StreamReader_readline__doc__[] = "StreamReader.readline()";

static PyObject*
StreamReader_readline(PyObject *typeself, PyObject *args)
{
    PyObject *self, *tmp = NULL, *r = NULL;
    PyObject *stream, *stnfoobj;
    streaminfo *stnfo;
    long size = -1;
    int errtype;

    if (!PyArg_ParseTuple(args, "O|O:readline", &self, &tmp))
        return NULL;

    if (tmp == Py_None || tmp == NULL)
        size = -1;
    else if (PyInt_Check(tmp))
        size = PyInt_AsLong(tmp);
    else {
        PyErr_SetString(PyExc_TypeError, "an integer is required");
        return NULL;
    }

    if (size == 0)
        return PyUnicode_FromUnicode(NULL, 0);

    if ((stream = PyObject_GetAttrString(self, "stream")) == NULL)
        return NULL;

    if ((tmp = PyObject_GetAttrString(self, "errors")) == NULL) {
        Py_DECREF(stream);
        return NULL;
    }
    errtype = error_type(PyString_AsString(tmp));
    Py_DECREF(tmp);
    if (errtype == error_undef)
        return NULL;

    if ((stnfoobj = PyObject_GetAttrString(self, "_streaminfo")) == NULL) {
        Py_DECREF(stream);
        return NULL;
    }
    if ((stnfo = (streaminfo*)PyCObject_AsVoidPtr(stnfoobj)) == NULL)
        goto out;

    if (size < 0)
        tmp = PyObject_CallMethod(stream, "readline", NULL); /* without tuple */
    else
        tmp = PyObject_CallMethod(stream, "readline", "i",
                HAS_STATE(stnfo->state) ? size : max(2, size) );
        
    if (tmp == NULL)
        goto out;

    r = stnfo->decoder(
        &(stnfo->state), PyString_AS_STRING(tmp), PyString_GET_SIZE(tmp), errtype,
        PyUnicode_FromUnicode
    );
    Py_DECREF(tmp);

out:
    Py_DECREF(stream);
    Py_DECREF(stnfoobj);
    return r;
}

static char StreamReader_readlines__doc__[] = "StreamReader.readlines()";

static PyObject*
StreamReader_readlines(PyObject *typeself, PyObject *args)
{
    PyObject *self, *r = NULL, *tmp = NULL;
    PyObject *stream, *stnfoobj;
    streaminfo *stnfo;
    int size = -1, errtype;

    if (!PyArg_ParseTuple(args, "O|O:readlines", &self, &tmp))
        return NULL;

    if (tmp == Py_None || tmp == NULL)
        size = -1;
    else if (PyInt_Check(tmp))
        size = PyInt_AsLong(tmp);
    else {
        PyErr_SetString(PyExc_TypeError, "an integer is required");
        return NULL;
    }

    if (size == 0)
        return PyUnicode_FromUnicode(NULL, 0);

    if ((stream = PyObject_GetAttrString(self, "stream")) == NULL)
        return NULL;

    if ((tmp = PyObject_GetAttrString(self, "errors")) == NULL) {
        Py_DECREF(stream);
        return NULL;
    }
    errtype = error_type(PyString_AsString(tmp));
    Py_DECREF(tmp);
    if (errtype == error_undef)
        return NULL;

    if ((stnfoobj = PyObject_GetAttrString(self, "_streaminfo")) == NULL) {
        Py_DECREF(stream);
        return NULL;
    }
    if ((stnfo = (streaminfo*)PyCObject_AsVoidPtr(stnfoobj)) == NULL)
        goto out;

    if (size < 0)
        tmp = PyObject_CallMethod(stream, "read", NULL); /* without tuple */
    else
        tmp = PyObject_CallMethod(stream, "read", "i",
                HAS_STATE(stnfo->state) ? size : max(2, size) );
        
    if (tmp == NULL)
        goto out;

    r = stnfo->decoder(
        &(stnfo->state), PyString_AS_STRING(tmp), PyString_GET_SIZE(tmp), errtype,
        readline_finalizer
    );
    Py_DECREF(tmp);

out:
    Py_DECREF(stream);
    Py_DECREF(stnfoobj);
    return r;
}

static char StreamReader_reset__doc__[] = "StreamReader.reset()";

static PyObject*
StreamReader_reset(PyObject *typeself, PyObject *args)
{
    PyObject *self, *stnfoobj;
    streaminfo *stnfo;

    if (!PyArg_ParseTuple(args, "O|:reset", &self))
        return NULL;

    if ((stnfoobj = PyObject_GetAttrString(self, "_streaminfo")) == NULL)
        return NULL;

    if ((stnfo = (streaminfo*)PyCObject_AsVoidPtr(stnfoobj)) != NULL)
        RESET_STATE(stnfo->state);

    Py_DECREF(stnfoobj);
    Py_INCREF(Py_None);
    return Py_None;
}

struct PyMethodDef StreamReader_methods[] = {
    {"__init__", (PyCFunction) StreamReader___init__,
                 METH_VARARGS | METH_KEYWORDS,
                 StreamReader___init____doc__},
    {"read",     (PyCFunction) StreamReader_read,
                 METH_VARARGS,
                 StreamReader_read__doc__},
    {"readline", (PyCFunction) StreamReader_readline,
                 METH_VARARGS,
                 StreamReader_readline__doc__},
    {"readlines",(PyCFunction) StreamReader_readlines,
                 METH_VARARGS,
                 StreamReader_readlines__doc__},
    {"reset",    (PyCFunction) StreamReader_reset,
                 METH_VARARGS,
                 StreamReader_reset__doc__},
    {NULL,},
};
