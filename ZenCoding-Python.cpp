
/*
 ZenCoding Python Plugin
 This code needs tidying up - it should be split out into classes
 - there's waaaay too many global variables (i.e. more than none :)
*/

#include "stdafx.h"
#include "AboutDlg.h"

#define CHECK_INITIALISED()  if (!g_initialised){ initialise(); }




/* Info for Notepad++ */
CONST TCHAR PLUGIN_NAME[]	= _T("Zen Coding - Python");

FuncItem	*funcItem = NULL;

/* Global data */
NppData				nppData;
HANDLE				g_hModule			= NULL;
bool                g_pythonFailure     = false;
bool                g_initialised       = false;
bool				g_expandIsTab		= false;
bool				g_autoSelectProfile = true;
bool				g_watchSave			= false;
TCHAR				g_settingsFile[MAX_PATH];
TCHAR				g_iniPath[MAX_PATH];

int g_fiAutoProfile  = -1;
int g_fiProfileXhtml = -1;
int g_fiProfileHtml  = -1;
int g_fiProfileXml   = -1;
int g_fiProfilePlain = -1;
int g_currentProfileIndex = -1;

/* Dialogs */
AboutDialog		aboutDlg;


// Runs a string in python
void runScript(const TCHAR *script);
void runString(const TCHAR *str, int messageType = PYSCR_EXECSTATEMENT);

// Checks if the key is tab on it's own
bool keyIsTab(const ShortcutKey& key);

// Sets the profile (ticks menu item, and informs Python)
void setProfile(const TCHAR *profileName, int cmdIndex);


// Settings
void loadSettings();
void saveSettings();

void initialise()
{
	TCHAR configPath[MAX_PATH];
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(configPath));
	
	tstring my_zen_settings(configPath);
	my_zen_settings.append(_T("\\ZenCodingPython\\zencoding\\my_zen_settings.py"));

		
	if (!::PathFileExists(my_zen_settings.c_str()))
	{
		std::ofstream mySettingsFile(my_zen_settings.c_str());

		mySettingsFile << "my_zen_settings = {\n"
						  "     'html': {\n"
						  "      	'abbreviations': {\n"
						  "			'jq': '<script type=\"text/javascript\" src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.3.2/jquery.min.js\"></script>',\n"
						  "			'demo': '<div id   =\"demo\"></div>'\n"
						  "		}\n"
						  "   }\n"
						  "}\n";
		mySettingsFile.close();

	}

	tstring zen_module(_T("sys.path.append(r'"));
	zen_module.append(configPath);
	zen_module.append(_T("\\ZenCodingPython')\nimport npp_zen_coding\n"));

	runString(zen_module.c_str());

	g_initialised = true;

	// Set the current profile if it's not xhtml (the default)
	if (g_fiProfileHtml == g_currentProfileIndex)
	{
		setProfile(_T("html"), g_fiProfileHtml);
	}
	else if (g_fiProfileXml == g_currentProfileIndex)
	{
		setProfile(_T("xml"), g_fiProfileXml);
	}
	else if (g_fiProfilePlain == g_currentProfileIndex)
	{
		setProfile(_T("plain"), g_fiProfilePlain);
	}

	

}

void doExpandAbbreviation()
{
	CHECK_INITIALISED();
	if (g_expandIsTab)
	{
		runString(_T("npp_zen_coding.expand_abbreviation(True)"));
	}
	else
	{
		runString(_T("npp_zen_coding.expand_abbreviation(False)"));
	}
}

	

void doWrapWithAbbreviation()
{
	CHECK_INITIALISED();
	runString(_T("npp_zen_coding.wrap_with_abbreviation()"));
}


void doNextEditPoint()
{
	CHECK_INITIALISED();
	runString(_T("npp_zen_coding.next_edit_point()"));
}

void doPreviousEditPoint()
{
	CHECK_INITIALISED();
	runString(_T("npp_zen_coding.prev_edit_point()"));
}

void doMatchPairInward()
{
	CHECK_INITIALISED();
	runString(_T("npp_zen_coding.match_pair_inward()"));
}

void doMatchPairOutward()
{
	CHECK_INITIALISED();
	runString(_T("npp_zen_coding.match_pair_outward()"));
}

void doGoToMatchingPair()
{
	CHECK_INITIALISED();
	runString(_T("npp_zen_coding.go_to_matching_pair()"));
}

void doToggleComment()
{
	CHECK_INITIALISED();
	runString(_T("npp_zen_coding.toggle_comment()"));
}

void doSplitJoinTag()
{
	CHECK_INITIALISED();
	runString(_T("npp_zen_coding.split_join_tag()"));
}

