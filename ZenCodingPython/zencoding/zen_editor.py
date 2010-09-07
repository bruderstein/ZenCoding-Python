'''
High-level editor interface that communicates with underlying editor (like
Espresso, Coda, etc.) or browser.
Basically, you should call <code>set_context(obj)</code> method to
set up undelying editor context before using any other method.

This interface is used by <i>zen_actions.py</i> for performing different
actions like <b>Expand abbreviation</b>

@example
import zen_editor
zen_editor.set_context(obj);
//now you are ready to use editor object
zen_editor.get_selection_range();

@author Sergey Chikuyonok (serge.che@gmail.com)
@link http://chikuyonok.ru
'''

import Npp
import zen_core
import re

class ZenEditor():
	def __init__(self):

		self._editor = Npp.editor
		self._notepad = Npp.notepad
		self.padding_re = re.compile(r"^(\s+)")
		self.syntax_map = { Npp.LANGTYPE.HTML : 'html',
		                    Npp.LANGTYPE.CSS  : 'css',
					        Npp.LANGTYPE.XML  : 'xml'
					      }
					   
	def set_context(self, context):
		"""
		Setup underlying editor context. You should call this method
		<code>before</code> using any Zen Coding action.
		@param context: context object
		"""
		self._context = context


	def get_selection_range(self):
		"""
		Returns character indexes of selected text
		@return: list of start and end indexes
		@example
		start, end = zen_editor.get_selection_range();
		print('%s, %s' % (start, end))
		"""
		return self._editor.getSelectionStart(), self._editor.getSelectionEnd()


	def create_selection(self, start, end=None):
		"""
		Creates selection from <code>start</code> to <code>end</code> character
		indexes. If <code>end</code> is ommited, this method should place caret
		and <code>start</code> index
		@type start: int
		@type end: int
		@example
		zen_editor.create_selection(10, 40)
		# move caret to 15th character
		zen_editor.create_selection(15)
		"""
		if end is None:
		    end = start
		self._editor.setSelection(start, end)

	def get_current_line_range(self):
		"""
		Returns current line's start and end indexes
		@return: list of start and end indexes
		@example
		start, end = zen_editor.get_current_line_range();
		print('%s, %s' % (start, end))
		"""
		start = self._editor.lineFromPosition(self._editor.getSelectionStart())
		end = self._editor.lineFromPosition(self._editor.getSelectionEnd())
		return start,end

	def get_caret_pos(self):
		""" Returns current caret position """
		return self._editor.getCurrentPos()

	def set_caret_pos(self, pos):
		"""
		Set new caret position
		@type pos: int
		"""
		self._editor.gotoPos(pos)

	def get_current_line(self):
		"""
		Returns content of current line
		@return: str
		"""
		return self._editor.getCurLine()

	def replace_content(self, value, start=None, end=None):
		"""
		Replace editor's content or it's part (from <code>start</code> to
		<code>end</code> index). If <code>value</code> contains
		<code>caret_placeholder</code>, the editor will put caret into
		this position. If you skip <code>start</code> and <code>end</code>
		arguments, the whole target's content will be replaced with
		<code>value</code>.

		If you pass <code>start</code> argument only,
		the <code>value</code> will be placed at <code>start</code> string
		index of current content.

		If you pass <code>start</code> and <code>end</code> arguments,
		the corresponding substring of current target's content will be
		replaced with <code>value</code>
		@param value: Content you want to paste
		@type value: str
		@param start: Start index of editor's content
		@type start: int
		@param end: End index of editor's content
		@type end: int
		"""
		
		caret_placeholder = zen_core.get_caret_placeholder()
		realStart = start
		caret_pos = -1
		if realStart is None:
			realStart = 0
		
		line_padding = self.padding_re.search(self._editor.getCurLine())
		if line_padding:
			line_padding = line_padding.group(1)
			value = zen_core.pad_string(value, line_padding)
		
		
		new_pos = value.find(caret_placeholder)
		if new_pos != -1:
			caret_pos = realStart + new_pos
			value = value.replace(caret_placeholder, '')
		else:
			caret_pos = realStart + len(value)
			

			
		if start is None:
		    self._editor.setText(value)
		else:
		    if end is None:
		        self._editor.insertText(start, value)
		    else:
		        self._editor.setTarget(start, end)
		        self._editor.replaceTarget(value)

		if caret_pos != -1:
			self._editor.gotoPos(caret_pos)
		
	

	def get_content(self):
		"""
		Returns editor's content
		@return: str
		"""
		return self._editor.getText()

	def get_syntax(self):
		"""
		Returns current editor's syntax mode
		@return: str
		"""
		
		syntax = self.syntax_map.get(self._notepad.getLangType(), 'html')
		
		# Same language used for XML/XSL/XSD
		if syntax == 'xml':
			if self._notepad.getCurrentFilename()[-4:].lower() == '.xsl':
				syntax = 'xsl'
			elif self._notepad.getCurrentFilename()[-4:].lower() == '.xsd':
				syntax = 'xsd'
				
		return syntax
		
	def get_profile_name(self):
		"""
		Returns current output profile name (@see zen_coding#setup_profile)
		@return {String}
		"""
		return 'xhtml'
   
	def prompt(self, title):
		"""
		Ask user to enter something
		@param title: Dialog title
		@type title: str
		@return: Entered data
		@since: 0.65
		"""
		return self._editor.prompt(title, "Zen Coding - Python")

   
	def get_selection(self):
		"""
		Returns current selection
		@return: str
		@since: 0.65
		"""
		return self._editor.getSelText()
   
	def get_file_path(self):
		"""
		Returns current editor's file path
		@return: str
		@since: 0.65
		"""
		return self._notepad.getCurrentFilename()
	