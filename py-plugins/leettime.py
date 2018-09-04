import time
import sys
import os
import datetime
import pyircbot

if len(sys.argv) < 3:
    print("Port channel!")
    sys.exit(2)

PORT=int(sys.argv[1])
CHANNEL=sys.argv[2]

def load_token(fname):
    path = os.path.expanduser(fname)
    with open(path, 'r') as f:
        token = f.readlines()[0].strip()

    return token

plugin = pyircbot.Plugin()
plugin.name='leettime'
plugin.token=load_token('~/ircbot_token')

bot = pyircbot.Bot('127.0.0.1', PORT, plugin)
bot.start()

LEET_MSG="{color} It's leet time! {reset}".format(
    color=chr(0x03) + '07,01',
    reset=chr(0x0F)
)

sent = False
while bot.isRunning():
    now = datetime.datetime.now()
    hour, minute = now.hour, now.minute
    if hour == 13 and minute == 37:
        if not sent:
            msg = pyircbot.IRCMessage()
            msg.command = "PRIVMSG"
            msg.params = [CHANNEL, LEET_MSG]
            bot.sendIrcMessage(msg)
            sent = True
    else:
        sent = False

    time.sleep(1)

bot.wait()
