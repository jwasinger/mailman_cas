/*
 * euckr_codec.h - $Revision: 1.5 $
 *
 * KoreanCodecs EUC-KR Codec C Implementation
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

static char euc_kr_decode__doc__[] = "EUC-KR decoder";

static PyObject *
euc_kr_decode(PyObject *self, PyObject *args)
{
    unsigned char *argstr, *srccur, *srcend;
    int arglen, errtype = error_strict;
    char *errors = NULL;
    Py_UNICODE *destptr, *destcur, *codemap, code;
    PyObject *r;

    if (!PyArg_ParseTuple(args, "s#|z:euc_kr_decode", &argstr, &arglen, &errors))
        return NULL;

    errtype = error_type(errors);
    if (errtype == error_undef)
        return NULL;

    destcur = destptr = PyMem_New(Py_UNICODE, arglen+1);
    for (srccur = argstr, srcend = argstr + arglen; srccur < srcend; srccur++) {
        if (*srccur & 0x80) {
          if (srccur+1 >= srcend) {
            switch (errtype) {
              case error_strict:
                PyMem_Del(destptr);
                PyErr_Format(PyExc_UnicodeError,
                             "EUC-KR decoding error: invalid character \\x%02x", *srccur);
                return NULL;
              case error_replace:
                *(destcur++) = UNIFIL;
                break;
              case error_ignore:
                break;
            }
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
invalid:        switch (errtype) {
                  case error_strict:
                    PyMem_Del(destptr);
                    PyErr_Format(PyExc_UnicodeError,
                                 "EUC-KR decoding error: invalid character \\x%02x%02x",
                                 srccur[0], srccur[1]);
                    return NULL;
                  case error_replace:
                    *(destcur++) = UNIFIL;
                    break;
                  case error_ignore:
                    break;
                }
                srccur++;
            }
          }
        } else
            *(destcur++) = *srccur;
    }

    r = codec_tuple(PyUnicode_FromUnicode(destptr, destcur-destptr), arglen);
    PyMem_Del(destptr);
    return r;
}


static char euc_kr_encode__doc__[] = "EUC-KR encoder";

static PyObject *
euc_kr_encode(PyObject *self, PyObject *args)
{
    Py_UNICODE *argptr, *srccur, *srcend;
    int arglen, errtype = error_strict;
    char *errors = NULL;
    unsigned char *destptr, *destcur, *decbuf;
    PyObject *r;

    if (!PyArg_ParseTuple(args, "u#|z:euc_kr_encode", &argptr, &arglen, &errors))
        return NULL;

    errtype = error_type(errors);
    if (errtype == error_undef)
        return NULL;

    destcur = destptr = PyMem_New(unsigned char, arglen*2+1);
    for (srccur = argptr, srcend = argptr + arglen; srccur < srcend; srccur++) {
        if (*srccur <= 0x7F)
            *(destcur++) = (unsigned char)*srccur;
        else
            if((decbuf = _ksc5601_encode(*srccur)) == 0) {
                switch (errtype) {
                  case error_strict:
                    PyMem_Del(destptr);
                    PyErr_Format(PyExc_UnicodeError,
                                 "EUC-KR encoding error: invalid character \\u%04x",
                                 *srccur);
                    return NULL;
                    break;
                  case error_replace:
                    *(destcur++) = 0xa1;
                    *(destcur++) = 0xa1;
                    break;
                  /* case error_ignore: break; */
                }
            } else {
                *(destcur++) = decbuf[0];
                *(destcur++) = decbuf[1];
            }
    }

    r = codec_tuple(PyString_FromStringAndSize((char*)destptr, destcur - destptr), arglen);
    PyMem_Del(destptr);
    return r;
}
