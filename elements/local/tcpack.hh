#ifndef TCPACK_HH
#define TCPACK_HH
#include <click/element.hh>
#include <click/timer.hh>
#include <click/click_tcp.h>

/*
 * =c
 * TCPAck([HEARTBEAT])
 * =s TCP
 * acknowledge TCP packets
 * =d
 * performs TCP style acknowledgement. marked TCP/IP packets are expected on
 * both input and output ports. use MarkIPHeader to mark packets.
 *
 * input port 0 is TCP input. a packet that arrives on this input may trigger
 * an acknowledgement. the acknowledgement acknowledges the latest in-order
 * packet received. TCPAck gets this information from a downstream TCPBuffer
 * element that it discovers using flow-based router context, much like how
 * RED elements discover their QUEUE elements. the packet is sent out on
 * output port 0 untouched.
 *
 * input port 1 is TCP output. a packet that arrives on this input get tagged
 * with an acknowledgement number. this ack number is obtained from TCPBuffer
 * as described above. this packet also causes TCPAck to cancel scheduled ACK.
 * the packet is then sent out on output port 1.
 *
 * finally, output port 2 is used to send scheduled ACKs. TCPAck uses the
 * latest seq number it sees across input/output port 1 as the sequence number
 * for the acknowledgement. an ACK is generated on this output only if after
 * HEARTBEAT number of ms a triggered acknowledge was not sent. by default,
 * HEARTBEAT is set to 20.
 *
 * TCPAck only deals with DATA packets. it doesn't try to acknowledge SYN and
 * FIN packets. TCPAck starts using ack no from the first SYN ACK packet it
 * seems on in/output port 0 or 1. packets before that are rejected.
 */

class TCPBuffer;

class TCPAck : public Element {
private:
  Timer _timer;

  bool _synack;
  unsigned _seq_nxt;
  unsigned _ack_nxt;
  TCPBuffer *_tcpbuffer;

  unsigned _heartbeat_ms;
  
  bool iput(Packet *);
  bool oput(Packet *);

public:
  TCPAck();
  ~TCPAck();
  
  const char *class_name() const		{ return "TCPAck"; }
  const char *processing() const		{ return "aa/aah"; }
  TCPAck *clone() const				{ return new TCPAck; }

  int initialize(ErrorHandler *);
  void uninitialize();
  int configure(const Vector<String> &conf, ErrorHandler *errh);

  void push(int, Packet *);
  Packet *pull(int);
  void run_scheduled();
};

#endif

