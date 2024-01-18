#include "StdAfx.h"

#if defined(ENABLE_CHANGELOOK)
#include "PythonChangeLook.h"
#include "PythonNetworkStream.h"

CPythonChangeLook::CPythonChangeLook()
{
	Clear();
}

CPythonChangeLook::~CPythonChangeLook()
{
	Clear();
}

void CPythonChangeLook::Clear()
{
	dwCost = 0;

	m_vMaterials.clear();
	m_vMaterials.resize(CPythonChangeLook::WINDOW_MAX_MATERIALS);

	for (BYTE bPos = 0; bPos < m_vMaterials.size(); ++bPos)
	{
		TChangeLookMaterial tMaterial;
		tMaterial.bHere = 0;
		tMaterial.wCell = 0;
		m_vMaterials[bPos] = tMaterial;
	}
}

void CPythonChangeLook::AddMaterial(BYTE bPos, TItemPos tPos)
{
	if (bPos >= CPythonChangeLook::WINDOW_MAX_MATERIALS)
		return;

	TChangeLookMaterial tMaterial;
	tMaterial.bHere = 1;
	tMaterial.wCell = tPos.cell;

	m_vMaterials[bPos] = tMaterial;
}

void CPythonChangeLook::RemoveMaterial(BYTE bPos)
{
	if (bPos >= CPythonChangeLook::WINDOW_MAX_MATERIALS)
		return;

	TChangeLookMaterial tMaterial;
	tMaterial.bHere = 0;
	tMaterial.wCell = 0;

	m_vMaterials[bPos] = tMaterial;
}

void CPythonChangeLook::RemoveAllMaterials()
{
	for (BYTE bPos = 0; bPos < CPythonChangeLook::WINDOW_MAX_MATERIALS; ++bPos)
	{
		TChangeLookMaterial tMaterial;
		tMaterial.bHere = 0;
		tMaterial.wCell = 0;

		m_vMaterials[bPos] = tMaterial;
	}
}

bool CPythonChangeLook::GetAttachedItem(BYTE bPos, BYTE& bHere, WORD& wCell)
{
	if (bPos >= CPythonChangeLook::WINDOW_MAX_MATERIALS)
		return false;

	bHere = m_vMaterials[bPos].bHere;
	wCell = m_vMaterials[bPos].wCell;
	return true;
}

BOOL CPythonChangeLook::IsCleanScroll(DWORD dwItemVnum) {
	switch (dwItemVnum) {
		case 72325: {
			return TRUE;
		}
		default: {
			return FALSE;
		}
	}
}

BOOL CPythonChangeLook::IsTransmutationTicket(DWORD dwItemVnum) {
	switch (dwItemVnum) {
		case 72326: {
			return TRUE;
		}
		default: {
			return FALSE;
		}
	}
}

PyObject* changelookSendCloseRequest(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendChangeLookClosePacket();
	return Py_BuildNone();
}

PyObject* changelookAdd(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bPos;
	TItemPos tPos;

	if (!PyTuple_GetInteger(poArgs, 0, &tPos.window_type))
		return Py_BuildException();
	else if (!PyTuple_GetInteger(poArgs, 1, &tPos.cell))
		return Py_BuildException();
	else if (!PyTuple_GetInteger(poArgs, 2, &bPos))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendChangeLookAddPacket(tPos, bPos);
	return Py_BuildNone();
}

PyObject* changelookRemove(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetInteger(poArgs, 0, &bPos))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendChangeLookRemovePacket(bPos);
	return Py_BuildNone();
}

PyObject* changelookGetCost(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("L", CPythonChangeLook::Instance().GetCost());
}

PyObject* changelookGetAttachedItem(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetInteger(poArgs, 0, &bPos))
		return Py_BuildException();

	BYTE bHere;
	WORD wCell;
	bool bGet = CPythonChangeLook::Instance().GetAttachedItem(bPos, bHere, wCell);
	if (!bGet)
	{
		bHere = 0;
		wCell = 0;
	}

	return Py_BuildValue("ii", bHere, wCell);
}

PyObject* changelookSendRefineRequest(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bHere;
	WORD wCell;
	bool bGet = CPythonChangeLook::Instance().GetAttachedItem(1, bHere, wCell);
	if (bGet)
	{
		if (bHere)
		{
			CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
			rkNetStream.SendChangeLookRefinePacket();
		}
	}

	return Py_BuildNone();
}

PyObject* changelookIsCleanScroll(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonChangeLook::Instance().IsCleanScroll(iIndex));
}

PyObject* changelookIsTransmutationTicket(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonChangeLook::Instance().IsTransmutationTicket(iIndex));
}

void initChangeLook()
{
	static PyMethodDef functions[] = {
		{ "SendCloseRequest", changelookSendCloseRequest, METH_VARARGS },
		{ "Add", changelookAdd, METH_VARARGS },
		{ "Remove", changelookRemove, METH_VARARGS },
		{ "GetCost", changelookGetCost, METH_VARARGS },
		{ "GetAttachedItem", changelookGetAttachedItem, METH_VARARGS },
		{ "SendRefineRequest", changelookSendRefineRequest, METH_VARARGS },
		{ "IsCleanScroll", changelookIsCleanScroll, METH_VARARGS },
		{ "IsTransmutationTicket", changelookIsTransmutationTicket, METH_VARARGS },
		{ NULL, NULL, NULL },
	};

	PyObject* pModule = Py_InitModule("changelook", functions);
	PyModule_AddIntConstant(pModule, "WINDOW_MAX_MATERIALS", CPythonChangeLook::WINDOW_MAX_MATERIALS);
	PyModule_AddIntConstant(pModule, "LIMIT_RANGE", CPythonChangeLook::LIMIT_RANGE);
}
#endif
