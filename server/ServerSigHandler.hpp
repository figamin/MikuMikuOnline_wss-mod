//
// Using an interrupt handler for graceful deletion
//

#pragma once
#include <csignal>
#include <boost/thread.hpp>
//#include "../common/network/Session.hpp"
#include "Server.hpp"

namespace network {

class ServerSigHandler {
public:
  ServerSigHandler(int sig,Server *_server) : sig_to_wait(sig){
    sigset_t ss;
    sigemptyset( &ss );
    sigaddset( &ss, sig_to_wait );
    // Set the mask and also backup the current mask
    sigprocmask( SIG_BLOCK, &ss, &backup );
    // This thread waits for a signal, captures it, and then runs a member function
    signal_thread = new boost::thread( boost::bind(&ServerSigHandler::wait_and_exec, this) );
    signal_thread->detach();
    server=_server;
  }
  
  ~ServerSigHandler() {
    sigprocmask( SIG_SETMASK, &backup, NULL );
    // not needed
    // signal_thread->join(); 
    delete signal_thread;
  }

private:
  // Signals to be trapped
  int sig_to_wait;
  // Mask backup
  sigset_t backup;
  boost::thread *signal_thread;
  Server *server;
  bool wait_and_exec() {
    sigset_t ss;
    sigemptyset(&ss);
    int ret = sigaddset(&ss, sig_to_wait);
    if (ret != 0) 
      return false;
    // Block the signal
    ret = pthread_sigmask(SIG_BLOCK, &ss, NULL);
    if (ret != 0) 
      return false;
    while(1) {
      int signo;
      if (sigwait(&ss, &signo) == 0) {
	// Stop the server
	(server->Stop)( signo );
	break;
      }
    }
    return true;
  }
};

}
