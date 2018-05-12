#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "pyircbot/bot.hpp"
#include "pyircbot/plugin.hpp"
#include "ircbot/irc_message.hpp"
#include "ircbot/command_parser.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pyircbot, m) {
  py::class_<Plugin>(m, "Plugin")
    .def(py::init())
    .def_readwrite("name", &Plugin::name)
    .def_readwrite("token", &Plugin::token)
    .def_readwrite("onInit", &Plugin::onInit)
    .def_readwrite("onMessage", &Plugin::onMessage)
    .def_readwrite("onCommand", &Plugin::onCommand)
    .def_readwrite("onShutdown", &Plugin::onShutdown)
    .def_readwrite("onRestart", &Plugin::onRestart)
    .def_readwrite("onReload", &Plugin::onReload);

  py::class_<Bot>(m, "Bot")
    .def(py::init<const std::string&, uint16_t, Plugin>(),
         py::arg("hostname"), py::arg("port"), py::arg("plugin"))
    .def("hostname", &Bot::hostname)
    .def("port", &Bot::port)
    .def("connect", &Bot::connect)
    .def("connected", &Bot::connected)
    .def("start", &Bot::start)
    .def("stop", &Bot::stop)
    .def("send", &Bot::send)
    .def("receive", &Bot::receive)
    .def("initialize", &Bot::initialize,
         py::arg("name"), py::arg("token"));

  py::class_<IRCMessage>(m, "IRCMessage")
    .def(py::init())
    .def(py::init<const std::string&,
                  std::initializer_list<std::string>>(),
         py::arg("command"), py::arg("params"))
    .def("__str__", &IRCMessage::operator std::string)
    .def_readwrite("servername", &IRCMessage::servername)
    .def_readwrite("user", &IRCMessage::user)
    .def_readwrite("nick", &IRCMessage::nick)
    .def_readwrite("host", &IRCMessage::host)
    .def_readwrite("command", &IRCMessage::command)
    .def_readwrite("params", &IRCMessage::params);

  py::class_<CommandParser::Command>(m, "Command")
    .def(py::init())
    .def_readwrite("command", &CommandParser::Command::command)
    .def_readwrite("arguments", &CommandParser::Command::arguments)
    .def_readwrite("irc_message", &CommandParser::Command::irc_message);
}
