#ifndef QUAE_MODICA_INTERFACE_H
#define QUAE_MODICA_INTERFACE_H

namespace QuaeModicaUtil {
/**
 * parent interface class
 */
template <class Input, class Output> class Interface {
public:
  Interface() : _isConnected(false) {}
  ~Interface() { disconnect(); }

  /**
   * connect to the given interface
   */
  bool connect() {
    return _isConnected || (_isConnected = childConnect());
  }

  /**
   * disconnect to the given interface
   */
  bool disconnect() {
    return !_isConnected || (_isConnected = childDisconnect());
  }

  /**
   * return if the given interface is connected
   */
  bool isConnected() {
    return (_isConnected = childPing());
  }

  /**
   * read a message from the interface
   */
  Output receive(unsigned int retries = 3) {
    while (retries-- > 0 && !connect());
    return (retries == 0) ? nullptr : childReceive();
  }

  /**
   * write a message to the interface
   */
  bool send(const Input& value, unsigned int retries = 3) {
    while (retries-- > 0 && !connect());
    return (retries == 0) ? 1 : childSend(value);
  }
private:
  bool _isConnected;

  /**
   * child implemented connect function
   */
  virtual bool childConnect() = 0;

  /**
   * child implemented disconnect function
   */
  virtual bool childDisconnect() = 0;

  /**
   * child implemented ping function
   */
  virtual bool childPing() = 0;

  /**
   * child implemented read function
   */
  virtual Output childReceive() = 0;

  /**
   * child implemented write function
   */
  virtual bool childSend(Input value) = 0;
};
}
#endif // #ifndef QUAE_MODICA_INTERFACE_H
