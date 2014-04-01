/*
 *
 */
#include <Python.h>
#include "structmember.h"
#include "fkw_rbtree_dict.h"
#include "fkw_seg.h"

typedef struct {
	PyObject_HEAD	
	unsigned int hash_size;
	fkw_dict_array_t *dict_array;
} FkwDict;

static PyObject *
FkwDict_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    FkwDict *self;

    self = (FkwDict *)type->tp_alloc(type, 0);

	self->hash_size = 5;
	self->dict_array = NULL;

    return (PyObject *)self;

}

static int 
FkwDict_init(FkwDict *self, PyObject *args, PyObject *kwds)
{
	if (!PyArg_ParseTuple(args, "|I", &self->hash_size)){
		return -1;
	}

	self->dict_array = fkw_dict_array_init(self->hash_size);

	return 0;
}

static void
FkwDict_dealloc(FkwDict *self)
{
	fkw_free_dict(&(self->dict_array));
	self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
FkwDict_load_dict(FkwDict *self, PyObject *args)
{
	const char *fpath;
	if (!PyArg_ParseTuple(args, "s", &fpath)){
		PyErr_SetString(PyExc_TypeError, "load dict takes exactly one argument (0 given)");
		return NULL;	
	}

	if (!fkw_load_dict(self->dict_array, fpath)){
		PyErr_SetString(PyExc_TypeError, "load dict takes exactly one argument (0 given)");
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
FkwDict_add_word(FkwDict *self, PyObject *args)
{
	const char *word;
	const int len;
	if (!PyArg_ParseTuple(args, "s#", &word, &len)){
		PyErr_SetString(PyExc_TypeError, "add word takes exactly one argument (0 given)");
		return NULL;	
	}
	if (!fkw_add_dict_word(self->dict_array, word, strlen(word))){
		PyErr_SetString(PyExc_TypeError, "add word failed");
		return NULL;
	}
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *
FkwDict_search_word(FkwDict *self, PyObject *args)
{
	const char *word;
	fkw_rbtree_node_value_t *node_value;
	fkw_rbtree_node_value_t *tmp;

	if (! PyArg_ParseTuple(args, "s", &word)){
		PyErr_SetString(PyExc_TypeError, "search word takes exactly one argument (0 given)");
		return NULL;	
	}
	
	node_value = fkw_dict_search(self->dict_array, word, strlen(word));
	if (node_value){
		Py_INCREF(Py_True);
		return Py_True;
	} else {
		Py_INCREF(Py_False);
		return Py_False;
	}
}

PyObject *
FkwDict_full_seg(PyObject *self, PyObject *args)
{
	int i;

	PyObject *dict_arg;

	FkwDict *dict;

	fkw_str_t text;

	fkw_word_list_t *word_list;

	fkw_word_t *tmp;

	PyObject* item_list;

	if (!PyArg_ParseTuple(args, "Os#", &dict_arg, &(text.data), &(text.len))){
		PyErr_SetString(PyExc_TypeError, "full_seg takes exactly two argument");
		return NULL;	
	}
	dict = (FkwDict *) dict_arg;

	word_list = fkw_full_seg(&text, dict->dict_array);
	if (! word_list){
		return PyList_New(0);
	}

	i = 0;
    tmp = word_list->start;
    item_list = PyList_New(word_list->num);
	while(tmp){
		PyList_SetItem(item_list, i, Py_BuildValue("s#", tmp->value.data, tmp->value.len));
		tmp = tmp->next;
		i = i + 1;
	}

	fkw_free_word_list(&word_list);
    return item_list;
}

static PyMethodDef FkwDict_methods[] = {
	{
		"load_dict", 
		(PyCFunction)FkwDict_load_dict, 
		METH_VARARGS,
		"load dict from file, one word one line"
	},
	{
		"add_word", 
		(PyCFunction)FkwDict_add_word, 
		METH_VARARGS,
		"add a word to dict"
	},
	{
		"search_word", 
		(PyCFunction)FkwDict_search_word, 
		METH_VARARGS,
		"search word"
	},

	{NULL}  /*  Sentinel */
};


static PyTypeObject FkwDictType = {
	PyObject_HEAD_INIT(NULL)
	0, /* ob_size*/
	"filter_keywords.FkwDict", /* tp_name*/
	sizeof(FkwDict),           /* tp_basicsize*/
	0, /* tp_itemsize*/
	(destructor)FkwDict_dealloc, /* tp_dealloc*/
	0, /* tp_print*/
	0, /* tp_getattr*/
	0, /* tp_setattr*/
	0, /* tp_compare*/
	0, /* tp_repr*/
	0, /* tp_as_number*/
	0, /* tp_as_sequence*/
	0, /* tp_as_mapping*/
	0, /* tp_hash */
	0, /* tp_call*/
	0, /* tp_str*/
	0, /* tp_getattro*/
	0, /* tp_setattro*/
	0, /* tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags*/
	"FkwDict objects",           /*  tp_doc */
	0, /*  tp_traverse */
	0, /*  tp_clear */
	0, /*  tp_richcompare */
	0, /*  tp_weaklistoffset */
	0, /*  tp_iter */
	0, /*  tp_iternext */
	FkwDict_methods, /*  tp_methods */
	0, /*  tp_members */
	0, /*  tp_getset */
	0, /*  tp_base */
	0, /*  tp_dict */
	0, /*  tp_descr_get */
	0, /*  tp_descr_set */
	0, /*  tp_dictoffset */
	(initproc)FkwDict_init, /*  tp_init */
	NULL, /*  tp_alloc */
	FkwDict_new, /*  tp_new */
};

static PyMethodDef filter_keywords_methods[] = {
	{
		"fkw_full_seg", 
		(PyCFunction)FkwDict_full_seg,
		METH_VARARGS,
		"do full seg"
	},

	{
		"filter_keywords", 
		(PyCFunction)FkwDict_full_seg,
		METH_VARARGS,
		"alias to fkw_full_seg, just for fun"
	},

	{NULL}  /*  Sentinel */
};

#ifndef PyMODINIT_FUNC /*  declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
initfilter_keywords(void)
{
	PyObject* m;

    if (PyType_Ready(&FkwDictType) < 0)
        return;

	m = Py_InitModule3("filter_keywords", filter_keywords_methods,
			"create filter keywords extension");

	Py_INCREF(&FkwDictType);
	PyModule_AddObject(m, "FkwDict", (PyObject *)&FkwDictType);
}
