import Npp

import os
import pickle

import zencoding
import zencoding.zen_editor

_npp_editor = zencoding.zen_editor.ZenEditor()

_profile = 'xhtml'
_full_list = {}

_originalSeparator = 32
_initialAutoCompleteLength = 0


def expand_abbreviation(isTab):
	global _profile
	result = zencoding.run_action('expand_abbreviation', _npp_editor, None, _profile)
	if isTab and result == False:
		Npp.editor.tab()

def wrap_with_abbreviation():
	global _profile
	abbr = Npp.notepad.prompt("Wrap with abbreviation:", "Zen Coding - Python")
	if abbr is not None:
		zencoding.run_action('wrap_with_abbreviation', _npp_editor, abbr, None, _profile)

def next_edit_point():
	zencoding.run_action('next_edit_point', _npp_editor)


def prev_edit_point():
	zencoding.run_action('prev_edit_point', _npp_editor)

def select_next_item():
	zencoding.run_action('select_next_item', _npp_editor)

def select_previous_item():
	zencoding.run_action('select_previous_item', _npp_editor)
	

def match_pair_inward():
	zencoding.run_action('match_pair_inward', _npp_editor)


def match_pair_outward():
	zencoding.run_action('match_pair_outward', _npp_editor)


def go_to_matching_pair():
	zencoding.run_action('go_to_matching_pair', _npp_editor)

def merge_lines():
	zencoding.run_action('merge_lines', _npp_editor)
	

def toggle_comment():
	zencoding.run_action('toggle_comment', _npp_editor)


def split_join_tag():
	zencoding.run_action('split_join_tag', _npp_editor)


def remove_tag():
	zencoding.run_action('remove_tag', _npp_editor)


def reflect_css_value():
	zencoding.run_action('reflect_css_value', _npp_editor)

def evaluate_math_expression():
	zencoding.run_action('evaluate_math_expression', _npp_editor)

def increment_number(step):
	zencoding.run_action('increment_number', _npp_editor, step)
	
def update_image_size():
	zencoding.run_action('update_image_size', _npp_editor)

def add_entry(type):
	text = Npp.editor.getSelText()
	if not text:
		Npp.notepad.messageBox('Please highlight some text to use as the {0}'.format(type[:-1]), 'Zen Coding - Python')
		return

	user_vocab = zencoding.resources.get_vocabulary('user')	
	abbrev = Npp.notepad.prompt('Enter {0}:'.format(type[:-1]), 'Zen Coding Add Entry')
	mode = _npp_editor.get_syntax()
	
	if abbrev:
		if mode not in user_vocab:
			user_vocab[mode] = {}
		if type not in user_vocab[mode]:
			user_vocab[mode][type] = {}
		
		user_vocab[mode][type][abbrev] = text
		_create_autocomplete_list()
		try:
			save_user_settings()
			Npp.notepad.messageBox('{0} "{1}" added and saved'.format(type[0].upper() + type[1:-1], abbrev), 'Zen Coding')
		except:
			Npp.notepad.messageBox('{0} "{1}" added but error saving user settings'.format(type[0].upper() + type[1:-1], abbrev), 'Zen Coding')

			
def _get_autocomplete_list_for_lang(lang):
	system_vocab = zencoding.resources.get_vocabulary('system')
	user_vocab = zencoding.resources.get_vocabulary('user')
	lang_list = []
	if lang in system_vocab:
		if 'abbreviations' in system_vocab[lang]:
			lang_list = [x for x in system_vocab[lang]['abbreviations']]
				
		if 'snippets' in system_vocab[lang]:
			lang_list.extend([x for x in system_vocab[lang]['snippets']])
	
			
	if lang in user_vocab:
		if 'abbreviations' in user_vocab[lang]:
			lang_list.extend([x for x in user_vocab[lang]['abbreviations']])
			
		if 'snippets' in user_vocab[lang]:
			lang_list.extend([x for x in user_vocab[lang]['snippets']])
	
	if lang in system_vocab and 'extends' in system_vocab[lang]:
		if type(system_vocab[lang]['extends']).__name__ == 'str':
			system_vocab[lang]['extends'] = system_vocab[lang]['extends'].split(',')
			
		for extendslang in system_vocab[lang]['extends']:
			lang_list.extend(_get_autocomplete_list_for_lang(extendslang))
	
	if lang in user_vocab and 'extends' in user_vocab[lang]:
		if type(user_vocab[lang]['extends']).__name__ == 'str':
			user_vocab[lang]['extends'] = user_vocab[lang]['extends'].split(',')
			
		for extendslang in user_vocab[lang]['extends']:
			if system_vocab[lang]['extends'] and extendslang not in system_vocab[lang]['extends'].split(','):
				lang_list.extend(_get_autocomplete_list_for_lang(extendslang))
	
	return lang_list

	
