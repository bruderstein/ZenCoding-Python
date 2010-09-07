import Npp

from zencoding import zen_actions
from zencoding import zen_editor
from zencoding import zen_core
from zencoding import my_zen_settings
from zencoding import stparser

_npp_editor = zen_editor.ZenEditor()
_profile = 'xhtml'

def expand_abbreviation(isTab):
	global _profile
	result = zen_actions.expand_abbreviation(_npp_editor, _npp_editor.get_syntax(), _profile)
	if isTab and result == False:
		Npp.editor.tab()

def wrap_with_abbreviation():
	global _profile
	abbr = Npp.notepad.prompt("Wrap with abbreviation:", "Zen Coding - Python")
	if abbr is not None:
		zen_actions.wrap_with_abbreviation(_npp_editor, abbr, _npp_editor.get_syntax(), _profile)

def next_edit_point():
	zen_actions.next_edit_point(_npp_editor)


def prev_edit_point():
	zen_actions.prev_edit_point(_npp_editor)


def match_pair_inward():
	zen_actions.match_pair_inward(_npp_editor)


def match_pair_outward():
	zen_actions.match_pair_outward(_npp_editor)


def go_to_matching_pair():
	zen_actions.go_to_matching_pair(_npp_editor)



def toggle_comment():
	zen_actions.toggle_comment(_npp_editor)


def split_join_tag():
	zen_actions.split_join_tag(_npp_editor)


def remove_tag():
	zen_actions.remove_tag(_npp_editor)


def update_image_size():
	zen_actions.update_image_size(_npp_editor)

	
def set_profile(profile):
	global _profile
	_profile = profile
	
def update_settings():
	reload(my_zen_settings)
	zen_core.update_settings(stparser.get_settings(my_zen_settings.my_zen_settings))
	