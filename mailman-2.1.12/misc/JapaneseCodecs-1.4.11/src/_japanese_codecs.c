/*
 *  _japanese_codecs.c
 *  Tamito KAJIYAMA <24 September 2001>
 *
 *  ACKNOWLEDGMENTS: Part of this program is based on ms932cocecs.c
 *  written by Atsuo ISHIOMOTO.
 */

static char *version =
"$Id: _japanese_codecs.c,v 1.12 2003/11/29 23:19:15 kajiyama Exp $";

#include "Python.h"

#include "_japanese_codecs.h"

static PyObject *ErrorObject;

/* Helper functions */

static
PyObject *codec_tuple(PyObject *unicode, int len)
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

enum { error_strict, error_ignore, error_replace, error_undef };

int error_type(const char *errors) 
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

static int
lookup_jis_map(unsigned char *jis_map[],
               unsigned short c,
               Py_UNICODE *p)
{
  register unsigned char *t = jis_map[c % N];
  register unsigned char key = c / N;
  register int i;

  for (i = *t++; i > 0; i--) {
    if (*t == key) {
      *p = (*(t+1) << 8) + *(t+2);
      return 1;
    }
    t += 3;
  }
  return 0;
}

static int
lookup_ucs_map(unsigned char *ucs_map[],
               Py_UNICODE c,
               unsigned char *p)
{
  register unsigned char *t = ucs_map[c % N];
  register unsigned char key = c / N;
  register int i;

  for (i = *t++; i > 0; i--) {
    if (*t == key) {
      *p++ = *(t+1);
      *p   = *(t+2);
      return 1;
    }
    t += 3;
  }
  return 0;
}

/* Encoder and decoder for EUC-JP */

static char _japanese_codecs_euc_jp_encode__doc__[] = "";

static PyObject *encode_euc_jp(const Py_UNICODE *, int, const char *);

static PyObject *
_japanese_codecs_euc_jp_encode(PyObject *self, PyObject *args)
{
  PyObject *str, *v;
  const char *errors = NULL;

  if (!PyArg_ParseTuple(args, "O|z:_japanese_codecs_euc_jp_encode",
                        &str, &errors))
    return NULL;

  str = PyUnicode_FromObject(str);
  if (str == NULL)
    return NULL;
  v = codec_tuple(encode_euc_jp(PyUnicode_AS_UNICODE(str),
                                PyUnicode_GET_SIZE(str),
                                errors),
                  PyUnicode_GET_SIZE(str));
  Py_DECREF(str);
  return v;
}

static PyObject *
encode_euc_jp(const Py_UNICODE *s, int size, const char *errors)
{
  PyObject *v;
  unsigned char *p, *buf;
  const Py_UNICODE *end;
  int errtype;

  errtype = error_type(errors);
  if (errtype == error_undef)
    return NULL;

  v = PyString_FromStringAndSize(NULL, size * 3);
  if (v == NULL)
    return NULL;
  if (size == 0)
    return v;

  p = buf = PyString_AS_STRING(v);
  end = s + size;
  while (s < end) {
    /* ASCII */
    if (*s < 0x80) {
      *p++ = *s++;
    }
    else if (*s == 0xa5) { /* YEN SIGN */
      *p++ = '\\';
      s++;
    }
    else if (*s == 0x203e) { /* OVERLINE */
      *p++ = '~';
      s++;
    }
    /* JIS X 0208 */
    else if (lookup_ucs_map(jisx0208_ucs_map, *s, p)) {
      p += 2;
      s++;
    }
    /* JIS X 0201 Katakana */
    else if (*s >= 0xff61 && *s <= 0xff9f) {
      *p++ = 0x8e;
      *p++ = *s - 0xfec0;
      s++;
    }
    /* JIS X 0212 Kanji Supplement */
    else if (lookup_ucs_map(jisx0212_ucs_map, *s, p+1)) {
      *p = 0x8f;
      p += 3;
      s++;
    }
    else if (errtype == error_strict) {
      PyObject *e = PyUnicode_EncodeUnicodeEscape(s, 1);
      PyErr_Format(PyExc_UnicodeError, "EUC-JP encoding error: "
                   "invalid character %s", PyString_AS_STRING(e));
      Py_DECREF(e);
      goto onError;
    }
    else if (errtype == error_replace) {
      *p++ = 0xa2;
      *p++ = 0xae;
      s++;
    }
    else if (errtype == error_ignore) {
      s++;
    }
  }

  if (_PyString_Resize(&v, (int)(p - buf)))
    goto onError;

  return v;

onError:
  Py_DECREF(v);
  return NULL;
}

static char _japanese_codecs_euc_jp_decode__doc__[] = "";

static PyObject *
decode_euc_jp(unsigned char *, int, const char *);

static PyObject *
_japanese_codecs_euc_jp_decode(PyObject *self, PyObject *args)
{
  unsigned char *s;
  int size;
  const char *errors = NULL;

  if (!PyArg_ParseTuple(args, "t#|z:_japanese_codecs_euc_jp_decode",
                        &s, &size, &errors))
    return NULL;

  return codec_tuple(decode_euc_jp(s, size, errors), size);
}

