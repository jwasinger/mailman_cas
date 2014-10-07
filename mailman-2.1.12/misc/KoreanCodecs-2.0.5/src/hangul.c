/*
 * hangul.c - $Revision: 1.14 $
 *
 * KoreanCodecs Hangul Module C Implementation
 *
 * Author  : Hye-Shik Chang <perky@FreeBSD.org>
 * Date    : $Date: 2002/07/19 00:01:53 $
 * Created : 25 April 2002
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
"$Id: hangul.c,v 1.14 2002/07/19 00:01:53 perky Exp $";

#include "Python.h"

enum { /* Jaeum Codes on U+3100 */
    G = 0x3131, GG, GS, N, NJ, NH, D, DD, L, LG, LM, LB,
    LS, LT, LP, LH, M, B, BB, BS, S, SS, NG, J, JJ, C, K, T, P, H
};

enum { /* Moeum Codes on U+3100 */
    A = 0x314f, AE, YA, YAE, EO, E, YEO, YE, O, WA, WAE, OE, YO,
    U, WEO, WE, WI, YU, EU, YI, I
};

#define NCHOSUNG    19
#define NJUNGSUNG   21
#define NJONGSUNG   28

#define NJAEUM      30
#define NMOEUM      21

#define JAEUM_BOTTOM G
#define JAEUM_TOP   H
#define MOEUM_BOTTOM A
#define MOEUM_TOP   I

#define HANGUL_BOTTOM 0xac00
#define HANGUL_TOP  0xd7a3

#define JBASE_CHOSUNG 0x1100
#define JBASE_JUNGSUNG 0x1161
#define JBASE_JONGSUNG 0x11A8
#define CHOSUNG_FILLER 0x115f
#define JUNGSUNG_FILLER 0x1160

static PyObject *UniNull, *UniSpace;
static PyObject *ErrorObject;

#define MAX_MULTIJAMO   3
typedef struct _jamotype {
    char *name;
    Py_UNICODE code;
    int multi[MAX_MULTIJAMO];
    char orders[3]; /* cho, jung, jong */
} jamotype;

#define CODE(c)     #c,c
#define NOMULTI     {0,0,0}
#define J_C         {0,-1,-1}
#define J_J         {-1,-1,0}
#define J_CJ        {0,-1,0}
#define M_J         {-1,0,-1}
static jamotype jamos[] = {
/* JAEUM */
    { CODE(G),  NOMULTI, J_CJ }, { CODE(GG), {G, G,}, J_CJ }, { CODE(GS), {G, S,}, J_J  },
    { CODE(N),  NOMULTI, J_CJ }, { CODE(NJ), {N, J,}, J_J  }, { CODE(NH), {N, H,}, J_J  },
    { CODE(D),  NOMULTI, J_CJ }, { CODE(DD), {D, D,}, J_C  }, { CODE(L),  NOMULTI, J_CJ },
    { CODE(LG), {L, G,}, J_J  }, { CODE(LM), {L, M,}, J_J  }, { CODE(LB), {L, B,}, J_J  },
    { CODE(LS), {L, S,}, J_J  }, { CODE(LT), {L, T,}, J_J  }, { CODE(LP), {L, P,}, J_J  },
    { CODE(LH), {L, H,}, J_J  }, { CODE(M),  NOMULTI, J_CJ }, { CODE(B),  NOMULTI, J_CJ },
    { CODE(BB), {B, B,}, J_C  }, { CODE(BS), {B, S,}, J_J  }, { CODE(S),  NOMULTI, J_CJ },
    { CODE(SS), {S, S,}, J_CJ }, { CODE(NG), NOMULTI, J_CJ }, { CODE(J),  NOMULTI, J_CJ },
    { CODE(JJ), {J, J,}, J_C  }, { CODE(C),  NOMULTI, J_CJ }, { CODE(K),  NOMULTI, J_CJ },
    { CODE(T),  NOMULTI, J_CJ }, { CODE(P),  NOMULTI, J_CJ }, { CODE(H),  NOMULTI, J_CJ },
/* MOEUM */
    { CODE(A),  NOMULTI, M_J  }, { CODE(AE), {A, I,}, M_J  }, { CODE(YA), NOMULTI, M_J  },
    { CODE(YAE), {YA,I}, M_J  }, { CODE(EO), NOMULTI, M_J  }, { CODE(E),  NOMULTI, M_J  },
    { CODE(YEO), NOMULTI, M_J }, { CODE(YE), {YEO,I}, M_J  }, { CODE(O),  NOMULTI, M_J  },
    { CODE(WA),  {O, A}, M_J  }, { CODE(WAE), {O,A,I}, M_J }, { CODE(OE), {O, I},  M_J  },
    { CODE(YO), NOMULTI, M_J  }, { CODE(U),  NOMULTI, M_J  }, { CODE(WEO), {U, EO}, M_J },
    { CODE(WE), {U, E},  M_J  }, { CODE(WI), {U, I},  M_J  }, { CODE(YU), NOMULTI, M_J  },
    { CODE(EU), NOMULTI, M_J  }, { CODE(YI), {EU, I}, M_J  }, { CODE(I),  NOMULTI, M_J  },
/* END MARKER */
    { 0, 0, NOMULTI, {0,} },
};
#undef NOMULTI
#undef CODE

