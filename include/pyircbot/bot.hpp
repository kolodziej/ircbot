#ifndef _PYIRCBOT_BOT_HPP
#define _PYIRCBOT_BOT_HPP

#include <pybind11/pybind11.h>

namespace py = pybind11;

class Bot {
 public:
  Bot(const std::string& hostname, uint16_t port);

  std::string hostname() const;
  uint16_t port() const;

 private:
  const std::string m_hostname;
  const uint16_t m_port;
};

PYBIND11_MODULE(pyircbot, m) {
  py::class_<Bot>(m, "Bot")
    .def(py::init<const std::string&, uint16_t>(),
         py::arg("hostname"), py::arg("port"))
    .def("hostname", &Bot::hostname)
    .def("port", &Bot::port);
}

#endif
