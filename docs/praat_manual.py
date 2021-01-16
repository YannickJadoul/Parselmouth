import docutils
import sphinx

import re


PRAAT_DOCS_URL = "https://www.fon.hum.uva.nl/praat/manual/{}.html"


def praat_manual_role(typ, rawtext, text, lineno, inliner, options={}, content=[]):
	text = docutils.utils.unescape(text)
	has_explicit_title, title, manpage = sphinx.util.nodes.split_explicit_title(text)

	url = PRAAT_DOCS_URL.format(re.sub(r'[^A-Za-z0-9_+-]', '_', manpage))  # See ManPages_writeAllToHtmlDir

	if not has_explicit_title:
		title = "Praat: \"{}\"".format(manpage)

	node = docutils.nodes.emphasis(title, title, classes=['praat'])
	# Alternatively, node = docutils.nodes.literal(title, title, classes=['xref', 'praat'])
	refnode = docutils.nodes.reference('', '', internal=False, refuri=url, reftype='ref', reftitle=title)
	refnode.append(node)
	return [refnode], []


def setup(app):
	app.add_role('praat', praat_manual_role)
