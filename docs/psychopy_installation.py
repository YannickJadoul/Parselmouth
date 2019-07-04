import io
import json
import os
import platform
import psychopy
import ssl
import sys
import zipfile
try:
	from urllib.request import urlopen
except ImportError:
	from urllib2 import urlopen



# CHANGE THESE VARIABLES TO CUSTOMIZE THE PARSELMOUTH INSTALLATION

# In which folder should Parselmouth be installed? Default: folder 'parselmouth-psychopy' in the home directory.
INSTALLATION_PATH = os.path.join(os.path.expanduser('~'), 'parselmouth-psychopy')

# Which version of Parselmouth should be installed ('x.y.z')? Default: the latest (None).
PARSELMOUTH_VERSION = None



print("Determining platform and Python version")
if sys.platform == 'darwin':
	platform_tag = 'macosx'
elif sys.platform == 'win32':
	if platform.architecture()[0] == '64bit':
		platform_tag = 'win_amd64'
	elif platform.architecture()[0] == '32bit':
		platform_tag = 'win32'
	else:
		raise Exception("Something went wrong. Cannot detect wether Windows architecture is 32- or 64-bit. Please try the manual approach and report this problem on GitHub.")
else:
	raise Exception("Platform is not macOS or Windows. Install Parselmouth with pip.")

py_version = 'cp' + ''.join(map(str, sys.version_info[:2]))


try:
	ssl_context = ssl.SSLContext()
except:
	ssl_context = ssl.SSLContext(ssl.PROTOCOL_SSLv23)

print("Finding corresponding PyPI download URL for {} & {}".format(platform_tag, py_version))
pypi_data = json.load(urlopen('https://pypi.org/pypi/praat-parselmouth/json', context=ssl_context))

if PARSELMOUTH_VERSION is not None and PARSELMOUTH_VERSION not in pypi_data['releases']:
	raise Exception("Could not find version {} of Parselmouth on PyPI ({})".format(PARSELMOUTH_VERSION, ", ".join(pypi_data['releases'].keys())))

pypi_urls = pypi_data['urls'] if PARSELMOUTH_VERSION is None else pypi_data['releases'][PARSELMOUTH_VERSION]
matching_urls = [url for url in pypi_urls if url['python_version'] == py_version and platform_tag in url['filename']]

if len(matching_urls) == 0:
	raise Exception("Something went wrong. No matching packages found. Please try the manual approach and report this problem on GitHub.")
elif len(matching_urls) > 1:
	raise Exception("Something went wrong. Multiple matching packages found. Please try the manual approach and report this problem on GitHub.")

if os.path.exists(INSTALLATION_PATH):
	raise Exception("Intallation path already exists. Remove the folder \"{}\" before running this script to update an existing installation.".format(INSTALLATION_PATH))
os.mkdir(INSTALLATION_PATH, int('744', 8))

print("Downloading \"{}\" from \"{}\"".format(matching_urls[0]['filename'], matching_urls[0]['url']))
whl = zipfile.ZipFile(io.BytesIO(urlopen(matching_urls[0]['url'], context=ssl_context).read()), 'r')

print("Extracting \"{}\" into \"{}\"".format(matching_urls[0]['filename'], INSTALLATION_PATH))
whl.extractall(INSTALLATION_PATH)


print("Adding \"{}\" to 'paths' list in PsychoPy's 'General' settings".format(INSTALLATION_PATH))
psychopy.prefs.loadUserPrefs()
if INSTALLATION_PATH not in psychopy.prefs.general['paths']:
	psychopy.prefs.general['paths'].append(INSTALLATION_PATH)
psychopy.prefs.saveUserPrefs()


print("Done! Restart PsychoPy to use Parselmouth.")

