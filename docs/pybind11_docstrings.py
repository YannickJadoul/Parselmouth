import re

RE_ARGS_KWARGS = re.compile(r'(?<![*\\])\*args|(?<!\\)\*\*kwargs')


def fix_args_kwargs(app, what, name, obj, options, lines):
	for i, line in enumerate(lines):
		lines[i] = RE_ARGS_KWARGS.sub(lambda m: m.group().replace('*', '\\*'), line)


def setup(app):
	app.connect('autodoc-process-docstring', fix_args_kwargs)
