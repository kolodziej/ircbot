namespace ircbot {
namespace helpers {

template <typename Endpoint>
std::string endpointAddress(Endpoint endpoint) {
  return endpoint.address().to_string() + std::string{":"} +
         std::to_string(endpoint.port());
}

}  // namespace helpers
}  // namespace ircbot