static PyObject *
decode_euc_jp(unsigned char *s, int size, const char *errors)
{
  PyObject *v;
  unsigned char *end;
  Py_UNICODE *p;
  int errtype;

  errtype = error_type(errors);
  if (errtype == error_undef)
    return NULL;

  v = PyUnicode_FromUnicode(NULL, size * 2);
  if (v == NULL)
    return NULL;
  if (size == 0)
    return v;

  p = PyUnicode_AS_UNICODE(v);
  end = s + size;
  while (s < end) {
    /* ASCII */
    if (*s < 0x80) {
      *p++ = *s++;
    }
    /* JIS X 0201 Katakana */
    else if (*s == 0x8e) {
      if (s + 1 < end && *(s+1) >= 0xa1 && *(s+1) <= 0xdf) {
        *p++ = *(s+1) + 0xfec0;
        s += 2;
      }
      else if (errtype == error_strict) {
        if (s + 1 < end) {
          PyErr_Format(PyExc_UnicodeError, "EUC-JP decoding error: "
                       "invalid character 0x%02x in JIS X 0201", *(s+1));
        } else {
          PyErr_Format(PyExc_UnicodeError, "EUC-JP decoding error: "
                       "truncated string");
        }
        goto onError;
      }
      else if (errtype == error_replace) {
        *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
        s += 2;
      }
      else if (errtype == error_ignore) {
        s += 2;
      }
    }
    /* JIS X 0212 Kanji Supplement */
    else if (*s == 0x8f) {
      if (s + 2 < end &&
          lookup_jis_map(jisx0212_jis_map, (*(s+1) << 8) + *(s+2), p)) {
        p++;
        s += 3;
      }
      else if (errtype == error_strict) {
        if (s + 2 < end) {
          PyErr_Format(PyExc_UnicodeError, "EUC-JP decoding error: "
                       "invalid character 0x%02x%02x in JIS X 0212",
                       *(s+1), *(s+2));
        } else {
          PyErr_Format(PyExc_UnicodeError, "EUC-JP decoding error: "
                       "truncated string");
        }
        goto onError;
      }
      else if (errtype == error_replace) {
        *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
        s += 3;
      }
      else if (errtype == error_ignore) {
        s += 3;
      }
    }
    /* JIS X 0208 */
    else if (s + 1 < end &&
             lookup_jis_map(jisx0208_jis_map, (*s << 8) + *(s+1), p)) {
      p++;
      s += 2;
    }
    else if (errtype == error_strict) {
      if (s + 1 < end) {
        PyErr_Format(PyExc_UnicodeError, "EUC-JP decoding error: "
                     "invalid character 0x%02x%02x in JIS X 0208",
                     *s, *(s+1));
      } else {
        PyErr_Format(PyExc_UnicodeError, "EUC-JP decoding error: "
                     "truncated string");
      }
      goto onError;
    }
    else if (errtype == error_replace) {
      *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
      s += 2;
    }
    else if (errtype == error_ignore) {
      s += 2;
    }
  }

  if (PyUnicode_Resize(&v, (int)(p - PyUnicode_AS_UNICODE(v))))
    goto onError;

  return v;

onError:
  Py_DECREF(v);
  return NULL;
}

/* Encoder and decoder for Shift_JIS */

static char _japanese_codecs_shift_jis_encode__doc__[] = "";

static PyObject *encode_shift_jis(const Py_UNICODE *, int, const char *);

static PyObject *
_japanese_codecs_shift_jis_encode(PyObject *self, PyObject *args)
{
  PyObject *str, *v;
  const char *errors = NULL;

  if (!PyArg_ParseTuple(args, "O|z:_japanese_codecs_shift_jis_encode",
                        &str, &errors))
    return NULL;

  str = PyUnicode_FromObject(str);
  if (str == NULL)
    return NULL;
  v = codec_tuple(encode_shift_jis(PyUnicode_AS_UNICODE(str),
                                   PyUnicode_GET_SIZE(str),
                                   errors),
                  PyUnicode_GET_SIZE(str));
  Py_DECREF(str);
  return v;
}

static PyObject *
encode_shift_jis(const Py_UNICODE *s, int size, const char *errors)
{
  PyObject *v;
  unsigned char *p, *buf;
  const Py_UNICODE *end;
  int errtype;

  errtype = error_type(errors);
  if (errtype == error_undef)
    return NULL;

  v = PyString_FromStringAndSize(NULL, size * 2);
  if (v == NULL)
    return NULL;
  if (size == 0)
    return v;

  p = buf = PyString_AS_STRING(v);
  end = s + size;
  while (s < end) {
    /* ASCII */
    if (*s < 0x80) {
      *p++ = *s++;
    }
    else if (*s == 0xa5) { /* YEN SIGN */
      *p++ = '\\';
      s++;
    }
    else if (*s == 0x203e) { /* OVERLINE */
      *p++ = '~';
      s++;
    }
    /* JIS X 0208 */
    else if (lookup_ucs_map(jisx0208_ucs_map, *s, p)) {
      if (*p & 1) {
        *p = *p / 2 + ((*p < 0xdf) ? 0x31 : 0x71);
        *(p+1) -= ((*(p+1) < 0xe0) ? 0x61 : 0x60);
      } else {
        *p = *p / 2 + ((*p < 0xdf) ? 0x30 : 0x70);
        *(p+1) -= 2;
      }
      p += 2;
      s++;
    }
    /* JIS X 0201 Katakana */
    else if (*s >= 0xff61 && *s <= 0xff9f) {
      *p++ = *s - 0xfec0;
      s++;
    }
    else if (errtype == error_strict) {
      PyObject *e = PyUnicode_EncodeUnicodeEscape(s, 1);
      PyErr_Format(PyExc_UnicodeError, "Shift_JIS encoding error: "
                   "invalid character %s", PyString_AS_STRING(e));
      Py_DECREF(e);
      goto onError;
    }
    else if (errtype == error_replace) {
      *p++ = 0x81;
      *p++ = 0xac;
      s++;
    }
    else if (errtype == error_ignore) {
      s++;
    }
  }

  if (_PyString_Resize(&v, (int)(p - buf)))
    goto onError;

  return v;

onError:
  Py_DECREF(v);
  return NULL;
}

static char _japanese_codecs_shift_jis_decode__doc__[] = "";

#define SJIS2EUC(c1, c2) \
        (((c2) < 0x9f) ? ((((c1) * 2 - (((c1) < 0xe0) ? 0x61 : 0xe1)) << 8) \
                          + (c2) + (((c2) < 0x7f) ? 0x61 : 0x60))           \
                       : ((((c1) * 2 - (((c1) < 0xe0) ? 0x60 : 0xe0)) << 8) \
                          + (c2) + 2))

static PyObject *
decode_shift_jis(unsigned char *, int, const char *);

static PyObject *
_japanese_codecs_shift_jis_decode(PyObject *self, PyObject *args)
{
  unsigned char *s;
  int size;
  const char *errors = NULL;

  if (!PyArg_ParseTuple(args, "t#|z:_japanese_codecs_shift_jis_decode",
                        &s, &size, &errors))
    return NULL;

  return codec_tuple(decode_shift_jis(s, size, errors), size);
}

