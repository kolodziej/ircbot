#ifndef _PLUGIN_HPP
#define _PLUGIN_HPP

#include <memory>
#include <deque>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <unordered_map>

#include "ircbot/client.hpp"
#include "ircbot/plugin_config.hpp"
#include "ircbot/irc_message.hpp"
#include "ircbot/command_parser.hpp"
#include "ircbot/config.hpp"
#include "ircbot/logger.hpp"

namespace ircbot {

/** \class Plugin
 *
 * \brief Base for all plugins
 *
 * Base class for all plugins. It implements support for filtering, processing
 * and responding to incoming messages. It also supports CommandParser.
 */

class Plugin {
  /** Type of function that is called when command from user comes */
  using CmdFunction = std::function<void(const CommandParser::Command&)>;

 public:
  /** Constructor 
   *
   * \param config PluginConfig instance with prefix for command configured
   */
  Plugin(PluginConfig config);

  /** Destructor
   * 
   * Performs shutdown operations and waits for running thread to terminate.
   */
  virtual ~Plugin() {};

  /** Get pointer to client */
  std::shared_ptr<Client> client();

  /** Returns plugin id
   *
   * \return Unique string identifier in Client scope
   */
  std::string getId() const;

  /** Returns plugin name
   *
   * \return plugin name - same name can occur many times for one Client
   * instance
   */
  virtual std::string getName() const = 0;

  /** Stops plugin */
  void stop();

  /** It is called each time when message for Plugin is received
   *
   * \param cmd IRCMessage instance for incoming message
   */
  void receive(IRCMessage cmd);

  /** Called on plugin initialization */
  virtual void onInit() {}

  /** Main plugin's thread loop */
  virtual void run();

  /** Is called in main thread loop when m_incoming_cv is notified
   *
   * \param msg incoming message
   */
  virtual void onMessage(IRCMessage /*msg*/) {}

  /** Is called each time configuration changes */
  virtual void onNewConfiguration() {}

  /** Checks if message should be processed by this plugin
   *
   * \param cmd constant reference to message
   */
  virtual bool filter(const IRCMessage& cmd);

  /** Is called by destructor */
  virtual void onShutdown() {}

  /** If Plugin has instance of CommandParser, this function checks if given
   * message is a command using isCommand function.
   *
   * \param cmd const reference to message 
   */
  bool preFilter(const IRCMessage& cmd);

  /** Install CommandParser instance in plugin
   *
   * \param parser shared pointer to CommandParser
   */
  void installCommandParser(std::shared_ptr<CommandParser> parser);

  /** Check if plugin has installed CommandParser */
  bool hasCommandParser() const;

  /** Deinstall CommandParser instance */
  void deinstallCommandParser();

  /** Check if cmd is a command
   *
   * \param cmd irc message instance
   */
  bool isCommand(IRCMessage cmd) const;

  /** Is called when incoming message was a correctly parsed command
   *
   * \param cmd Command received from CommandParser
   */
  void onCommand(CommandParser::Command cmd);

  /** Set configuration
   *
   * \param cfg new configuration
   */
  void setConfig(Config cfg);

  /** Get configuration
   *
   * \return reference to configuration
   */
  Config& getConfig();

  /** Get const refernce to configuration
   *
   * \return const reference to configuration
   */
  const Config& getConfig() const;

  /** Run plugin's thread with exception handling */
  void spawn();

 protected:
  /** Check if plugin is running */
  bool isRunning() const;
  size_t commandsCount() const;
  IRCMessage getCommand();

  /** Send message
   *
   * Passes message to Client's queue of outgoing messages. It is thread safe.
   *
   * \param msg outgoing message
   */
  void send(const IRCMessage& msg);

  /** Get configuration as boost::property_tree::ptree */
  pt::ptree& cfg();

  /** Add function that can be called using commands passed by user via irc
   * message 
   * 
   * \param cmd command name
   * \param f function that should be called
   */
  void addFunction(const std::string& cmd, CmdFunction f);

  /** Remove function for given command
   *
   * \param cmd command for which function should be removed 
   */
  void removeFunction(const std::string& cmd);

  /** Call function for given command
   *
   * \param command command instance for which function should be called
   */
  void callFunction(const CommandParser::Command& command);

 private:
  /** instance of Client that owns this plugin instance */
  std::shared_ptr<Client> m_client;

  /** plugin id */
  const std::string m_id;

  /** plugin configuration */
  Config m_cfg;

  /** installed CommandParser */
  std::shared_ptr<CommandParser> m_command_parser;

  /** map commands to functions */
  std::unordered_map<std::string, CmdFunction> m_functions;

  /** is plugin running? */
  std::atomic<bool> m_running;

  /** queue of incoming irc messages */
  std::deque<IRCMessage> m_incoming;

  /** mutex synchronizing incoming queue */
  std::mutex m_incoming_mtx;

  /** condition variable notifying main plugin thread about new messages */
  std::condition_variable m_incoming_cv;

  /** thread for this plugin */
  std::thread m_thread;
};

} // namespace ircbot

#endif