static jamotype *jamo_chosung[NCHOSUNG], *jamo_jungsung[NJUNGSUNG], *jamo_jongsung[NJONGSUNG];

#define getJamotype(c) jamos[(c)-JAEUM_BOTTOM]
#define isJaeum(c) (JAEUM_BOTTOM <= (c) && (c) <= JAEUM_TOP)
#define isMoeum(c) (MOEUM_BOTTOM <= (c) && (c) <= MOEUM_TOP)
#define isHangulSyllable(c) (HANGUL_BOTTOM <= (c) && (c) <= HANGUL_TOP)
#define isChosung(c) (getJamotype(c).orders[0] >= 0)
#define isJungsung(c) (getJamotype(c).orders[1] >= 0)
#define isJongsung(c) (getJamotype(c).orders[2] >= 0)
#define getChosungOrder(c) (getJamotype(c).orders[0])
#define getJungsungOrder(c) (getJamotype(c).orders[1])
#define getJongsungOrder(c) (getJamotype(c).orders[2])


static char hangul_isJaeum__doc__[] = "isJaeum(code): Verify whether the code is Jaeum.";

static PyObject *
hangul_isJaeum(PyObject *self, PyObject *args)
{
    Py_UNICODE *code;
    int codelen, istrue = 0;

    if (!PyArg_ParseTuple(args, "u#:isJaeum", &code, &codelen))
        return NULL;

    if (codelen)
        for (istrue = 1; codelen--; code++)
            if (!isJaeum(*code)) {
                istrue = 0;
                break;
            }

    if (istrue) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static char hangul_isMoeum__doc__[] = "isMoeum(code): Verify whether the code is Moeum.";

static PyObject *
hangul_isMoeum(PyObject *self, PyObject *args)
{
    Py_UNICODE *code;
    int codelen, istrue = 0;

    if (!PyArg_ParseTuple(args, "u#:isMoeum", &code, &codelen))
        return NULL;

    if (codelen)
        for (istrue = 1; codelen--; code++)
            if (!isMoeum(*code)) {
                istrue = 0;
                break;
            }

    if (istrue) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static char hangul_ishangul__doc__[] = "ishangul(code): Verify whether the code is hangul.";

static PyObject *
hangul_ishangul(PyObject *self, PyObject *args)
{
    Py_UNICODE *code;
    int codelen, istrue = 0;

    if (!PyArg_ParseTuple(args, "u#:ishangul", &code, &codelen))
        return NULL;

    if (codelen)
        for (istrue = 1; codelen--; code++)
            if (!(isHangulSyllable(*code) || isJaeum(*code) || isMoeum(*code))) {
                istrue = 0;
                break;
            }

    if (istrue) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    else {
        Py_INCREF(Py_False);
        return Py_False;
    }
}

static char hangul_join__doc__[] = "join([chosung, jungsung, jongsung]): Assemble hangul syllable from jamos.";

static PyObject *
hangul_join(PyObject *self, PyObject *args)
{
    PyObject *argchar, *argelems[3];
    Py_UNICODE elems[3], *uobj;
    int i;

    if (!PyArg_ParseTuple(args, "O:join", &argchar))
        return NULL;

    if (PyList_Check(argchar)) {
        if (PyList_GET_SIZE(argchar) != 3)
            goto argerr;
        for (i = 0; i < 3; i ++)
            argelems[i] = PyList_GET_ITEM(argchar, i);
    }
    else if (PyTuple_Check(argchar)) {
        if (PyTuple_GET_SIZE(argchar) != 3)
            goto argerr;
        for (i = 0; i < 3; i ++)
            argelems[i] = PyTuple_GET_ITEM(argchar, i);
    }
    else {
argerr: PyErr_Format(PyExc_ValueError, "need list or tuple with 3 unicode elements");
        return NULL;
    }

    for (i = 0; i < 3; i ++) {
        if ((uobj = PyUnicode_AsUnicode(argelems[i])) == NULL)
            goto argerr;
        if (PyUnicode_GET_SIZE(argelems[i]))
            elems[i] = *uobj;
        else
            elems[i] = 0;
    }

    if ( (elems[0] && (!isJaeum(elems[0]) || !isChosung(elems[0]))) /* Chosung validity */
         || (elems[1] && (!isMoeum(elems[1]))) /* Jungsung validity */
         || (elems[2] && (!isJaeum(elems[2]) || !isJongsung(elems[2])))   ) {
        PyErr_Format(ErrorObject, "not valid jamo combination");
        return NULL;
    }

    if ((!elems[0] || !elems[1]) && elems[2]) {
        PyErr_Format(ErrorObject, "trying to assemble character which "
                                  "is not in unicode map");
        return NULL;
    }
    else if (elems[0] && !elems[1]) {
        Py_INCREF(argelems[0]);
        return argelems[0];
    }
    else if (elems[1] && !elems[0]) {
        Py_INCREF(argelems[1]);
        return argelems[1];
    }
    else if (!elems[0]) { /* [Null, Null, Null] */
        Py_INCREF(UniSpace);
        return UniSpace;
    }
    else {
        Py_UNICODE code;

        code = ((getChosungOrder(elems[0]) * NJUNGSUNG) + getJungsungOrder(elems[1])) * 
                 NJONGSUNG + (elems[2]?getJongsungOrder(elems[2]):0) + HANGUL_BOTTOM;
        return PyUnicode_FromUnicode(&code, 1);
    }
}

static char hangul_split__doc__[] = "split(code): Disassemble hangul syllable into jamos.";

static PyObject *
hangul_split(PyObject *self, PyObject *args)
{
    Py_UNICODE *code;
    PyObject *r;
    int codelen;

    if (!PyArg_ParseTuple(args, "u#:split", &code, &codelen))
        return NULL;

    if (codelen < 1) {
        PyErr_Format(PyExc_ValueError, "need not null unicode string");
        return NULL;
    }

    if (isHangulSyllable(*code)) {
        Py_UNICODE cho, jung, jong;
        PyObject *jongobj;
        Py_UNICODE hseq, t;
        
        hseq = *code - HANGUL_BOTTOM;

        cho  = jamo_chosung[hseq / (NJUNGSUNG*NJONGSUNG)]->code;
        jung = jamo_jungsung[(hseq / NJONGSUNG) % NJUNGSUNG]->code;

        if ((t = hseq % NJONGSUNG)) {
            jong = jamo_jongsung[t]->code;
            jongobj = PyUnicode_FromUnicode(&jong, 1);
        } else {
            jongobj = UniNull;
            Py_INCREF(UniNull);
        }

        r = PyTuple_New(3);
        PyTuple_SET_ITEM(r, 0, PyUnicode_FromUnicode(&cho, 1));
        PyTuple_SET_ITEM(r, 1, PyUnicode_FromUnicode(&jung, 1));
        PyTuple_SET_ITEM(r, 2, jongobj);

        return r;
    }
    else if (isJaeum(*code)) {
        r = PyTuple_New(3);
        PyTuple_SET_ITEM(r, 0, PyUnicode_FromUnicode(code, 1));
        PyTuple_SET_ITEM(r, 1, UniNull); Py_INCREF(UniNull);
        PyTuple_SET_ITEM(r, 2, UniNull); Py_INCREF(UniNull);
        return r;
    }
    else if (isMoeum(*code)) {
        r = PyTuple_New(3);
        PyTuple_SET_ITEM(r, 0, UniNull); Py_INCREF(UniNull);
        PyTuple_SET_ITEM(r, 1, PyUnicode_FromUnicode(code, 1));
        PyTuple_SET_ITEM(r, 2, UniNull); Py_INCREF(UniNull);
        return r;
    }
    else {
        PyErr_Format(ErrorObject, "not a hangul code");
        return NULL;
    }
}

static char hangul_conjoin__doc__[] = "conjoin(unicodestring): conjoin unicode johab string into unicode syllable string";

static PyObject *
hangul_conjoin(PyObject *self, PyObject *args)
{
    PyObject *r;
    Py_UNICODE *code, *dst, *dstorg, c;
    int cho, jung, jong;
    int codelen, i;

    if (!PyArg_ParseTuple(args, "u#:conjoin", &code, &codelen))
        return NULL;

    dstorg = dst = PyMem_New(Py_UNICODE, codelen);

    for (i = 0; i < codelen; i++) {
        c = code[i];
        if ((JBASE_CHOSUNG <= c && c <= 0x1112) || c == CHOSUNG_FILLER) {
            if (codelen > i+1 && JUNGSUNG_FILLER <= code[i+1] && code[i+1] <= 0x1175) {
                if (c == CHOSUNG_FILLER) cho = -1;
                else cho = c - JBASE_CHOSUNG;
                if (code[i+1] == JUNGSUNG_FILLER) jung = -1;
                else jung = code[i+1] - JBASE_JUNGSUNG;

                if (codelen > i+2 && JBASE_JONGSUNG <= code[i+2] && code[i+2] <= 0x11c2) {
                    jong = code[i+2] - JBASE_JONGSUNG + 1;
                    i += 2;
                }
                else {
                    jong = 0; i++;
                }

                if (jong && (cho == -1 || jung == -1)) { /* can't trans to syllable */
                    if (cho >= 0)  *(dst++) = jamo_chosung[cho]->code;
                    if (jung >= 0) *(dst++) = jamo_jungsung[jung]->code;
                    *(dst++) = jamo_jongsung[jong]->code;
                }
                else if (cho == -1) /* jungsung only */
                    *(dst++) = jamo_jungsung[jung]->code;
                else if (jung == -1) /* chosung only */
                    *(dst++) = jamo_chosung[cho]->code;
                else /* full set */
                    *(dst++) = HANGUL_BOTTOM + (cho * NJUNGSUNG + jung) * NJONGSUNG + jong;
            }
            else if (c != CHOSUNG_FILLER) /* chosung only */
                *(dst++) = jamo_chosung[c-JBASE_CHOSUNG]->code;
        }
        else if (JBASE_JUNGSUNG <= c && c <= 0x1175) /* jungsung only */
            *(dst++) = jamo_jungsung[c-JBASE_JUNGSUNG]->code;
        else
            *(dst++) = c;
    }

    r = PyUnicode_FromUnicode(dstorg, dst-dstorg);
    PyMem_Del(dstorg);

    return r;
}


static char hangul_disjoint__doc__[] = "disjoint(unicodestring): disjoint unicode syllable string into unicode johab string";

static PyObject *
hangul_disjoint(PyObject *self, PyObject *args)
{
    Py_UNICODE *code, *dst, *dstorg, c;
    PyObject *r;
    int codelen, i;

    if (!PyArg_ParseTuple(args, "u#:split", &code, &codelen))
        return NULL;

    dstorg = dst = PyMem_New(Py_UNICODE, codelen*3);

    for (i = 0; i < codelen; i++) {
        c = code[i];
        if (isHangulSyllable(c)) {
            int hseq;
            Py_UNICODE jong;

            hseq = c - HANGUL_BOTTOM;
            jong = hseq % NJONGSUNG;

            *(dst++) = hseq / (NJUNGSUNG * NJONGSUNG) + JBASE_CHOSUNG;
            *(dst++) = (hseq / NJONGSUNG) % NJUNGSUNG + JBASE_JUNGSUNG;
            if (jong)
                *(dst++) = jong + JBASE_JONGSUNG - 1;
        }
        else if (isJaeum(c) && isChosung(c)) {
            *(dst++) = getChosungOrder(c) + JBASE_CHOSUNG;
            *(dst++) = JUNGSUNG_FILLER;
        }
        else if (isMoeum(c)) {
            *(dst++) = CHOSUNG_FILLER;
            *(dst++) = getJungsungOrder(c) + JBASE_JUNGSUNG;
        } else
            *(dst++) = c;
    }

    r = PyUnicode_FromUnicode(dstorg, dst-dstorg);
    PyMem_Del(dstorg);

    return r;
}


static char pseudofinal[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 0 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 1 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,  /* 2 */
    1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,  /* 3 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0,  /* 4 */
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,  /* 5 */
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,  /* 6 */
    1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 7 */
};

static char hangul_format__doc__[] = "format(fmt, arg1, arg2, ...) or format(fmt, kw1=arg1, kw2=arg2"
                            ", ...):\nformat unicode string and fix korean suffixes after arguments";

static PyObject *
hangul_format(PyObject *self, PyObject *args, PyObject *kwargs)
{
/*--- Poor Structure of this function ;)
  hangul_format(fmt, *args, **kwargs)
    -> insert end fmtmarkers(U+115E which is not used by Unicode) after every format position
      -> PyUnicode_Format
        -> Fix and update hangul suffixes in place of fmtmarkers
          -> make PyObject and return.
 */
#define FMTMARKER 0x115E
    Py_UNICODE *fmt, *fmtout, *fcur;
    PyObject *r;
    int fmtsize;
    int inpth, infmt, escape;

    {
        PyObject *fmtobj;
        int argsize;

        argsize = PyTuple_GET_SIZE(args);
        if (!argsize || !PyUnicode_Check(fmtobj = PyTuple_GET_ITEM(args, 0))) {
            PyErr_Format(PyExc_TypeError, "needs unicode format string.");
            return NULL;
        }
        fmtsize = PyUnicode_GET_SIZE(fmtobj);
        fmt = PyUnicode_AS_UNICODE(fmtobj);

        if (!kwargs)
            args = PyTuple_GetSlice(args, 1, argsize);
    }

    fmtout = PyMem_New(Py_UNICODE, fmtsize + fmtsize/2);
    inpth = infmt = escape = 0;

    for (fcur = fmtout; fmtsize--; fmt++) {
        if (*fmt != FMTMARKER) /* skip bogus markers */
            *(fcur++) = *fmt;

        if (escape)
            escape = 0;
        else if (*fmt == '\\')
            escape = 1;
        else if (infmt) {
            if (!inpth && (('A' <= *fmt && *fmt <= 'Z') || ('a' <= *fmt && *fmt <= 'z'))) {
                *(fcur++) = FMTMARKER;
                infmt = 0;
            }
            else if (inpth && *fmt == ')')
                inpth = 0;
            else if (*fmt == '(')
                inpth = 1;
            else if (*fmt == '%')
                infmt = 0;
        }
        else if (*fmt == '%')
            infmt = 1;
    }

    r = PyUnicode_Format(
            PyUnicode_FromUnicode(fmtout, fcur-fmtout),
            kwargs?kwargs:args
        );
    if (!kwargs) {
        Py_DECREF(args);
    } /* {} to avoid gcc warning */
    if (!r)
        goto out;

    fmt       = PyUnicode_AS_UNICODE(r);
    fmtsize   = PyUnicode_GET_SIZE(r);

#define HAS_FINAL() ( \
    (past = *(fmt-1)), \
    isHangulSyllable(past) ?  \
        ((past-HANGUL_BOTTOM) % NJONGSUNG > 0) \
        : (past < 0x80 ? pseudofinal[past] : 0) \
)

#define HAS_FINAL_OR_NOTSYL() ( \
    (past = *(fmt-1)), \
    isHangulSyllable(past) ?  \
        ((past-HANGUL_BOTTOM) % NJONGSUNG > 0) \
        : 1 \
)

#define PROCESSSUFFIX(nofinal, existfinal) \
    if (next == nofinal || next == existfinal) { \
        *(fcur++) = HAS_FINAL() ? (existfinal) : (nofinal); \
        fmtsize--; fmt++; \
    }

#define PROCESSSUFFIX_IDA(jongsungadder, existfinal) \
    if (next == existfinal) { \
        if (HAS_FINAL_OR_NOTSYL()) \
            *(fcur++) = existfinal; \
        else \
            *(fcur-1) += jongsungadder; \
        fmtsize-=3; fmt+=3; \
    }

    for (fcur = fmtout; fmtsize--; fmt++) {
        if (*fmt == FMTMARKER) {
            if (fcur > fmtout && fmtsize > 0) {
                Py_UNICODE past, next = *(fmt+1);

                if (next == '(' && fmtsize > 2 && *(fmt+3) == ')') { /* ida suffxes */
                    next = *(fmt+2);
                    PROCESSSUFFIX_IDA(0, 0xc774) /* (I)DA */
                    else PROCESSSUFFIX_IDA(17, 0xc785) /* (IP)NIDA */
                    else PROCESSSUFFIX_IDA(4, 0xc778) /* (IN)- */
                }
                else if (0xac00 <= next && next <= 0xc774) {
                    PROCESSSUFFIX(0xb97c, 0xc744) /* REUL, EUL */
                    else PROCESSSUFFIX(0xb294, 0xc740) /* NEUN, EUN */
                    else PROCESSSUFFIX(0xac00, 0xc774) /* GA, I */
                    else PROCESSSUFFIX(0xc640, 0xacfc) /* WA, GWA */
                }
            }
        }
        else
            *(fcur++) = *fmt;
    }

/* these were written separatedly for win32 compilers */
#undef PROCESSSUFFIX
#undef PROCESSSUFFIX_IDA
#undef HAS_FINAL
#undef HAS_FINAL_OR_NOTSYL

    Py_DECREF(r);
    r = PyUnicode_FromUnicode(fmtout, fcur-fmtout);

out:
    PyMem_Free(fmtout);
    return r;
}