static PyObject *
decode_shift_jis(unsigned char *s, int size, const char *errors)
{
  PyObject *v;
  unsigned char *end;
  Py_UNICODE *p;
  int errtype;

  errtype = error_type(errors);
  if (errtype == error_undef)
    return NULL;

  v = PyUnicode_FromUnicode(NULL, size * 2);
  if (v == NULL)
    return NULL;
  if (size == 0)
    return v;

  p = PyUnicode_AS_UNICODE(v);
  end = s + size;
  while (s < end) {
    /* ASCII */
    if (*s < 0x80) {
      *p++ = *s++;
    }
    /* JIS X 0201 Katakana */
    else if (*s >= 0xa1 && *s <= 0xdf) {
      *p++ = *s + 0xfec0;
      s += 1;
    }
    /* JIS X 0208 */
    else if (s + 1 < end &&
             ((0x81 <= *s && *s <= 0x9f) ||
              (0xe0 <= *s && *s <= 0xfc)) &&
             ((0x40 <= *(s+1) && *(s+1) <= 0x7e) ||
              (0x80 <= *(s+1) && *(s+1) <= 0xfc)) &&
             lookup_jis_map(jisx0208_jis_map, SJIS2EUC(*s, *(s+1)), p)) {
      p++;
      s += 2;
    }
    else if (errtype == error_strict) {
      if (s + 1 < end) {
        PyErr_Format(PyExc_UnicodeError, "Shift_JIS decoding error: "
                     "invalid character 0x%02x%02x", *s, *(s+1));
      } else {
        PyErr_Format(PyExc_UnicodeError, "Shift_JIS decoding error: "
                     "truncated string");
      }
      goto onError;
    }
    else if (errtype == error_replace) {
      *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
      s += 2;
    }
    else if (errtype == error_ignore) {
      s += 2;
    }
  }

  if (PyUnicode_Resize(&v, (int)(p - PyUnicode_AS_UNICODE(v))))
    goto onError;

  return v;

onError:
  Py_DECREF(v);
  return NULL;
}

/* Encoder and decoder for MS932 */

static char _japanese_codecs_ms932_encode__doc__[] = "";

static PyObject *encode_ms932(const Py_UNICODE *, int, const char *);

static PyObject *
_japanese_codecs_ms932_encode(PyObject *self, PyObject *args)
{
  PyObject *str, *v;
  const char *errors = NULL;

  if (!PyArg_ParseTuple(args, "O|z:_japanese_codecs_ms932_encode",
                        &str, &errors))
    return NULL;

  str = PyUnicode_FromObject(str);
  if (str == NULL)
    return NULL;
  v = codec_tuple(encode_ms932(PyUnicode_AS_UNICODE(str),
                               PyUnicode_GET_SIZE(str),
                               errors),
                  PyUnicode_GET_SIZE(str));
  Py_DECREF(str);
  return v;
}

static PyObject *
encode_ms932(const Py_UNICODE *s, int size, const char *errors)
{
  PyObject *v;
  unsigned char *p, *buf;
  const Py_UNICODE *end;
  int errtype;

  errtype = error_type(errors);
  if (errtype == error_undef)
    return NULL;

  v = PyString_FromStringAndSize(NULL, size * 2);
  if (v == NULL)
    return NULL;
  if (size == 0)
    return v;

  p = buf = PyString_AS_STRING(v);
  end = s + size;
  while (s < end) {
    /* ASCII */
    if (*s < 0x80) {
      *p++ = *s++;
    }
    else if (*s == 0xa5) { /* YEN SIGN */
      *p++ = '\\';
      s++;
    }
    else if (*s == 0x203e) { /* OVERLINE */
      *p++ = '~';
      s++;
    }
    /* MS932 */
    else if (lookup_ucs_map(ms932_ucs_map, *s, p)) {
      if (!*p) {
        *p = *(p+1);
        p += 1;
      }
      else {
        p += 2;
      }
      s++;
    }
    /* JIS X 0208 */
    else if (lookup_ucs_map(jisx0208_ucs_map, *s, p)) {
      if (*p & 1) {
        *p = *p / 2 + ((*p < 0xdf) ? 0x31 : 0x71);
        *(p+1) -= ((*(p+1) < 0xe0) ? 0x61 : 0x60);
      } else {
        *p = *p / 2 + ((*p < 0xdf) ? 0x30 : 0x70);
        *(p+1) -= 2;
      }
      p += 2;
      s++;
    }
    /* JIS X 0201 Katakana */
    else if (*s >= 0xff61 && *s <= 0xff9f) {
      *p++ = *s - 0xfec0;
      s++;
    }
    else if (errtype == error_strict) {
      PyObject *e = PyUnicode_EncodeUnicodeEscape(s, 1);
      PyErr_Format(PyExc_UnicodeError, "MS932 encoding error: "
                   "invalid character %s", PyString_AS_STRING(e));
      Py_DECREF(e);
      goto onError;
    }
    else if (errtype == error_replace) {
      *p++ = 0x81;
      *p++ = 0xac;
      s++;
    }
    else if (errtype == error_ignore) {
      s++;
    }
  }

  if (_PyString_Resize(&v, (int)(p - buf)))
    goto onError;

  return v;

onError:
  Py_DECREF(v);
  return NULL;
}

static char _japanese_codecs_ms932_decode__doc__[] = "";

#define SJIS2EUC(c1, c2) \
        (((c2) < 0x9f) ? ((((c1) * 2 - (((c1) < 0xe0) ? 0x61 : 0xe1)) << 8) \
                          + (c2) + (((c2) < 0x7f) ? 0x61 : 0x60))           \
                       : ((((c1) * 2 - (((c1) < 0xe0) ? 0x60 : 0xe0)) << 8) \
                          + (c2) + 2))

static PyObject *
decode_ms932(unsigned char *, int, const char *);

static PyObject *
_japanese_codecs_ms932_decode(PyObject *self, PyObject *args)
{
  unsigned char *s;
  int size;
  const char *errors = NULL;

  if (!PyArg_ParseTuple(args, "t#|z:_japanese_codecs_ms932_decode",
                        &s, &size, &errors))
    return NULL;

  return codec_tuple(decode_ms932(s, size, errors), size);
}

