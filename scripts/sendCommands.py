# Copyright (c) 2018 Jeroen Voogd
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import time
import urllib
import httplib2

# prepare the http connection
http = httplib2.Http()

def sendJevoisCommand(command):
    # the URL including the command (note that the spaces in the command need to
    # be converted)
    url = "http://192.168.1.115/set?cmd=" + urllib.quote(command)
    print "sending url=" + url
    try:
        # send the command to the esp8266
        response, content = http.request(url,"GET")
    # some other exception has occurred
    except:
        content=''
    # print the content (if any), see the function 'receive_data' in
    # jevoisEsp8266.ino
    if content:
        print content
    # the jevois camera takes some time to respond to a command
    time.sleep(2)

# these are the actual commands. In this example it starts DemoArUco
sendJevoisCommand("setpar serout None")
sendJevoisCommand('streamoff')
sendJevoisCommand('setmapping2 YUYV 640 480 20.0 JeVois DemoArUco')
sendJevoisCommand("setpar serlog None")
sendJevoisCommand("setpar serstyle Detail")
sendJevoisCommand("setpar serout Hard")
sendJevoisCommand('streamon')
# sendJevoisCommand("info")
# sendJevoisCommand("ping")
