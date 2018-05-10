#include <pybind11/pybind11.h>
#include <pybind11/functional.h>

#include "pyircbot/bot.hpp"
#include "pyircbot/plugin.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pyircbot, m) {
  py::class_<Plugin>(m, "Plugin")
    .def(py::init())
    .def_readwrite("onMessage", &Plugin::onMessageCallback);

  py::class_<Bot>(m, "Bot")
    .def(py::init<const std::string&, uint16_t>(),
         py::arg("hostname"), py::arg("port"))
    .def("hostname", &Bot::hostname)
    .def("port", &Bot::port)
    .def("connect", &Bot::connect)
    .def("start", &Bot::start)
    .def("stop", &Bot::stop)
    .def("send", &Bot::send)
    .def("receive", &Bot::receive)
    .def("initialize", &Bot::initialize,
         py::arg("name"), py::arg("token"));
}
