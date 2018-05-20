import os
import sys
import copy
import re
import pyircbot
import time

from urllib.request import urlopen
from bs4 import BeautifulSoup

if len(sys.argv) < 2:
    print("Port!")
    sys.exit(2)

PORT=int(sys.argv[1])

def load_token(fname):
    path = os.path.expanduser(fname)
    with open(path, 'r') as f:
        token = f.readlines()[0].strip()

    return token

HTTP_REGEX = r'http[s]?://(?:[a-zA-Z]|[0-9]|[$-_@.&+]|[!*\(\),]|(?:%[0-9a-fA-F][0-9a-fA-F]))+'

def getTitle(link):
    try:
        soup = BeautifulSoup(urlopen(link), 'lxml')
        return soup.title.string
    except:
        return None

def onMessage(bot, message):
    if message.command != "PRIVMSG":
        return

    content = message.params[-1]
    links = re.findall(HTTP_REGEX, content)
    if len(links) == 0:
        return

    print("Found {} links".format(len(links)))

    resp = pyircbot.IRCMessage()
    resp.command = "PRIVMSG"
    for link in links:
        print("Processing {}".format(link))
        title = getTitle(link)
        if title is None:
            print("Could not fetch title")
            continue

        print("For link: {}, title: {}".format(link, title))

        codes = {
            'code': chr(0x03),
            'reset': chr(0x0F)
        }
        txt = " {code}04,01[{}]{reset}".format(title, **codes)
        resp.params = [message.params[0], txt]
        print("Sending response: {}".format(str(resp)))
        bot.sendIrcMessage(resp)

def onShutdown(bot):
    print("Shutting down!")

plugin = pyircbot.Plugin()
plugin.name='ircbot links'
plugin.token=load_token('~/ircbot_token')
plugin.onMessage=onMessage
plugin.onShutdown=onShutdown

bot = pyircbot.Bot('127.0.0.1', PORT, plugin)
bot.start()

while bot.isRunning():
    time.sleep(0.1)

bot.wait()
