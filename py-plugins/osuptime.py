import time
import sys
import os
import pyircbot
from datetime import timedelta

if len(sys.argv) < 2:
    print("Port!")
    sys.exit(2)

PORT=int(sys.argv[1])

def load_token(fname):
    path = os.path.expanduser(fname)
    with open(path, 'r') as f:
        token = f.readlines()[0].strip()

    return token

def onMessage(bot, message):
    if message.command != "PRIVMSG":
        return

    CMD = "!osuptime"
    if not message.params[1][:len(CMD)] == CMD:
        return

    with open('/proc/uptime', 'r') as f:
        uptime_seconds = float(f.readline().split()[0])
        uptime_string = str(timedelta(seconds = uptime_seconds)) 

    uptime_msg = "{color} Host system uptime: {uptime}! {reset}".format(
        color=chr(0x03) + '07,01',
        reset=chr(0x0F),
        uptime=uptime_string 
    )

    resp = pyircbot.IRCMessage()
    resp.command = "PRIVMSG"
    resp.params = [pyircbot.response_destination(message), uptime_msg]
    bot.sendIrcMessage(resp)

plugin = pyircbot.Plugin()
plugin.name='leettime'
plugin.token=load_token('~/ircbot_token')
plugin.onMessage = onMessage

bot = pyircbot.Bot('127.0.0.1', PORT, plugin)
bot.start()

sent = False
while bot.isRunning():
    time.sleep(0.1)

bot.wait()
