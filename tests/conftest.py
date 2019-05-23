import pytest

import os
import sys

if '' not in sys.path:
	sys.path.insert(2, '')


def pytest_addoption(parser):
	parser.addoption('--praat-tests', action="store_true", default=False, help="run Praat tests")


def pytest_collection_modifyitems(config, items):
	praat_tests = bool(config.getoption('--praat-tests'))
	for item in items:
		if praat_tests != ('praat' in item.keywords):
			item.add_marker(pytest.mark.skip())


class Resources(object):
	def __init__(self, base_path):
		self.base_path = base_path
	
	def __getitem__(self, file_name):
		return os.path.join(self.base_path, "data", file_name)


@pytest.fixture(scope="module")
def resources(request):
	return Resources(request.fspath.dirname)


from resource_fixtures import *
