#include <Python.h>
#include <stdio.h>


#define ESCAPED_CHARS_TABLE_SIZE 63

static Py_ssize_t escaped_chars_delta_len[ESCAPED_CHARS_TABLE_SIZE];
static unsigned char *escaped_chars_repl[ESCAPED_CHARS_TABLE_SIZE];
static int inited = 0;

static void init_constants(void) {
    if (inited) {
        return;
    }

    escaped_chars_repl['"'] = (unsigned char*) "&#34;";
    escaped_chars_repl['\'']= (unsigned char*) "&#39;";
    escaped_chars_repl['&'] = (unsigned char*) "&amp;";
    escaped_chars_repl['<'] = (unsigned char*) "&lt;";
    escaped_chars_repl['>'] = (unsigned char*) "&gt;";

    /* lengths of those characters when replaced - 1 */
    memset(escaped_chars_delta_len, 0, sizeof(escaped_chars_delta_len));
    escaped_chars_delta_len['"'] = escaped_chars_delta_len['\''] = \
        escaped_chars_delta_len['&'] = 4;
    escaped_chars_delta_len['<'] = escaped_chars_delta_len['>'] = 3;

    inited = 1;
}

PyObject* do_escape(PyObject* strObj) {
    Py_ssize_t i;
    PyObject *res;
    unsigned char *buf;

    char *buffer;
    unsigned char* incb;
    Py_ssize_t length;

    unsigned char *outp;
    unsigned char *inp;
    unsigned char *next_escp;
    Py_ssize_t delta = 0, erepl = 0, delta_len = 0;
    unsigned char *inp_end;

    //

    buffer = PyString_AsString(strObj);
    if (!buffer) {
        return NULL;
    }
    length = PyString_Size(strObj);
    incb = (unsigned char*)buffer;

    inp_end = incb + length;

    /* ...Do some I/O operation involving buf... */
    for (i = 0; i < length; i++) {
        unsigned char c = incb[i];

        if (c < ESCAPED_CHARS_TABLE_SIZE) {
            delta += escaped_chars_delta_len[c];
            erepl += !!escaped_chars_delta_len[c];
        }
    }

    /* Do we need to escape anything at all? */
    if (!erepl) {
        Py_INCREF(strObj);
        return (PyObject*)strObj;
    }

    buf = PyMem_New(unsigned char, length + delta + 1); /* for I/O */
    buf[length + delta] = '\0';
    if (buf == NULL)
        return PyErr_NoMemory();

    

    outp = &(buf[0]);
    inp = &(incb[0]);
    while (erepl-- > 0) {
        /* look for the next substitution */
        next_escp = inp;
        while (next_escp < inp_end) {
            //printf("next_escp: %d\n", *next_escp);
            if (*next_escp < ESCAPED_CHARS_TABLE_SIZE &&
                (delta_len = escaped_chars_delta_len[*next_escp])) {
                ++delta_len;
                break;
            }
            ++next_escp;
        }

        if (next_escp > inp) {
            /* copy unescaped chars between inp and next_escp */
            memcpy(outp, inp, next_escp - inp);
            outp += next_escp - inp;
        }

        /* escape 'next_escp' */
        memcpy(outp, escaped_chars_repl[*next_escp], delta_len);
        outp += delta_len;

        inp = next_escp + 1;
    }

    if (inp < inp_end) {
        memcpy(outp, inp, inp_end - inp);
    }
    
    res = PyString_FromString((char *) buf);
    PyMem_Del(buf); /* allocated with PyMem_New */
    return res;
}

PyObject* escape_html(PyObject *self, PyObject *obj) {
    PyObject* ret;
    PyObject* strObj;

    int isStr = PyString_CheckExact(obj);
    if (isStr) {
        strObj = obj;
    }
    else {
        // FIXME: should deallocate?
        strObj = PyObject_Str(obj);
        if (strObj == NULL) {
            return NULL;
        }
    }

    ret = do_escape(strObj);

    if (isStr) {
    
    }
    else {
        Py_DECREF(strObj);
    }

    return ret;
}


/* python ext */
static PyMethodDef utf8_escape_methods[] = {
    // The first property is the name exposed to python, the second is the C++ function name        
    { "escape_html", (PyCFunction)escape_html, METH_O, NULL },

    // Terminate the array with an object containing nulls.
    { NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC initutf8_escape(void) {
    init_constants();

    PyObject *module = Py_InitModule("utf8_escape", utf8_escape_methods);
}