/* List of methods defined in the module */

#define meth(name, func, doc) {name, (PyCFunction)func, METH_VARARGS, doc}
#define meth_kw(name, func, doc) {name, (PyCFunction)func, METH_VARARGS|METH_KEYWORDS, doc}

static struct PyMethodDef hangul_methods[] = {
  meth("isJaeum",   hangul_isJaeum,     hangul_isJaeum__doc__),
  meth("isMoeum",   hangul_isMoeum,     hangul_isMoeum__doc__),
  meth("ishangul",  hangul_ishangul,    hangul_ishangul__doc__),
  meth("join",      hangul_join,        hangul_join__doc__),
  meth("split",     hangul_split,       hangul_split__doc__),
  meth("conjoin",   hangul_conjoin,     hangul_conjoin__doc__),
  meth("disjoint",  hangul_disjoint,    hangul_disjoint__doc__),
  meth_kw("format", hangul_format,      hangul_format__doc__),
  {NULL, NULL},
};

#define SET_INTCONSTANT(dict, value) \
            PyDict_SetItemString(dict, #value, PyInt_FromLong((long) value))
#define SET_STRCONSTANT(dict, value) \
            PyDict_SetItemString(dict, #value, PyString_FromString(value))
#define SET_CHARCONSTANT(dict, value) \
            PyDict_SetItemString(dict, #value, PyString_FromFormat("%c", value))

/* Initialization function for the module */

void
inithangul(void)
{
    PyObject *m, *d, *tmp;
    Py_UNICODE tuni[2];
    int i;

    /* Create the module and add the functions */
    m = Py_InitModule("hangul", hangul_methods);

    UniNull = PyUnicode_FromUnicode(NULL, 0);
    tuni[0] = 0x3000; /* Unicode Double-wide Space */
    UniSpace = PyUnicode_FromUnicode(tuni, 1);

    /* Add some symbolic constants to the module */
    d = PyModule_GetDict(m);
    SET_INTCONSTANT(d, NCHOSUNG);
    SET_INTCONSTANT(d, NJUNGSUNG);
    SET_INTCONSTANT(d, NJONGSUNG);
    {
        PyObject *Chosung, *Jungsung, *Jongsung;
        PyObject *Jaeum, *Moeum;
        PyObject *JaeumDict, *MoeumDict;
        PyObject *JaeumCodes, *MoeumCodes;
        PyObject *JaeumMulti, *MoeumMulti;
        int cur_cho, cur_jung, cur_jong;
        int cur_jaeum, cur_moeum;
        jamotype *jamo;

        /* Bind Chosung, Jungsung, Jongsung lists */
        cur_cho = cur_jung = cur_jong = 0;
        Chosung  = PyList_New(NCHOSUNG);
        Jungsung = PyList_New(NJUNGSUNG);
        Jongsung = PyList_New(NJONGSUNG);
        PyDict_SetItemString(d, "Chosung", Chosung);
        PyDict_SetItemString(d, "Jungsung", Jungsung);
        PyDict_SetItemString(d, "Jongsung", Jongsung);
        jamo_jongsung[cur_jong] = NULL;
        Py_INCREF(UniNull);
        PyList_SET_ITEM(Jongsung, cur_jong++, UniNull);

        /* Create Jaeum and Moeum meta class */
        JaeumDict = PyDict_New();
        MoeumDict = PyDict_New();
        tmp = PyString_FromString("Jaeum");
        Jaeum = PyClass_New(NULL, JaeumDict, tmp);
        Py_DECREF(tmp);
        tmp = PyString_FromString("Moeum");
        Moeum = PyClass_New(NULL, MoeumDict, tmp);
        Py_DECREF(tmp);

        /* Bind meta class members */
        PyDict_SetItemString(d, "Jaeum", Jaeum);
        PyDict_SetItemString(d, "Moeum", Moeum);
        PyDict_SetItemString(JaeumDict, "Chosung", Chosung);
        PyDict_SetItemString(MoeumDict, "Jungsung", Jungsung);
        PyDict_SetItemString(JaeumDict, "Jongsung", Jongsung);

        /* Create Jaeum and Moeum Members */
        JaeumCodes = PyTuple_New(NJAEUM);
        MoeumCodes = PyTuple_New(NMOEUM);
        JaeumMulti = PyDict_New();
        MoeumMulti = PyDict_New();
        cur_jaeum = cur_moeum = 0;
        PyDict_SetItemString(JaeumDict, "Codes", JaeumCodes);
        PyDict_SetItemString(MoeumDict, "Codes", MoeumCodes);
        PyDict_SetItemString(JaeumDict, "Width", PyInt_FromLong(NJAEUM));
        PyDict_SetItemString(MoeumDict, "Width", PyInt_FromLong(NMOEUM));
        PyDict_SetItemString(JaeumDict, "MultiElement", JaeumMulti);
        PyDict_SetItemString(MoeumDict, "MultiElement", MoeumMulti);

        for (jamo = jamos; jamo->name; jamo++) {
            PyObject *unijamo, *multicls;
            int tuplen;

            tuni[0] = jamo->code;
            unijamo = PyUnicode_FromUnicode(tuni, 1);
            PyDict_SetItemString(d, jamo->name, unijamo);

            if (isJaeum(jamo->code)) {
                PyTuple_SET_ITEM(JaeumCodes, cur_jaeum++, unijamo);
                Py_INCREF(unijamo);
                if (isChosung(jamo->code)) {
                    jamo->orders[0] = cur_cho;
                    jamo_chosung[cur_cho] = jamo;
                    PyList_SET_ITEM(Chosung,  cur_cho++, unijamo);
                    Py_INCREF(unijamo);
                    PyDict_SetItemString(JaeumDict, jamo->name, unijamo);
                }
                if (isJongsung(jamo->code)) {
                    jamo->orders[2] = cur_jong;
                    jamo_jongsung[cur_jong] = jamo;
                    PyList_SET_ITEM(Jongsung, cur_jong++, unijamo);
                    Py_INCREF(unijamo);
                    PyDict_SetItemString(JaeumDict, jamo->name, unijamo);
                }
                multicls = JaeumMulti;
            }
            else { /* Moeum */
                PyTuple_SET_ITEM(MoeumCodes, cur_moeum++, unijamo);
                Py_INCREF(unijamo);
                if (isJungsung(jamo->code)) {
                    jamo->orders[1] = cur_jung;
                    jamo_jungsung[cur_jung] = jamo;
                    PyList_SET_ITEM(Jungsung, cur_jung++, unijamo);
                    Py_INCREF(unijamo);
                    PyDict_SetItemString(MoeumDict, jamo->name, unijamo);
                }
                multicls = MoeumMulti;
            }
            if (jamo->multi[0]) {
                tuplen = jamo->multi[2] ? 3 : 2;
                tmp = PyTuple_New(tuplen);
                for (i = 0; i < tuplen; i++) {
                    tuni[0] = jamo->multi[i];
                    PyTuple_SET_ITEM(tmp, i, PyUnicode_FromUnicode(tuni, 1));
                }
                PyDict_SetItem(multicls, unijamo, tmp);
                Py_DECREF(tmp);
            }
            Py_DECREF(unijamo);
        }

        Py_DECREF(Chosung); Py_DECREF(Jungsung); Py_DECREF(Jongsung);
        Py_DECREF(JaeumDict);  Py_DECREF(MoeumDict);
        Py_DECREF(JaeumCodes); Py_DECREF(MoeumCodes);
        Py_DECREF(JaeumMulti); Py_DECREF(MoeumMulti);
    }

    tmp = PyTuple_New(2);
    tuni[0] = HANGUL_BOTTOM;
    PyTuple_SET_ITEM(tmp, 0, PyUnicode_FromUnicode(tuni, 1));
    tuni[0] = HANGUL_TOP;
    PyTuple_SET_ITEM(tmp, 1, PyUnicode_FromUnicode(tuni, 1));
    PyDict_SetItemString(d, "ZONE", tmp);
    Py_DECREF(tmp);

    tuni[0] = JBASE_CHOSUNG;
    PyDict_SetItemString(d, "JBASE_CHOSUNG", PyUnicode_FromUnicode(tuni, 1));
    tuni[0] = JBASE_JUNGSUNG;
    PyDict_SetItemString(d, "JBASE_JUNGSUNG", PyUnicode_FromUnicode(tuni, 1));
    tuni[0] = JBASE_JONGSUNG;
    PyDict_SetItemString(d, "JBASE_JONGSUNG", PyUnicode_FromUnicode(tuni, 1));
    tuni[0] = CHOSUNG_FILLER;
    PyDict_SetItemString(d, "CHOSUNG_FILLER", PyUnicode_FromUnicode(tuni, 1));
    tuni[0] = JUNGSUNG_FILLER;
    PyDict_SetItemString(d, "JUNGSUNG_FILLER", PyUnicode_FromUnicode(tuni, 1));
    PyDict_SetItemString(d, "Null", UniNull);
    PyDict_SetItemString(d, "Space", UniSpace);

    PyDict_SetItemString(d, "version", PyString_FromString(version));

    ErrorObject = PyErr_NewException("hangul.UnicodeHangulError", NULL, NULL);
    PyDict_SetItemString(d, "UnicodeHangulError", ErrorObject);
    Py_DECREF(ErrorObject);

    /* Check for errors */
    if (PyErr_Occurred())
        Py_FatalError("can't initialize the hangul module");
}
