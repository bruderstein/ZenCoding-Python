#include "stdafx.h"
#include "PluginInterface.h"

#include "FuncItemManager.h"

FuncItemManager::FuncItemManager()
	: m_funcItems(NULL)
{
}


FuncItemManager::~FuncItemManager()
{
	for(std::list<FuncItem*>::iterator it = m_funcList.begin(); it != m_funcList.end(); ++it)
	{
		if ((*it)->_pShKey != NULL)
		{
			delete (*it)->_pShKey;
		}
		delete (*it);
	}

	if (m_funcItems != NULL)
	{
		delete [] m_funcItems;
	}
}




int FuncItemManager::addFunction(const TCHAR *functionName, 
		PFUNCPLUGINCMD function,
		UCHAR key,
		int modifiers,
		bool initToChecked /* = false */)
{
	ShortcutKey *shkey = new ShortcutKey();
	shkey->_key = key;
	shkey->_isCtrl = (modifiers & MODIFIER_CTRL) == MODIFIER_CTRL;
	shkey->_isAlt = (modifiers & MODIFIER_ALT) == MODIFIER_ALT;
	shkey->_isShift = (modifiers & MODIFIER_SHIFT) == MODIFIER_SHIFT;
	return addFunction(functionName, function, shkey, initToChecked);
}

int FuncItemManager::addFunction(const TCHAR *functionName, 
		PFUNCPLUGINCMD function,
		ShortcutKey* shortcutKey /* = NULL */,
		bool initToChecked /* = false */)
{
	FuncItem *item = new FuncItem();
	_tcscpy_s<64>(item->_itemName, functionName);
	item->_pFunc = function;
	item->_init2Check = initToChecked;
	item->_pShKey = shortcutKey;
	
	m_funcList.push_back(item);
	return m_funcList.size() - 1;
}


void FuncItemManager::addSplitter()
{
	FuncItem *item = new FuncItem();
	item->_itemName[0] = _T('\0');
	item->_pFunc = NULL;
	m_funcList.push_back(item);
}


FuncItem* FuncItemManager::getFuncItems(int *funcCount)
{
	if (m_funcItems != NULL)
	{
		delete [] m_funcItems;
	}
	
	*funcCount = m_funcList.size();
	m_funcItems = new FuncItem[(*funcCount)];
	
	// Copy the list items into the m_funcList. 
	// Sadly this has to make an actual copy of each funcItem, but that's life..

	int pos = 0;
	for(std::list<FuncItem*>::iterator it = m_funcList.begin(); it != m_funcList.end(); ++it, ++pos)
	{
		m_funcItems[pos] = *(*it);
	}

	return m_funcItems;
}