void doRemoveTag()
{
	CHECK_INITIALISED();
	runString(_T("npp_zen_coding.remove_tag()"));
}

void doUpdateImageSize()
{
	CHECK_INITIALISED();
	runString(_T("npp_zen_coding.update_image_size()"));
}


void doAbout()
{
	aboutDlg.doDialog();
}

void doEditSettings()
{
	g_watchSave = true;
	
	
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(g_settingsFile));
	_tcscat_s(g_settingsFile, MAX_PATH, _T("\\ZenCodingPython\\zencoding\\my_zen_settings.py"));

	::SendMessage(nppData._nppHandle, NPPM_DOOPEN, 0, reinterpret_cast<LPARAM>(g_settingsFile));

}

void setProfile(const TCHAR *profileName, int cmdIndex)
{
	
	if (-1 != g_currentProfileIndex)
	{
		::SendMessage(nppData._nppHandle, NPPM_SETMENUITEMCHECK, funcItem[g_currentProfileIndex]._cmdID, FALSE);
	}
	
	::SendMessage(nppData._nppHandle, NPPM_SETMENUITEMCHECK, funcItem[cmdIndex]._cmdID, TRUE);
	
	if (g_initialised)
	{
		TCHAR cmd[150];
		_tcscpy_s(cmd, 150, _T("npp_zen_coding.set_profile('"));
		_tcscat_s(cmd, 150, profileName);
		_tcscat_s(cmd, 150, _T("')"));

		runString(cmd);
	}

	g_currentProfileIndex = cmdIndex;
}

void doProfileAutoSelect()
{
	g_autoSelectProfile = !g_autoSelectProfile;
	::SendMessage(nppData._nppHandle, NPPM_SETMENUITEMCHECK, funcItem[g_fiAutoProfile]._cmdID, g_autoSelectProfile ? TRUE : FALSE);
	saveSettings();
}

void doProfileXhtml()
{
	setProfile(_T("xhtml"), g_fiProfileXhtml);
}

void doProfileHtml()
{
	setProfile(_T("html"), g_fiProfileHtml);
}

void doProfileXml()
{
	setProfile(_T("xml"), g_fiProfileXml);
}

void doProfilePlain()
{
	setProfile(_T("plain"), g_fiProfilePlain);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hModule = hModule;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


extern "C" __declspec(dllexport) void setInfo(NppData notepadPlusData)
{
	nppData = notepadPlusData;
	aboutDlg.init(static_cast<HINSTANCE>(g_hModule), nppData);
	
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, reinterpret_cast<LPARAM>(g_iniPath));
	_tcscat_s(g_iniPath, MAX_PATH, _T("\\ZenCodingPython.ini"));
}


extern "C" __declspec(dllexport) CONST TCHAR * getName()
{
	return PLUGIN_NAME;
}


extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF)
{
	*nbF = 20;
	funcItem = new FuncItem[*nbF];
	
	
	_tcscpy_s<64>(funcItem[0]._itemName, _T("Expand Abbreviation"));
	funcItem[0]._pFunc = doExpandAbbreviation;
	funcItem[0]._init2Check = FALSE;
	funcItem[0]._pShKey = new ShortcutKey();
	funcItem[0]._pShKey->_key = VK_RETURN;
	funcItem[0]._pShKey->_isCtrl = true;
	funcItem[0]._pShKey->_isAlt = true;

	_tcscpy_s<64>(funcItem[1]._itemName, _T("Wrap with abbreviation..."));
	funcItem[1]._pFunc = doWrapWithAbbreviation;
	funcItem[1]._init2Check = FALSE;
	funcItem[1]._pShKey = NULL;


	_tcscpy_s<64>(funcItem[2]._itemName, _T("Next Edit Point"));
	funcItem[2]._pFunc = doNextEditPoint;
	funcItem[2]._init2Check = FALSE;
	funcItem[2]._pShKey = new ShortcutKey();
	funcItem[2]._pShKey->_key = VK_RIGHT;
	funcItem[2]._pShKey->_isCtrl = true;
	funcItem[2]._pShKey->_isAlt = true;

	_tcscpy_s<64>(funcItem[3]._itemName, _T("Previous edit point"));
	funcItem[3]._pFunc = doPreviousEditPoint;
	funcItem[3]._init2Check = FALSE;
	funcItem[3]._pShKey = new ShortcutKey();
	funcItem[3]._pShKey->_key = VK_LEFT;
	funcItem[3]._pShKey->_isCtrl = true;
	funcItem[3]._pShKey->_isAlt = true;


	
	_tcscpy_s<64>(funcItem[4]._itemName, _T("Match pair inward"));
	funcItem[4]._pFunc = doMatchPairInward;
	funcItem[4]._init2Check = FALSE;
	funcItem[4]._pShKey = NULL;
	
	_tcscpy_s<64>(funcItem[5]._itemName, _T("Match pair outward"));
	funcItem[5]._pFunc = doMatchPairOutward;
	funcItem[5]._init2Check = FALSE;
	funcItem[5]._pShKey = NULL;
	
	_tcscpy_s<64>(funcItem[6]._itemName, _T("Go to matching pair"));
	funcItem[6]._pFunc = doGoToMatchingPair;
	funcItem[6]._init2Check = FALSE;
	funcItem[6]._pShKey = NULL;
	

	_tcscpy_s<64>(funcItem[7]._itemName, _T("Toggle Comment"));
	funcItem[7]._pFunc = doToggleComment;
	funcItem[7]._init2Check = FALSE;
	funcItem[7]._pShKey = NULL;
	
	_tcscpy_s<64>(funcItem[8]._itemName, _T("Split / Join tag"));
	funcItem[8]._pFunc = doSplitJoinTag;
	funcItem[8]._init2Check = FALSE;
	funcItem[8]._pShKey = NULL;
	

	_tcscpy_s<64>(funcItem[9]._itemName, _T("Remove tag"));
	funcItem[9]._pFunc = doRemoveTag;
	funcItem[9]._init2Check = FALSE;
	funcItem[9]._pShKey = NULL;
	

	_tcscpy_s<64>(funcItem[10]._itemName, _T("Update image size"));
	funcItem[10]._pFunc = doUpdateImageSize;
	funcItem[10]._init2Check = FALSE;
	funcItem[10]._pShKey = NULL;

	_tcscpy_s<64>(funcItem[11]._itemName, _T("--"));
	funcItem[11]._pFunc = NULL;
	funcItem[11]._init2Check = FALSE;
	funcItem[11]._pShKey = NULL;


	_tcscpy_s<64>(funcItem[12]._itemName, _T("Edit settings"));
	funcItem[12]._pFunc = doEditSettings;
	funcItem[12]._init2Check = FALSE;
	funcItem[12]._pShKey = NULL;
	
	_tcscpy_s<64>(funcItem[13]._itemName, _T("Auto Select Profile"));
	funcItem[13]._pFunc = doProfileAutoSelect;
	funcItem[13]._init2Check = TRUE;
	funcItem[13]._pShKey = NULL;
	g_fiAutoProfile = 13;

	_tcscpy_s<64>(funcItem[14]._itemName, _T("Profile: xhtml"));
	funcItem[14]._pFunc = doProfileXhtml;
	funcItem[14]._init2Check = TRUE;
	funcItem[14]._pShKey = NULL;
	g_fiProfileXhtml = 14;
	g_currentProfileIndex = g_fiProfileXhtml;

	_tcscpy_s<64>(funcItem[15]._itemName, _T("Profile: html"));
	funcItem[15]._pFunc = doProfileHtml;
	funcItem[15]._init2Check = FALSE;
	funcItem[15]._pShKey = NULL;
	g_fiProfileHtml = 15;

	_tcscpy_s<64>(funcItem[16]._itemName, _T("Profile: xml"));
	funcItem[16]._pFunc = doProfileXml;
	funcItem[16]._init2Check = FALSE;
	funcItem[16]._pShKey = NULL;
	g_fiProfileXml = 16;

	_tcscpy_s<64>(funcItem[17]._itemName, _T("Profile: plain"));
	funcItem[17]._pFunc = doProfilePlain;
	funcItem[17]._init2Check = FALSE;
	funcItem[17]._pShKey = NULL;
	g_fiProfilePlain = 17;
	

	_tcscpy_s<64>(funcItem[18]._itemName, _T("--"));
	funcItem[18]._pFunc = NULL;
	funcItem[18]._init2Check = FALSE;
	funcItem[18]._pShKey = NULL;

	_tcscpy_s<64>(funcItem[19]._itemName, _T("About"));
	funcItem[19]._pFunc = doAbout;
	funcItem[19]._init2Check = FALSE;
	funcItem[19]._pShKey = NULL;
	return funcItem;
}




extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{

	switch(notifyCode->nmhdr.code)
	{
		case NPPN_READY:
			{
				ShortcutKey key;
				::SendMessage(nppData._nppHandle, NPPM_GETSHORTCUTBYCMDID, funcItem[0]._cmdID, reinterpret_cast<LPARAM>(&key));
				g_expandIsTab = keyIsTab(key);	
				loadSettings();
				// Lazy initialisation, so don't call initialise() yet
				//initialise();
			}
			break;

		case NPPN_SHORTCUTREMAPPED:
			if (funcItem[0]._cmdID == notifyCode->nmhdr.idFrom)
			{
				g_expandIsTab = keyIsTab(*reinterpret_cast<ShortcutKey*>(notifyCode->nmhdr.hwndFrom));
			}
			break;

		case NPPN_BUFFERACTIVATED:
		case NPPN_LANGCHANGED:
			if (g_autoSelectProfile)
			{
				int lang = ::SendMessage(nppData._nppHandle, NPPM_GETBUFFERLANGTYPE, notifyCode->nmhdr.idFrom, 0);
				switch(lang)
				{
					case L_XML:
						setProfile(_T("xml"), g_fiProfileXml);
						break;

					case L_TXT:
						setProfile(_T("plain"), g_fiProfilePlain);
						break;

					case L_HTML:
					default:
						setProfile(_T("xhtml"), g_fiProfileXhtml);
						break;
				}
			}
			break;
		case NPPN_FILESAVED:
			if (g_watchSave)
			{
				TCHAR filename[MAX_PATH];
				::SendMessage(nppData._nppHandle, NPPM_GETFULLPATHFROMBUFFERID, notifyCode->nmhdr.idFrom, reinterpret_cast<LPARAM>(filename));
				if (0 == _tcsicmp(filename, g_settingsFile))
				{
					if (g_initialised)
					{
						runString(_T("npp_zen_coding.update_settings()"));
						::MessageBox(nppData._nppHandle, _T("Zen Coding settings automatically refreshed"), _T("Zen Coding for Notepad++"), MB_ICONINFORMATION);
					}
					else
					{
						::MessageBox(nppData._nppHandle, _T("New Zen Coding settings have been applied"), _T("Zen Coding for Notepad++"), MB_ICONINFORMATION);
					}
				}
			}
			break;
	}

	
}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT message, WPARAM /* wParam */, LPARAM lParam)
{
	return TRUE;
}


#ifdef _UNICODE
extern "C" __declspec(dllexport) BOOL isUnicode()
{
	return TRUE;
}
#endif


void runScript(TCHAR *str)
{
	runString(str, PYSCR_EXECSCRIPT);
}




void runString(const TCHAR *script, int messageType /* = PYSCR_EXECSTATEMENT */)
{
	if (g_pythonFailure)
		return;

	PythonScript_Exec pse;
	pse.structVersion = 1;

	pse.completedEvent = NULL;
	pse.deliverySuccess = FALSE;
	pse.flags = 0;
	pse.script = script;
	
	

	TCHAR pluginName[] = _T("PythonScript.dll");
	CommunicationInfo commInfo;
	commInfo.internalMsg = messageType;
	commInfo.srcModuleName = PLUGIN_NAME;
	
	commInfo.info = reinterpret_cast<void*>(&pse);
	// SendMessage(nppData._nppHandle, NPPM_MSGTOPLUGIN, reinterpret_cast<WPARAM>(pluginName), reinterpret_cast<LPARAM>(&commInfo));

	BOOL delivery = SendMessage(nppData._nppHandle, NPPM_MSGTOPLUGIN, reinterpret_cast<WPARAM>(pluginName), reinterpret_cast<LPARAM>(&commInfo));
	if (!delivery)
	{
		MessageBox(NULL, _T("Python Script Plugin not found.  Please install the Python Script plugin from Plugin Manager"), _T("Zen Coding - Python"), 0);
		g_pythonFailure = true;
	}	
	else if (!pse.deliverySuccess)
	{
		MessageBox(NULL, _T("Python Script Plugin did not accept the script"), _T("Zen Coding - Python"), 0);
		g_pythonFailure = true;
	}
}

bool keyIsTab(const ShortcutKey& key)
{
	if (key._key == VK_TAB && (key._isAlt == false && key._isCtrl == false && key._isShift == false))
	{
		return true;
	}
	else
	{
		return false;
	}

}


void saveSettings()
{
	::WritePrivateProfileString(_T("Settings"), _T("AutoProfile"), g_autoSelectProfile ? _T("1") : _T("0"), g_iniPath);
}

void loadSettings()
{
	int result = ::GetPrivateProfileInt(_T("Settings"), _T("AutoProfile"), 1, g_iniPath);
	g_autoSelectProfile = (result == 1);

	::SendMessage(nppData._nppHandle, NPPM_SETMENUITEMCHECK, funcItem[g_fiAutoProfile]._cmdID, g_autoSelectProfile ? TRUE : FALSE);
}