static PyObject *
decode_ms932(unsigned char *s, int size, const char *errors)
{
  PyObject *v;
  unsigned char *end;
  Py_UNICODE *p;
  int errtype;

  errtype = error_type(errors);
  if (errtype == error_undef)
    return NULL;

  v = PyUnicode_FromUnicode(NULL, size * 2);
  if (v == NULL)
    return NULL;
  if (size == 0)
    return v;

  p = PyUnicode_AS_UNICODE(v);
  end = s + size;
  while (s < end) {
    /* ASCII */
    if (*s < 0x80) {
      *p++ = *s++;
    }
    /* JIS X 0201 Katakana */
    else if (*s >= 0xa1 && *s <= 0xdf) {
      *p++ = *s + 0xfec0;
      s += 1;
    }
    /* MS932 */
    else if (s + 1 < end &&
             lookup_jis_map(ms932_jis_map, *s << 8 | (*(s+1)), p)) {
      p++;
      s += 2;
    }
    /* JIS X 0208 */
    else if (s + 1 < end &&
             ((0x81 <= *s && *s <= 0x9f) ||
              (0xe0 <= *s && *s <= 0xfc)) &&
             ((0x40 <= *(s+1) && *(s+1) <= 0x7e) ||
              (0x80 <= *(s+1) && *(s+1) <= 0xfc)) &&
             lookup_jis_map(jisx0208_jis_map, SJIS2EUC(*s, *(s+1)), p)) {
      p++;
      s += 2;
    }
    else if (errtype == error_strict) {
      if (s + 1 < end) {
        PyErr_Format(PyExc_UnicodeError, "MS932 decoding error: "
                     "invalid character 0x%02x%02x", *s, *(s+1));
      } else {
        PyErr_Format(PyExc_UnicodeError, "MS932 decoding error: "
                     "truncated string");
      }
      goto onError;
    }
    else if (errtype == error_replace) {
      *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
      s += 2;
    }
    else if (errtype == error_ignore) {
      s += 2;
    }
  }

  if (PyUnicode_Resize(&v, (int)(p - PyUnicode_AS_UNICODE(v))))
    goto onError;

  return v;

onError:
  Py_DECREF(v);
  return NULL;
}

/* Encoder and decoder for ISO-2022-JP */

enum {
  US_ASCII = 0,
  JISX0208_1983,
  JISX0208_1978,
  JISX0201_KATAKANA,
  JISX0201_ROMAN,
  JISX0212_1990,
  DESIGNATIONS
};

typedef struct {
  char *str;
  int len;
} designation_t;

static designation_t designations[] = {
  {"\033(B", 3},  /* US_ASCII */
  {"\033$B", 3},  /* JISX0208_1983 */
  {"\033$@", 3},  /* JISX0208_1978 */
  {"\033(I", 3},  /* JISX0201_KATAKANA */
  {"\033(J", 3},  /* JISX0201_ROMAN */
  {"\033$(D", 4}, /* JISX0212_1990 */
};

static char _japanese_codecs_iso_2022_jp_encode__doc__[] = "";

static PyObject *encode_iso_2022_jp(const Py_UNICODE *, int, const char *);

static PyObject *
_japanese_codecs_iso_2022_jp_encode(PyObject *self, PyObject *args)
{
  PyObject *str, *v;
  const char *errors = NULL;

  if (!PyArg_ParseTuple(args, "O|z:_japanese_codecs_iso_2022_jp_encode",
                        &str, &errors))
    return NULL;

  str = PyUnicode_FromObject(str);
  if (str == NULL)
    return NULL;
  v = codec_tuple(encode_iso_2022_jp(PyUnicode_AS_UNICODE(str),
                                     PyUnicode_GET_SIZE(str),
                                     errors),
                  PyUnicode_GET_SIZE(str));
  Py_DECREF(str);
  return v;
}

static PyObject *
encode_iso_2022_jp(const Py_UNICODE *s, int nchars, const char *errors)
{
  PyObject *v;
  unsigned char *p, *buf, ch[2];
  const Py_UNICODE *end;
  int nbytes, bufsize, m, n = 0;
  int errtype, charset, new_charset = US_ASCII;
  designation_t *d;

  errtype = error_type(errors);
  if (errtype == error_undef)
    return NULL;
  if (nchars == 0)
    return PyString_FromStringAndSize(NULL, 0);

  bufsize = (nchars < 512) ? 1024 : nchars * 2;
  buf = (unsigned char *)malloc(bufsize);
  if (buf == NULL)
    return PyErr_NoMemory();

  charset = US_ASCII;
  nbytes = 0;
  p = buf;
  end = s + nchars;
  while (s < end) {
    /* ASCII */
    if (*s < 0x80) {
      new_charset = US_ASCII;
      ch[0] = *s++;
      n = 1;
    }
    /* JIS X 0201 Roman */
    else if (*s == 0xa5) {  /* YEN SIGN */
      new_charset = JISX0201_ROMAN;
      ch[0] = 0x5c;
      n = 1;
      s++;
    }
    else if (*s == 0x203e) {  /* OVERLINE */
      new_charset = JISX0201_ROMAN;
      ch[0] = 0x7e;
      n = 1;
      s++;
    }
    /* JIS X 0208 */
    else if (lookup_ucs_map(jisx0208_ucs_map, *s, ch)) {
      new_charset = JISX0208_1983;
      ch[0] &= 0x7f;
      ch[1] &= 0x7f;
      n = 2;
      s++;
    }
    else if (errtype == error_strict) {
      PyObject *e = PyUnicode_EncodeUnicodeEscape(s, 1);
      PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP encoding error: "
                   "invalid character %s", PyString_AS_STRING(e));
      Py_DECREF(e);
      goto onError;
    }
    else if (errtype == error_replace) {
      new_charset = JISX0208_1983;
      ch[0] = 0x22;
      ch[1] = 0x2e;
      n = 2;
      s++;
    }
    else if (errtype == error_ignore) {
      s++;
      continue;
    }
    if (charset != new_charset) {
      charset = new_charset;
      d = designations + charset;
      m = d->len;
    } else {
      d = NULL;
      m = 0;
    }
    if (nbytes + m + n >= bufsize) {
      bufsize *= 2;
      buf = (unsigned char *)realloc(buf, bufsize);
      if (buf == NULL)
        return PyErr_NoMemory();
      p = buf + nbytes;
    }
    if (d) {
      strncpy(p, d->str, m);
      p += m;
      nbytes += m;
    }
    strncpy(p, ch, n);
    p += n;
    nbytes += n;
  }
  if (charset != US_ASCII) {
    d = designations; /* US_ASCII */
    m = d->len;
    if (nbytes + m >= bufsize) {
      bufsize = nbytes + m;
      buf = (unsigned char *)realloc(buf, bufsize);
      if (buf == NULL)
        return PyErr_NoMemory();
      p = buf + nbytes;
    }
    strncpy(p, d->str, m);
    p += m;
    nbytes += m;
  }

  v = PyString_FromStringAndSize(buf, nbytes);
  free(buf);
  return v;

