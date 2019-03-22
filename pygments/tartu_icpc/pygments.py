# -*- coding: utf-8 -*-

import re

from pygments.lexers import CppLexer
from pygments.lexer import RegexLexer, include, bygroups, using, \
	this, inherit, default, words
from pygments.util import get_bool_opt
from pygments.token import Text, Comment, Operator, Keyword, Name, String, \
	Number, Punctuation, Error, Token
from collections import namedtuple

Region = namedtuple('Region', ['start', 'end', 'token'])
REGIONS = [

	Region('/*ly*/', '/*ry*/', Token.RegionYellow),
	Region('/*lp*/', '/*rp*/', Token.RegionPink),
	Region('/*lg*/', '/*rg*/', Token.RegionGreen),
]

__all__ = ['TartuICPCLexer']

class TartuICPCLexer(CppLexer):
	"""
	For C++ source code with additional highlighting.
	"""
	name = 'C++'
	aliases = ['tartu_icpc']
	filenames = ['*.cpp', '*.hpp', '*.c++', '*.h++',
				 '*.cc', '*.hh', '*.cxx', '*.hxx',
				 '*.C', '*.H', '*.cp', '*.CPP']
	mimetypes = ['text/x-c++hdr', 'text/x-c++src']
	priority = 0.1

	def passthrough(self, text, l, r, parent):
		import sys
		if parent == None:
			prev = None
			for ofs, tok, val in super().get_tokens_unprocessed(text[l:r]):
				if prev:
					yield prev

				#sys.stderr.write(f'PACKET "{val}"\n')
				prev = (ofs+l, tok, val)
			if prev:
				ofs, tok, val = prev
				#sys.stderr.write(f'pass_end "{val}"\n')
				if(len(val) >= 2 and val[-2:] == '/*'):
					val = val[:-2]
				#sys.stderr.write(f'pass_end "{val}"\n')
				yield ofs, tok, val
				return
		val = text[l:r]
		#sys.stderr.write(f'pass_end "{val}"\n')
		if(len(val) >= 2 and val[-2:] == '/*'):
			val = val[:-2]
		#sys.stderr.write(f'pass_end "{val}"\n')
		yield l, parent, val

	def _analyse(self, text, l, r, parent):
		import sys
		#sys.stderr.write(f'PIECE {l}..{r}\n')
		#sys.stderr.write(text[l:r])
		for regtype in REGIONS:
			start = text.find(regtype.start, l, r)
			if start == -1:
				continue
			ml = start + len(regtype.start)
			end = text.find(regtype.end, ml, r)
			if end == -1:
				continue
			mr = end + len(regtype.end)
			#sys.stderr.write(f'MATCH {regtype.token} {start}..{mr}\n')
			#sys.stderr.write(f'BEG {text[start:ml]}\n')
			#sys.stderr.write(f'END {text[end:mr]}\n')
			yield from self._analyse(text, l, start+2, parent)
			#yield start, regtype.token, text[start:mr]
			#yield start, regtype.token, text[start:ml]
			yield from self._analyse(text, ml, end, regtype.token)
			#yield end, regtype.token, text[end:mr]
			yield from self._analyse(text, mr, r, parent)
			return
		yield from self.passthrough(text, l, r, parent)

	def get_tokens_unprocessed(self, text):
		yield from self._analyse(text, 0, len(text), None)

from pygments.style import Style
from pygments.token import Keyword, Name, Comment, String, Error, \
	 Number, Operator, Generic, Whitespace

class TartuICPCStyle(Style):
	"""
	The default style (inspired by Emacs 22).
	"""

	background_color = "#ffffff"
	default_style = ""

	styles = {
		Whitespace:                "#bbbbbb",
		Comment:                   "italic #008800",
		Comment.Preproc:           "noitalic",
		Comment.Special:           "noitalic bold",

		Keyword:                   "bold #AA22FF",
		Keyword.Pseudo:            "nobold",
		Keyword.Type:              "bold #00BB00",

		Operator:                  "#666666",
		Operator.Word:             "bold #AA22FF",

		Name.Builtin:              "#AA22FF",
		Name.Function:             "#00A000",
		Name.Class:                "#0000FF",
		Name.Namespace:            "bold #0000FF",
		Name.Exception:            "bold #D2413A",
		Name.Variable:             "#B8860B",
		Name.Constant:             "#880000",
		Name.Label:                "#A0A000",
		Name.Entity:               "bold #999999",
		Name.Attribute:            "#BB4444",
		Name.Tag:                  "bold #008000",
		Name.Decorator:            "#AA22FF",

		String:                    "#BB4444",
		String.Doc:                "italic",
		String.Interpol:           "bold #BB6688",
		String.Escape:             "bold #BB6622",
		String.Regex:              "#BB6688",
		String.Symbol:             "#B8860B",
		String.Other:              "#008000",
		Number:                    "#666666",

		Generic.Heading:           "bold #000080",
		Generic.Subheading:        "bold #800080",
		Generic.Deleted:           "#A00000",
		Generic.Inserted:          "#00A000",
		Generic.Error:             "#FF0000",
		Generic.Emph:              "italic",
		Generic.Strong:            "bold",
		Generic.Prompt:            "bold #000080",
		Generic.Output:            "#888",
		Generic.Traceback:         "#04D",

		Error:                     "border:#FF0000",

		Token.RegionPink:          "bg:#FFB8FF",
		Token.RegionGreen:         "bg:#B8FFB8",
		Token.RegionYellow:        "bg:#FFFFB8",
	}
