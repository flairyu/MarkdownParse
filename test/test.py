#!/usr/bin/env python

import subprocess
import re
import glob
import os
import sys
from HTMLParser import HTMLParser
from htmlentitydefs import name2codepoint

significant_attrs = ["alt", "href", "src", "title"]
normalize_whitespace_re = re.compile('\s+')
class MyHTMLParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.last = "starttag"
        self.in_pre = False
        self.output = u""
    def handle_data(self, data):
        if self.in_pre:
            self.output += data
        else:
            data = normalize_whitespace_re.sub(' ', data)
            data_strip = data.strip()
            if (self.last == "ref") and data_strip and data[0] == " ":
                self.output += " "
            self.data_end_in_space_not_empty = (data[-1] == ' ' and data_strip)
            self.output += data_strip
            self.last = "data"
    def handle_endtag(self, tag):
        if tag == "pre":
            self.in_pre = False
        self.output += "</" + tag + ">"
        self.last = "endtag"
    def handle_starttag(self, tag, attrs):
        if tag == "pre":
            self.in_pre = True
        self.output += "<" + tag
        attrs = filter(lambda attr: attr[0] in significant_attrs, attrs)
        if attrs:
            attrs.sort()
            for attr in attrs:
                self.output += " " + attr[0] + "=" + '"' + attr[1] + '"'
        self.output += ">"
        self.last = "starttag"
    def handle_startendtag(self, tag, attrs):
        """Ignore closing tag for self-closing void elements."""
        self.handle_starttag(tag, attrs)
    def handle_entityref(self, name):
        self.add_space_from_last_data()
        self.output += unichr(name2codepoint[name])
        self.last = "ref"
    def handle_charref(self, name):
        self.add_space_from_last_data()
        if name.startswith("x"):
            c = unichr(int(name[1:], 16))
        else:
            c = unichr(int(name))
        self.output += c
        self.last = "ref"
    # Helpers.
    def add_space_from_last_data(self):
        """Maintain the space at: `a <span>b</span>`"""
        if self.last == 'data' and self.data_end_in_space_not_empty:
            self.output += ' '

def normalize_output(html):
    parser = MyHTMLParser()
    parser.feed(html)
    parser.close()
    return parser.output

def remove_extension(filename):
    return os.path.splitext(filename)[0]

def run_tests():
    abspath = os.path.abspath(__file__)
    dname = os.path.dirname(abspath)
    os.chdir(dname)
    
    whitespace_re = re.compile(b'\s+')
    
    results = []
    failures = []
    
    data_dir = 'data/'
    md_files = glob.glob(data_dir + "*.md")
    
    data_names = map(remove_extension, md_files)
    
    for data_name in data_names:
        with open(data_name + '.md', 'r') as input_file, \
             open(data_name + '.out', 'r') as output_file:
            result = subprocess.check_output("../build/test/test", stdin = input_file)
            result = normalize_output(str(result))
            expected_output = output_file.read()
            expected_output = normalize_output(expected_output)
            
            correct = result == expected_output
            results.append(correct)
            if not correct:
                failures.append((data_name, result, expected_output))

    for result in results:
        if result:
            sys.stdout.write(".")
        else:
            sys.stdout.write("F")
            
    if len(failures) > 0:
        print("\n\nFailures\n========")
        for failure in failures:
            print(failure[0])
            print("  "),
            print(failure[1])
            print("  "),
            print(failure[2])
    else:
        print("\n\nAll tests passed!")

if __name__ == "__main__":
    run_tests()