onError:
  free(buf);
  return NULL;
}

static char _japanese_codecs_iso_2022_jp_decode__doc__[] = "";

static PyObject *
decode_iso_2022_jp(unsigned char *, int, const char *);

static PyObject *
_japanese_codecs_iso_2022_jp_decode(PyObject *self, PyObject *args)
{
  unsigned char *s;
  int size;
  const char *errors = NULL;

  if (!PyArg_ParseTuple(args, "t#|z:_japanese_codecs_iso_2022_jp_decode",
                        &s, &size, &errors))
    return NULL;

  return codec_tuple(decode_iso_2022_jp(s, size, errors), size);
}

static PyObject *
decode_iso_2022_jp(unsigned char *s, int size, const char *errors)
{
  PyObject *v;
  unsigned char *end;
  Py_UNICODE *p;
  int errtype, charset;
  designation_t *d;

  errtype = error_type(errors);
  if (errtype == error_undef)
    return NULL;

  v = PyUnicode_FromUnicode(NULL, size * 2);
  if (v == NULL)
    return NULL;
  if (size == 0)
    return v;

  charset = US_ASCII;
  p = PyUnicode_AS_UNICODE(v);
  end = s + size;
  while (s < end) {
    if (*s == 0x1b) {
      for (charset = US_ASCII, d = designations;
           charset < DESIGNATIONS;
           charset++, d++) {
        if (s + d->len <= end && strncmp(s, d->str, d->len) == 0) {
          s += d->len;
          break;
        }
      }
      switch (charset) {
      case US_ASCII:
      case JISX0208_1983:
      case JISX0208_1978:
      case JISX0201_ROMAN:
        continue;
      default:
        PyErr_Format(PyExc_UnicodeError,
                     "ISO-2022-JP decoding error: invalid designation");
        goto onError;
      }
    }
    switch (charset) {
    case US_ASCII:
      *p++ = *s++; break;
    case JISX0208_1983:
    case JISX0208_1978:
      if (s + 1 < end &&
          lookup_jis_map(jisx0208_jis_map, (*s << 8) | *(s+1) | 0x8080, p)) {
        p++;
        s += 2;
      }
      else if (errtype == error_strict) {
        if (s + 1 < end) {
          PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP decoding error: "
                       "invalid character 0x%02x%02x in JIS X 0208",
                       *s, *(s+1));
        } else {
          PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP decoding error: "
                       "truncated string");
        }
        goto onError;
      }
      else if (errtype == error_replace) {
        *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
        s += 2;
      }
      else if (errtype == error_ignore) {
        s += 2;
      }
      break;
    case JISX0201_ROMAN:
      if (*s < 0x80) {
        switch (*s) {
        case 0x5c:
          *p++ = 0xa5; s++; break;  /* YEN SIGN */
        case 0x7e:
          *p++ = 0x203e; s++; break;  /* OVERLINE */
        default:
          *p++ = *s++;
        }
      }
      else if (errtype == error_strict) {
        PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP decoding error: "
                     "invalid character 0x%02x in JIS X 0201 Roman", *s);
        goto onError;
      }
      else if (errtype == error_replace) {
        *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
        s++;
      }
      else if (errtype == error_ignore) {
        s++;
      }
    }
  }

  if (PyUnicode_Resize(&v, (int)(p - PyUnicode_AS_UNICODE(v))))
    goto onError;

  return v;

onError:
  Py_DECREF(v);
  return NULL;
}

/* Encoder and decoder for ISO-2022-JP-1 */

static char _japanese_codecs_iso_2022_jp_1_encode__doc__[] = "";

static PyObject *encode_iso_2022_jp_1(const Py_UNICODE *, int, const char *);

static PyObject *
_japanese_codecs_iso_2022_jp_1_encode(PyObject *self, PyObject *args)
{
  PyObject *str, *v;
  const char *errors = NULL;

  if (!PyArg_ParseTuple(args, "O|z:_japanese_codecs_iso_2022_jp_1_encode",
                        &str, &errors))
    return NULL;

  str = PyUnicode_FromObject(str);
  if (str == NULL)
    return NULL;
  v = codec_tuple(encode_iso_2022_jp_1(PyUnicode_AS_UNICODE(str),
                                       PyUnicode_GET_SIZE(str),
                                       errors),
                  PyUnicode_GET_SIZE(str));
  Py_DECREF(str);
  return v;
}

