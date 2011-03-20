#!/usr/bin/env python
# -*- coding: utf-8 -*-

'''
Filter that alters XSD attributes - adjusted from xsl.py
@author Dave Brotherstone
'''

import re
import zencoding
import Npp
re_attr_id    = re.compile('\\s+id\\s*=\\s*([\'"])(.*)\\1')
re_attr_class = re.compile('\\s+class\\s*=\\s*([\'"])(.*)\\1')


def replace_attrs(item):
	"""
	Replaces the attributes of item so id becomes name
	and class becomes type
	"""
	item.start = re_attr_id.sub(r" name=\1\2\1", item.start)
	item.start = re_attr_class.sub(r" type=\1\2\1", item.start)

@zencoding.filter('xsd')	
def process(tree, profile):
	for item in tree.children:
		if item.type == 'tag':
			replace_attrs(item)
		
		process(item, profile)