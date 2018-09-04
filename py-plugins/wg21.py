import sys
import os
import time
import json
import pyircbot

from urllib.request import urlopen

if len(sys.argv) < 2:
    print("Port!")
    sys.exit(2)

PORT=int(sys.argv[1])

FNAME='wg21.json'
UPDATE_DATE=None

def getCurrentList():
    LINK = 'https://wg21.link/index.json'
    content = urlopen(LINK).read().decode('utf-8')
    data = json.loads(content)

    with open(FNAME, 'w') as f:
        f.write(content)

    return data

def getSavedList():
    with open(FNAME, 'r') as f:
        content = f.read()

    return json.loads(content)

def getList():
    if not UPDATE_DATE or UPDATE_DATE < int(time.time()) - 3600 * 2:
        UPDATE_DATE = int(time.time())
        return getCurrentList()
    else:
        return getSavedList()

def load_token(fname):
    path = os.path.expanduser(fname)
    with open(path, 'r') as f:
        token = f.readlines()[0].strip()

    return token

def onMessage(bot, message):
    if message.command != "PRIVMSG":
        return

    content = message.params[-1].strip()
    if not content.startswith('!wg21'):
        return

    data = getCurrentList()
    SEARCH_PHRASE = '!wg21-search '
    LINK_PHRASE = '!wg21-link '

    colors = {
        'green': chr(0x03) + '03,01',
        'yellow': chr(0x03) + '08,01',
        'orange': chr(0x03) + '07,01',
        'reset': chr(0x0F)
    }

    if content.startswith(SEARCH_PHRASE):
        articles = []
        value = content[len(SEARCH_PHRASE):]
        for key, element in data.items():
            if value in element['title']:
                articles.append(key)
        docs = ', '.join(articles)
        msg = 'Documents: {green}{docs}{reset}'.format(docs=docs, **colors)

        resp = pyircbot.IRCMessage()
        resp.command = "PRIVMSG"
        resp.params = [message.params[0], msg]
        bot.sendIrcMessage(resp)

    elif content.startswith(LINK_PHRASE):
        tags = content[len(LINK_PHRASE):].strip().split()
        for tag in tags:
            if tag not in data:
                continue

            resp = pyircbot.IRCMessage()
            resp.command = "PRIVMSG"
            msg = '{green}{tag}{reset}: {orange}{link}{reset} {yellow}{title}{reset}'.format(tag=tag,title=data[tag]['title'],link=data[tag]['link'],**colors)
            resp.params = [message.params[0], msg]
            bot.sendIrcMessage(resp)

plugin = pyircbot.Plugin()
plugin.name='wg21'
plugin.token=load_token('~/ircbot_token')
plugin.onMessage=onMessage

bot = pyircbot.Bot('127.0.0.1', PORT, plugin)
bot.start()

while bot.isRunning():
    time.sleep(0.1)

bot.wait()