static PyObject *
encode_iso_2022_jp_1(const Py_UNICODE *s, int nchars, const char *errors)
{
  PyObject *v;
  unsigned char *p, *buf, ch[2];
  const Py_UNICODE *end;
  int nbytes, bufsize, m, n = 0;
  int errtype, charset, new_charset = US_ASCII;
  designation_t *d;

  errtype = error_type(errors);
  if (errtype == error_undef)
    return NULL;
  if (nchars == 0)
    return PyString_FromStringAndSize(NULL, 0);

  bufsize = (nchars < 512) ? 1024 : nchars * 2;
  buf = (unsigned char *)malloc(bufsize);
  if (buf == NULL)
    return PyErr_NoMemory();

  charset = US_ASCII;
  nbytes = 0;
  p = buf;
  end = s + nchars;
  while (s < end) {
    /* ASCII */
    if (*s < 0x80) {
      new_charset = US_ASCII;
      ch[0] = *s++;
      n = 1;
    }
    /* JIS X 0201 Roman */
    else if (*s == 0xa5) {  /* YEN SIGN */
      new_charset = JISX0201_ROMAN;
      ch[0] = 0x5c;
      n = 1;
      s++;
    }
    else if (*s == 0x203e) {  /* OVERLINE */
      new_charset = JISX0201_ROMAN;
      ch[0] = 0x7e;
      n = 1;
      s++;
    }
    /* JIS X 0208 */
    else if (lookup_ucs_map(jisx0208_ucs_map, *s, ch)) {
      new_charset = JISX0208_1983;
      ch[0] &= 0x7f;
      ch[1] &= 0x7f;
      n = 2;
      s++;
    }
    /* JIS X 0212 */
    else if (lookup_ucs_map(jisx0212_ucs_map, *s, ch)) {
      new_charset = JISX0212_1990;
      ch[0] &= 0x7f;
      ch[1] &= 0x7f;
      n = 2;
      s++;
    }
    else if (errtype == error_strict) {
      PyObject *e = PyUnicode_EncodeUnicodeEscape(s, 1);
      PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP-1 encoding error: "
                   "invalid character %s", PyString_AS_STRING(e));
      Py_DECREF(e);
      goto onError;
    }
    else if (errtype == error_replace) {
      new_charset = JISX0208_1983;
      ch[0] = 0x22;
      ch[1] = 0x2e;
      n = 2;
      s++;
    }
    else if (errtype == error_ignore) {
      s++;
      continue;
    }
    if (charset != new_charset) {
      charset = new_charset;
      d = designations + charset;
      m = d->len;
    } else {
      d = NULL;
      m = 0;
    }
    if (nbytes + m + n >= bufsize) {
      bufsize *= 2;
      buf = (unsigned char *)realloc(buf, bufsize);
      if (buf == NULL)
        return PyErr_NoMemory();
      p = buf + nbytes;
    }
    if (d) {
      strncpy(p, d->str, m);
      p += m;
      nbytes += m;
    }
    strncpy(p, ch, n);
    p += n;
    nbytes += n;
  }
  if (charset != US_ASCII) {
    d = designations; /* US_ASCII */
    m = d->len;
    if (nbytes + m >= bufsize) {
      bufsize = nbytes + m;
      buf = (unsigned char *)realloc(buf, bufsize);
      if (buf == NULL)
        return PyErr_NoMemory();
      p = buf + nbytes;
    }
    strncpy(p, d->str, m);
    p += m;
    nbytes += m;
  }

  v = PyString_FromStringAndSize(buf, nbytes);
  free(buf);
  return v;

onError:
  free(buf);
  return NULL;
}

static char _japanese_codecs_iso_2022_jp_1_decode__doc__[] = "";

static PyObject *
decode_iso_2022_jp_1(unsigned char *, int, const char *);

static PyObject *
_japanese_codecs_iso_2022_jp_1_decode(PyObject *self, PyObject *args)
{
  unsigned char *s;
  int size;
  const char *errors = NULL;

  if (!PyArg_ParseTuple(args, "t#|z:_japanese_codecs_iso_2022_jp_1_decode",
                        &s, &size, &errors))
    return NULL;

  return codec_tuple(decode_iso_2022_jp_1(s, size, errors), size);
}

static PyObject *
decode_iso_2022_jp_1(unsigned char *s, int size, const char *errors)
{
  PyObject *v;
  unsigned char *end;
  Py_UNICODE *p;
  int errtype, charset;
  designation_t *d;

  errtype = error_type(errors);
  if (errtype == error_undef)
    return NULL;

  v = PyUnicode_FromUnicode(NULL, size * 2);
  if (v == NULL)
    return NULL;
  if (size == 0)
    return v;

  charset = US_ASCII;
  p = PyUnicode_AS_UNICODE(v);
  end = s + size;
  while (s < end) {
    if (*s == 0x1b) {
      for (charset = US_ASCII, d = designations;
           charset < DESIGNATIONS;
           charset++, d++) {
        if (s + d->len <= end && strncmp(s, d->str, d->len) == 0) {
          s += d->len;
          break;
        }
      }
      switch (charset) {
      case US_ASCII:
      case JISX0208_1983:
      case JISX0208_1978:
      case JISX0201_ROMAN:
      case JISX0212_1990:
        continue;
      default:
        PyErr_Format(PyExc_UnicodeError,
                     "ISO-2022-JP-1 decoding error: invalid designation");
        goto onError;
      }
    }
    switch (charset) {
    case US_ASCII:
      *p++ = *s++; break;
    case JISX0208_1978:
    case JISX0208_1983:
      if (s + 1 < end &&
          lookup_jis_map(jisx0208_jis_map, (*s << 8) | *(s+1) | 0x8080, p)) {
        p++;
        s += 2;
      }
      else if (errtype == error_strict) {
        if (s + 1 < end) {
          PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP-1 decoding error: "
                       "invalid character 0x%02x%02x in JIS X 0208",
                       *s, *(s+1));
        } else {
          PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP-1 decoding error: "
                       "truncated string");
        }
        goto onError;
      }
      else if (errtype == error_replace) {
        *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
        s += 2;
      }
      else if (errtype == error_ignore) {
        s += 2;
      }
      break;
    case JISX0201_ROMAN:
      if (*s < 0x80) {
        switch (*s) {
        case 0x5c:
          *p++ = 0xa5; s++; break;  /* YEN SIGN */
        case 0x7e:
          *p++ = 0x203e; s++; break;  /* OVERLINE */
        default:
          *p++ = *s++;
        }
      }
      else if (errtype == error_strict) {
        PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP-1 decoding error: "
                     "invalid character 0x%02x in JIS X 0201 Roman", *s);
        goto onError;
      }
      else if (errtype == error_replace) {
        *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
        s++;
      }
      else if (errtype == error_ignore) {
        s++;
      }
      break;
    case JISX0212_1990:
      if (s + 1 < end &&
          lookup_jis_map(jisx0212_jis_map, (*s << 8) | *(s+1) | 0x8080, p)) {
        p++;
        s += 2;
      }
      else if (errtype == error_strict) {
        if (s + 1 < end) {
          PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP-1 decoding error: "
                       "invalid character 0x%02x%02x in JIS X 0212",
                       *s, *(s+1));
        } else {
          PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP-1 decoding error: "
                       "truncated string");
        }
        goto onError;
      }
      else if (errtype == error_replace) {
        *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
        s += 2;
      }
      else if (errtype == error_ignore) {
        s += 2;
      }
    }
  }

  if (PyUnicode_Resize(&v, (int)(p - PyUnicode_AS_UNICODE(v))))
    goto onError;

  return v;
	
onError:
  Py_DECREF(v);
  return NULL;
}

/* Encoder and decoder for ISO-2022-JP-EXT */

static char _japanese_codecs_iso_2022_jp_ext_encode__doc__[] = "";

