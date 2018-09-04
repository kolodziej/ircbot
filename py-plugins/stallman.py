import os
import sys
import pyircbot
import json
import time

if len(sys.argv) < 2:
    print("Port!")
    sys.exit(2)

PORT=int(sys.argv[1])

LINKS = {
    'airbnb': 'https://stallman.org/airbnb.html',
    'amazon': 'https://stallman.org/amazon.html',
    'amtrak': 'https://stallman.org/amtrak.html',
    'ancestry': 'https://stallman.org/ancestry.html',
    'apple': 'https://stallman.org/apple.html',
    'discord': 'https://stallman.org/discord.html',
    'ebooks': 'https://stallman.org/ebooks.pdf',
    'eventbrite': 'https://stallman.org/eventbrite.html',
    'evernote': 'https://stallman.org/evernote.html',
    'facebook': 'https://stallman.org/facebook.html',
    'google': 'https://stallman.org/google.html',
    'intel': 'https://stallman.org/intel.html',
    'linkedin': 'https://stallman.org/linkedin.html',
    'lyft': 'https://stallman.org/lyft.html',
    'meetup': 'https://stallman.org/meetup.html',
    'microsoft': 'https://stallman.org/microsoft.html',
    'netflix': 'https://stallman.org/netflix.html',
    'patreon': 'https://stallman.org/patreon.html',
    'pay-toilets': 'https://stallman.org/pay-toilets.html',
    'skype': 'https://stallman.org/skype.html',
    'spotify': 'https://stallman.org/spotify.html',
    'twitter': 'https://stallman.org/twitter.html',
    'uber': 'https://stallman.org/uber.html',
    'wendys': 'https://stallman.org/wendys.html',
    'ubuntu': 'https://www.youtube.com/watch?v=CP8CNp-vksc',
    'linux': 'https://www.youtube.com/watch?v=b89fKsT1i7s',
    'torvalds': 'https://www.youtube.com/watch?v=b89fKsT1i7s',
}

def load_token(fname):
    path = os.path.expanduser(fname)
    with open(path, 'r') as f:
        token = f.readlines()[0].strip()

    return token

def message(bot, message):
    if message.command != "PRIVMSG":
        return

    cmd = message.params[1].split()
    if cmd[0] != '!stallman':
        return

    colors = {
        'green': chr(0x03) + '03,01',
        'yellow': chr(0x03) + '08,01',
        'orange': chr(0x03) + '07,01',
        'reset': chr(0x0F)
    }

    response = '{yellow}[Uncle Stallman wants to interject, because he has his very important opinion: {link}]{reset}'

    key = cmd[1]
    msg = '{orange}[Uncle Stallman has nothing to say this time... really!]{reset}'.format(**colors)
    if key in LINKS:
        msg = response.format(link=LINKS[key], **colors)

    dest = pyircbot.response_destination(message)
    resp = pyircbot.IRCMessage()
    resp.command = 'PRIVMSG'
    resp.params = [dest, msg]
    bot.sendIrcMessage(resp)

plugin = pyircbot.Plugin()
plugin.name = 'StallmanSays'
plugin.token = load_token('~/ircbot_token')
plugin.onMessage = message

bot = pyircbot.Bot('127.0.0.1', PORT, plugin)
bot.start()

while bot.isRunning():
    time.sleep(0.1)

bot.wait()
