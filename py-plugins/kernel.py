import os
import sys
import pyircbot
import json
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

FNAME = 'kernel_info'

def get_saved():
    with open(FNAME, 'r') as f:
        data = f.read()

    return json.loads(data)

def save(data):
    with open(FNAME, 'w') as f:
        f.write(json.dumps(data))

def get_current():
    text = urlopen('https://www.kernel.org/')
    soup = BeautifulSoup(text, 'lxml')
    stable, mainline, longterm = [], [], []
    for tr in soup.find(id='releases').find_all('tr'):
        td = tr.find_all('td')
        release = td[0].string
        ver = td[1].text.strip()

        if ver is None:
            continue

        if release == 'mainline:':
            mainline.append(ver)
        elif release == 'stable:':
            stable.append(ver)
        elif release == 'longterm:':
            longterm.append(ver)

    data = {
        'stable': stable,
        'mainline': mainline,
        'longterm': longterm,
        'time': int(time.time())
    }

    return data

def get_data():
    try:
        data = get_saved()
        if data['time'] < int(time.time()) - 3600 * 2:
            data = get_current()
            save(data)
    except:
        data = get_current()
        save(data)

    return data

def message(bot, message):
    if message.command != "PRIVMSG":
        return

    cmd = message.params[1].split()
    if cmd[0] != '!kernel':
        return

    try:
        data = get_data()
    except:
        print("Could not fetch data!")
        return

    colors = {
        'green': chr(0x03) + '03,01',
        'yellow': chr(0x03) + '08,01',
        'orange': chr(0x03) + '07,01',
        'reset': chr(0x0F)
    }

    key = cmd[1] if len(cmd) >= 2 else 'all'
    stable_txt = 'Linux stable: {green}{}{reset}'.format(', '.join(data['stable']), **colors)
    mainline_txt = 'Linux mainline: {orange}{}{reset}'.format(', '.join(data['mainline']), **colors)
    longterm_txt = 'Linux longterm: {yellow}{}{reset}'.format(', '.join(data['longterm']), **colors)
    all_txt = stable_txt + ' ' + mainline_txt + ' ' + longterm_txt

    result = {
        'all': all_txt,
        'mainline': mainline_txt,
        'stable': stable_txt,
        'longterm': longterm_txt
    }

    if key not in result:
        return

    resp = pyircbot.IRCMessage()
    resp.command = 'PRIVMSG'
    resp.params = [message.params[0], result[key]]

    bot.sendIrcMessage(resp)

plugin = pyircbot.Plugin()
plugin.name = 'kernel info'
plugin.token = load_token('~/ircbot_token')
plugin.onMessage = message

bot = pyircbot.Bot('127.0.0.1', PORT, plugin)
bot.start()

while bot.isRunning():
    time.sleep(0.1)

bot.wait()