static PyObject *encode_iso_2022_jp_ext(const Py_UNICODE *, int, const char *);

static PyObject *
_japanese_codecs_iso_2022_jp_ext_encode(PyObject *self, PyObject *args)
{
  PyObject *str, *v;
  const char *errors = NULL;

  if (!PyArg_ParseTuple(args, "O|z:_japanese_codecs_iso_2022_jp_ext_encode",
                        &str, &errors))
    return NULL;

  str = PyUnicode_FromObject(str);
  if (str == NULL)
    return NULL;
  v = codec_tuple(encode_iso_2022_jp_ext(PyUnicode_AS_UNICODE(str),
                                         PyUnicode_GET_SIZE(str),
                                         errors),
                  PyUnicode_GET_SIZE(str));
  Py_DECREF(str);
  return v;
}

static PyObject *
encode_iso_2022_jp_ext(const Py_UNICODE *s, int nchars, const char *errors)
{
  PyObject *v;
  unsigned char *p, *buf, ch[2];
  const Py_UNICODE *end;
  int nbytes, bufsize, m, n = 0;
  int errtype, charset, new_charset = US_ASCII;
  designation_t *d;

  errtype = error_type(errors);
  if (errtype == error_undef)
    return NULL;
  if (nchars == 0)
    return PyString_FromStringAndSize(NULL, 0);

  bufsize = (nchars < 512) ? 1024 : nchars * 2;
  buf = (unsigned char *)malloc(bufsize);
  if (buf == NULL)
    return PyErr_NoMemory();

  charset = US_ASCII;
  nbytes = 0;
  p = buf;
  end = s + nchars;
  while (s < end) {
    /* ASCII */
    if (*s < 0x80) {
      new_charset = US_ASCII;
      ch[0] = *s++;
      n = 1;
    }
    /* JIS X 0201 Roman */
    else if (*s == 0xa5) {  /* YEN SIGN */
      new_charset = JISX0201_ROMAN;
      ch[0] = 0x5c;
      n = 1;
      s++;
    }
    else if (*s == 0x203e) {  /* OVERLINE */
      new_charset = JISX0201_ROMAN;
      ch[0] = 0x7e;
      n = 1;
      s++;
    }
    /* JIS X 0201 Katakana */
    else if (*s >= 0xff61 && *s <= 0xff9f) {
      new_charset = JISX0201_KATAKANA;
      ch[0] = *s - 0xff40;
      n = 1;
      s++;
    }
    /* JIS X 0208 */
    else if (lookup_ucs_map(jisx0208_ucs_map, *s, ch)) {
      new_charset = JISX0208_1983;
      ch[0] &= 0x7f;
      ch[1] &= 0x7f;
      n = 2;
      s++;
    }
    /* JIS X 0212 */
    else if (lookup_ucs_map(jisx0212_ucs_map, *s, ch)) {
      new_charset = JISX0212_1990;
      ch[0] &= 0x7f;
      ch[1] &= 0x7f;
      n = 2;
      s++;
    }
    else if (errtype == error_strict) {
      PyObject *e = PyUnicode_EncodeUnicodeEscape(s, 1);
      PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP-EXT encoding error: "
                   "invalid character %s", PyString_AS_STRING(e));
      Py_DECREF(e);
      goto onError;
    }
    else if (errtype == error_replace) {
      new_charset = JISX0208_1983;
      ch[0] = 0x22;
      ch[1] = 0x2e;
      n = 2;
      s++;
    }
    else if (errtype == error_ignore) {
      s++;
      continue;
    }
    if (charset != new_charset) {
      charset = new_charset;
      d = designations + charset;
      m = d->len;
    } else {
      d = NULL;
      m = 0;
    }
    if (nbytes + m + n >= bufsize) {
      bufsize *= 2;
      buf = (unsigned char *)realloc(buf, bufsize);
      if (buf == NULL)
        return PyErr_NoMemory();
      p = buf + nbytes;
    }
    if (d) {
      strncpy(p, d->str, m);
      p += m;
      nbytes += m;
    }
    strncpy(p, ch, n);
    p += n;
    nbytes += n;
  }
  if (charset != US_ASCII) {
    d = designations; /* US_ASCII */
    m = d->len;
    if (nbytes + m >= bufsize) {
      bufsize = nbytes + m;
      buf = (unsigned char *)realloc(buf, bufsize);
      if (buf == NULL)
        return PyErr_NoMemory();
      p = buf + nbytes;
    }
    strncpy(p, d->str, m);
    p += m;
    nbytes += m;
  }

  v = PyString_FromStringAndSize(buf, nbytes);
  free(buf);
  return v;

onError:
  free(buf);
  return NULL;
}

static char _japanese_codecs_iso_2022_jp_ext_decode__doc__[] = "";

static PyObject *
decode_iso_2022_jp_ext(unsigned char *, int, const char *);

static PyObject *
_japanese_codecs_iso_2022_jp_ext_decode(PyObject *self, PyObject *args)
{
  unsigned char *s;
  int size;
  const char *errors = NULL;

  if (!PyArg_ParseTuple(args, "t#|z:_japanese_codecs_iso_2022_jp_ext_decode",
                        &s, &size, &errors))
    return NULL;

  return codec_tuple(decode_iso_2022_jp_ext(s, size, errors), size);
}

