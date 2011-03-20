#ifndef FUNCITEMMANAGER_H
#define FUNCITEMMANAGER_H


enum KeyModifiers
{
	MODIFIER_NONE  = 0,
	MODIFIER_CTRL  = 1,
	MODIFIER_ALT   = 2,
	MODIFIER_SHIFT = 4
};

class FuncItemManager
{
public:
	FuncItemManager();
	~FuncItemManager();

	int addFunction(const TCHAR *functionName, 
		PFUNCPLUGINCMD function,
		ShortcutKey* shortcutKey = NULL,
		bool initToChecked = false);

	int addFunction(const TCHAR *functionName, 
		PFUNCPLUGINCMD function,
		UCHAR key,
		int modifiers,
		bool initToChecked = false);

	void addSplitter();

	FuncItem* getFuncItems(int *funcCount);

private:
	std::list<FuncItem*> m_funcList;
	FuncItem* m_funcItems;


};


#endif // FUNCITEMMANAGER_H