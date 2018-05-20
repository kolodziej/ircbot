#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "pyircbot/bot.hpp"
#include "pyircbot/plugin.hpp"
#include "pyircbot/helpers.hpp"
#include "ircbot/irc_message.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pyircbot, m) {
  using pyircbot::Plugin;
  using pyircbot::Bot;
  using pyircbot::helpers::responseDestination;

  py::class_<Plugin>(m, "Plugin")
    .def(py::init())
    .def_readwrite("name", &Plugin::name)
    .def_readwrite("token", &Plugin::token)
    .def_readwrite("onInit", &Plugin::onInit)
    .def_readwrite("onMessage", &Plugin::onMessage)
    .def_readwrite("onShutdown", &Plugin::onShutdown)
    .def_readwrite("onRestart", &Plugin::onRestart)
    .def_readwrite("onReload", &Plugin::onReload);

  py::class_<Bot>(m, "Bot")
    .def(py::init<const std::string&, uint16_t, Plugin>(),
         py::arg("hostname"), py::arg("port"), py::arg("plugin"))
    .def("hostname", &Bot::hostname)
    .def("port", &Bot::port)
    .def("connected", &Bot::connected)
    .def("start", &Bot::start)
    .def("stop", &Bot::stop)
    .def("isRunning", &Bot::isRunning)
    .def("wait", &Bot::wait)
    .def("send", &Bot::send)
    .def("sendIrcMessage", &Bot::sendIrcMessage);

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

  m.def("response_destination", responseDestination,
        py::arg("msg"), py::arg("priv") = false);
}