static PyObject *
decode_iso_2022_jp_ext(unsigned char *s, int size, const char *errors)
{
  PyObject *v;
  unsigned char *end;
  Py_UNICODE *p;
  int errtype, charset;
  designation_t *d;

  errtype = error_type(errors);
  if (errtype == error_undef)
    return NULL;

  v = PyUnicode_FromUnicode(NULL, size * 2);
  if (v == NULL)
    return NULL;
  if (size == 0)
    return v;

  charset = US_ASCII;
  p = PyUnicode_AS_UNICODE(v);
  end = s + size;
  while (s < end) {
    if (*s == 0x1b) {
      for (charset = US_ASCII, d = designations;
           charset < DESIGNATIONS;
           charset++, d++) {
        if (s + d->len <= end && strncmp(s, d->str, d->len) == 0) {
          s += d->len;
          break;
        }
      }
      switch (charset) {
      case US_ASCII:
      case JISX0208_1983:
      case JISX0208_1978:
      case JISX0201_KATAKANA:
      case JISX0201_ROMAN:
      case JISX0212_1990:
        continue;
      default:
        PyErr_Format(PyExc_UnicodeError,
                     "ISO-2022-JP-EXT decoding error: invalid designation");
        goto onError;
      }
    }
    switch (charset) {
    case US_ASCII:
      *p++ = *s++; break;
    case JISX0208_1978:
    case JISX0208_1983:
      if (s + 1 < end &&
          lookup_jis_map(jisx0208_jis_map, (*s << 8) | *(s+1) | 0x8080, p)) {
        p++;
        s += 2;
      }
      else if (errtype == error_strict) {
        if (s + 1 < end) {
          PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP-EXT decoding error: "
                       "invalid character 0x%02x%02x in JIS X 0208",
                       *s, *(s+1));
        } else {
          PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP-EXT decoding error: "
                       "truncated string");
        }
        goto onError;
      }
      else if (errtype == error_replace) {
        *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
        s += 2;
      }
      else if (errtype == error_ignore) {
        s += 2;
      }
      break;
    case JISX0201_KATAKANA:
      if (*s >= 0x21 && *s <= 0x5f) {
        *p++ = *s + 0xff40;
        s++;
      }
      else if (*s <= 0x20 || *s == 0x7f) {
        *p++ = *s++;
      }
      else if (errtype == error_strict) {
        PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP-EXT decoding error: "
                     "invalid character 0x%02x in JIS X 0201 Katakana", *s);
        goto onError;
      }
      else if (errtype == error_replace) {
        *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
        s++;
      }
      else if (errtype == error_ignore) {
        s++;
      }
      break;
    case JISX0201_ROMAN:
      if (*s < 0x80) {
        switch (*s) {
        case 0x5c:
          *p++ = 0xa5; s++; break;  /* YEN SIGN */
        case 0x7e:
          *p++ = 0x203e; s++; break;  /* OVERLINE */
        default:
          *p++ = *s++;
        }
      }
      else if (errtype == error_strict) {
        PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP-EXT decoding error: "
                     "invalid character 0x%02x in JIS X 0201 Roman", *s);
        goto onError;
      }
      else if (errtype == error_replace) {
        *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
        s++;
      }
      else if (errtype == error_ignore) {
        s++;
      }
      break;
    case JISX0212_1990:
      if (s + 1 < end &&
          lookup_jis_map(jisx0212_jis_map, (*s << 8) | *(s+1) | 0x8080, p)) {
        p++;
        s += 2;
      }
      else if (errtype == error_strict) {
        if (s + 1 < end) {
          PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP-EXT decoding error: "
                       "invalid character 0x%02x%02x in JIS X 0212",
                       *s, *(s+1));
        } else {
          PyErr_Format(PyExc_UnicodeError, "ISO-2022-JP-EXT decoding error: "
                       "truncated string");
        }
        goto onError;
      }
      else if (errtype == error_replace) {
        *p++ = Py_UNICODE_REPLACEMENT_CHARACTER;
        s += 2;
      }
      else if (errtype == error_ignore) {
        s += 2;
      }
    }
  }

  if (PyUnicode_Resize(&v, (int)(p - PyUnicode_AS_UNICODE(v))))
    goto onError;

  return v;

onError:
  Py_DECREF(v);
  return NULL;
}

/* List of methods defined in the module */

#define meth(name, func, doc) {name, (PyCFunction)func, METH_VARARGS, doc}

static struct PyMethodDef _japanese_codecs_methods[] = {
  meth("euc_jp_encode",
       _japanese_codecs_euc_jp_encode,
       _japanese_codecs_euc_jp_encode__doc__),
  meth("euc_jp_decode",
       _japanese_codecs_euc_jp_decode,
       _japanese_codecs_euc_jp_decode__doc__),
  meth("shift_jis_encode",
       _japanese_codecs_shift_jis_encode,
       _japanese_codecs_shift_jis_encode__doc__),
  meth("shift_jis_decode",
       _japanese_codecs_shift_jis_decode,
       _japanese_codecs_shift_jis_decode__doc__),
  meth("ms932_encode",
       _japanese_codecs_ms932_encode,
       _japanese_codecs_ms932_encode__doc__),
  meth("ms932_decode",
       _japanese_codecs_ms932_decode,
       _japanese_codecs_ms932_decode__doc__),
  meth("iso_2022_jp_encode",
       _japanese_codecs_iso_2022_jp_encode,
       _japanese_codecs_iso_2022_jp_encode__doc__),
  meth("iso_2022_jp_decode",
       _japanese_codecs_iso_2022_jp_decode,
       _japanese_codecs_iso_2022_jp_decode__doc__),
  meth("iso_2022_jp_1_encode",
       _japanese_codecs_iso_2022_jp_1_encode,
       _japanese_codecs_iso_2022_jp_1_encode__doc__),
  meth("iso_2022_jp_1_decode",
       _japanese_codecs_iso_2022_jp_1_decode,
       _japanese_codecs_iso_2022_jp_1_decode__doc__),
  meth("iso_2022_jp_ext_encode",
       _japanese_codecs_iso_2022_jp_ext_encode,
       _japanese_codecs_iso_2022_jp_ext_encode__doc__),
  meth("iso_2022_jp_ext_decode",
       _japanese_codecs_iso_2022_jp_ext_decode,
       _japanese_codecs_iso_2022_jp_ext_decode__doc__),
  {NULL, (PyCFunction)NULL, 0, NULL} /* sentinel */
};

/* Initialization function for the module */

static char _japanese_codecs_module_documentation[] = "";

void
init_japanese_codecs(void)
{
  PyObject *m, *d;

  /* Create the module and add the functions */
  m = Py_InitModule4("_japanese_codecs", _japanese_codecs_methods,
                     _japanese_codecs_module_documentation,
                     (PyObject*)NULL, PYTHON_API_VERSION);

  /* Add some symbolic constants to the module */
  d = PyModule_GetDict(m);

  PyDict_SetItemString(d, "version", PyString_FromString(version));

  ErrorObject = PyErr_NewException("_japanese_codecs.error", NULL, NULL);
  PyDict_SetItemString(d, "error", ErrorObject);

  /* Check for errors */
  if (PyErr_Occurred())
    Py_FatalError("can't initialize the _japanese_codecs module");
}
