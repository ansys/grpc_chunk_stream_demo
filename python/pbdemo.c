#include <Python.h>

static PyMethodDef PodMethods[] = {
  {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
initpodpb(void)
{
  PyObject *m;

  m = Py_InitModule("pbdemo", PodMethods);
  if (m == NULL)
    return;
}
