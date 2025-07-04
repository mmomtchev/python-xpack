import sys
import urllib.request

print(sys.path)
contents = urllib.request.urlopen("https://www.duckduckgo.com").read()