def _create_autocomplete_list():
	global _full_list
	system_vocab = zencoding.resources.get_vocabulary('system')
	user_vocab = zencoding.resources.get_vocabulary('user')
	
	for lang in system_vocab:
		_full_list[lang] = _get_autocomplete_list_for_lang(lang)
		_full_list[lang].sort()
		
		
	for lang in user_vocab:
		if lang not in system_vocab:
			_full_list[lang] = _get_autocomplete_list_for_lang(lang)
			_full_list[lang].sort()


def _get_user_file():
	return os.path.dirname(os.path.abspath( __file__ )) + '\\user_settings.pickle'
	

def load_user_settings():
	try:
		f = open(_get_user_file(), 'rb')
	except:
		f = None
		pass
	if f:
		user_vocab = pickle.load(f)
		f.close()
		zencoding.resources.set_vocabulary(user_vocab, 'user')
	
	_create_autocomplete_list()
	
	
def save_user_settings():
	user_vocab = zencoding.resources.get_vocabulary('user')
	f = open(_get_user_file(), 'wb')
	pickle.dump(user_vocab, f)
	f.close()

def _get_autocomplete_list(syntax, start):
	startlen = len(start)
	return [x for x in _full_list[syntax] if x[:startlen] == start]


def _get_autocomplete_leader():
	pos = Npp.editor.getCurrentPos()
	lineStart = Npp.editor.positionFromLine(Npp.editor.lineFromPosition(pos))
	begin = ''
	for pos in range(pos - 1, lineStart - 1, -1):
		c = chr(Npp.editor.getCharAt(pos))
		if c in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ:_-':
			begin = c + begin
		else:
			break
	return begin

	
def _handle_selection(args):
	Npp.editor.clearCallbacks(_handle_selection)
	Npp.editor.clearCallbacks(_handle_cancel)
	Npp.editor.clearCallbacks(_handle_charadded)
	
	
def _handle_cancel(args = None):
	Npp.editor.clearCallbacks(_handle_selection)
	Npp.editor.clearCallbacks(_handle_cancel)
	Npp.editor.clearCallbacks(_handle_charadded)
	
	
def _handle_charadded(args):
	_handle_cancel()
	Npp.editor.autoCCancel()
	show_autocomplete()
	
	
	
def show_autocomplete():
	global _originalSeparator, _initialAutoCompleteLength
	
	begin = _get_autocomplete_leader()
	_originalSeparator = Npp.editor.autoCGetSeparator()
	Npp.editor.autoCSetSeparator(ord('\n'))
	_initialAutoCompleteLength = len(begin)
	autolist = _get_autocomplete_list(_npp_editor.get_syntax(), begin)
	
	Npp.editor.autoCSetCancelAtStart(False)
	Npp.editor.autoCSetFillUps(">+{[(")
	Npp.editor.callback(_handle_selection, [Npp.SCINTILLANOTIFICATION.AUTOCSELECTION])
	Npp.editor.callback(_handle_cancel, [Npp.SCINTILLANOTIFICATION.AUTOCCANCELLED])
	Npp.editor.callback(_handle_charadded, [Npp.SCINTILLANOTIFICATION.CHARADDED, Npp.SCINTILLANOTIFICATION.AUTOCCHARDELETED])
	Npp.editor.autoCShow(_initialAutoCompleteLength, "\n".join(autolist))
	
def set_profile(profile):
	global _profile
	_profile = profile
	
def reflect_css_value():
	zencoding.run_action('reflect_css_value', _npp_editor)
	

load_user_settings()
