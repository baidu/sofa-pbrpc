//
// asio.hpp
// ~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//  See www.boost.org/libs/asio for documentation.
//

#ifndef SOFA_PBRPC_BOOST_ASIO_HPP
#define SOFA_PBRPC_BOOST_ASIO_HPP

#include <sofa/pbrpc/boost/asio/async_result.hpp>
#include <sofa/pbrpc/boost/asio/basic_datagram_socket.hpp>
#include <sofa/pbrpc/boost/asio/basic_deadline_timer.hpp>
#include <sofa/pbrpc/boost/asio/basic_io_object.hpp>
#include <sofa/pbrpc/boost/asio/basic_raw_socket.hpp>
#include <sofa/pbrpc/boost/asio/basic_seq_packet_socket.hpp>
#include <sofa/pbrpc/boost/asio/basic_serial_port.hpp>
#include <sofa/pbrpc/boost/asio/basic_signal_set.hpp>
#include <sofa/pbrpc/boost/asio/basic_socket_acceptor.hpp>
#include <sofa/pbrpc/boost/asio/basic_socket_iostream.hpp>
#include <sofa/pbrpc/boost/asio/basic_socket_streambuf.hpp>
#include <sofa/pbrpc/boost/asio/basic_stream_socket.hpp>
#include <sofa/pbrpc/boost/asio/basic_streambuf.hpp>
#include <sofa/pbrpc/boost/asio/basic_waitable_timer.hpp>
#include <sofa/pbrpc/boost/asio/buffer.hpp>
#include <sofa/pbrpc/boost/asio/buffered_read_stream_fwd.hpp>
#include <sofa/pbrpc/boost/asio/buffered_read_stream.hpp>
#include <sofa/pbrpc/boost/asio/buffered_stream_fwd.hpp>
#include <sofa/pbrpc/boost/asio/buffered_stream.hpp>
#include <sofa/pbrpc/boost/asio/buffered_write_stream_fwd.hpp>
#include <sofa/pbrpc/boost/asio/buffered_write_stream.hpp>
#include <sofa/pbrpc/boost/asio/buffers_iterator.hpp>
#include <sofa/pbrpc/boost/asio/completion_condition.hpp>
#include <sofa/pbrpc/boost/asio/connect.hpp>
//#include <sofa/pbrpc/boost/asio/coroutine.hpp>
#include <sofa/pbrpc/boost/asio/datagram_socket_service.hpp>
#include <sofa/pbrpc/boost/asio/deadline_timer_service.hpp>
#include <sofa/pbrpc/boost/asio/deadline_timer.hpp>
#include <sofa/pbrpc/boost/asio/error.hpp>
#include <sofa/pbrpc/boost/asio/generic/basic_endpoint.hpp>
#include <sofa/pbrpc/boost/asio/generic/datagram_protocol.hpp>
#include <sofa/pbrpc/boost/asio/generic/raw_protocol.hpp>
#include <sofa/pbrpc/boost/asio/generic/seq_packet_protocol.hpp>
#include <sofa/pbrpc/boost/asio/generic/stream_protocol.hpp>
#include <sofa/pbrpc/boost/asio/handler_alloc_hook.hpp>
#include <sofa/pbrpc/boost/asio/handler_continuation_hook.hpp>
#include <sofa/pbrpc/boost/asio/handler_invoke_hook.hpp>
#include <sofa/pbrpc/boost/asio/handler_type.hpp>
#include <sofa/pbrpc/boost/asio/io_service.hpp>
#include <sofa/pbrpc/boost/asio/ip/address.hpp>
#include <sofa/pbrpc/boost/asio/ip/address_v4.hpp>
#include <sofa/pbrpc/boost/asio/ip/address_v6.hpp>
#include <sofa/pbrpc/boost/asio/ip/basic_endpoint.hpp>
#include <sofa/pbrpc/boost/asio/ip/basic_resolver.hpp>
#include <sofa/pbrpc/boost/asio/ip/basic_resolver_entry.hpp>
#include <sofa/pbrpc/boost/asio/ip/basic_resolver_iterator.hpp>
#include <sofa/pbrpc/boost/asio/ip/basic_resolver_query.hpp>
#include <sofa/pbrpc/boost/asio/ip/host_name.hpp>
#include <sofa/pbrpc/boost/asio/ip/icmp.hpp>
#include <sofa/pbrpc/boost/asio/ip/multicast.hpp>
#include <sofa/pbrpc/boost/asio/ip/resolver_query_base.hpp>
#include <sofa/pbrpc/boost/asio/ip/resolver_service.hpp>
#include <sofa/pbrpc/boost/asio/ip/tcp.hpp>
#include <sofa/pbrpc/boost/asio/ip/udp.hpp>
#include <sofa/pbrpc/boost/asio/ip/unicast.hpp>
#include <sofa/pbrpc/boost/asio/ip/v6_only.hpp>
#include <sofa/pbrpc/boost/asio/is_read_buffered.hpp>
#include <sofa/pbrpc/boost/asio/is_write_buffered.hpp>
#include <sofa/pbrpc/boost/asio/local/basic_endpoint.hpp>
#include <sofa/pbrpc/boost/asio/local/connect_pair.hpp>
#include <sofa/pbrpc/boost/asio/local/datagram_protocol.hpp>
#include <sofa/pbrpc/boost/asio/local/stream_protocol.hpp>
#include <sofa/pbrpc/boost/asio/placeholders.hpp>
#include <sofa/pbrpc/boost/asio/posix/basic_descriptor.hpp>
#include <sofa/pbrpc/boost/asio/posix/basic_stream_descriptor.hpp>
#include <sofa/pbrpc/boost/asio/posix/descriptor_base.hpp>
#include <sofa/pbrpc/boost/asio/posix/stream_descriptor.hpp>
#include <sofa/pbrpc/boost/asio/posix/stream_descriptor_service.hpp>
#include <sofa/pbrpc/boost/asio/raw_socket_service.hpp>
#include <sofa/pbrpc/boost/asio/read.hpp>
#include <sofa/pbrpc/boost/asio/read_at.hpp>
//#include <sofa/pbrpc/boost/asio/read_until.hpp>
#include <sofa/pbrpc/boost/asio/seq_packet_socket_service.hpp>
#include <sofa/pbrpc/boost/asio/serial_port.hpp>
#include <sofa/pbrpc/boost/asio/serial_port_base.hpp>
#include <sofa/pbrpc/boost/asio/serial_port_service.hpp>
#include <sofa/pbrpc/boost/asio/signal_set.hpp>
#include <sofa/pbrpc/boost/asio/signal_set_service.hpp>
#include <sofa/pbrpc/boost/asio/socket_acceptor_service.hpp>
#include <sofa/pbrpc/boost/asio/socket_base.hpp>
#include <sofa/pbrpc/boost/asio/strand.hpp>
#include <sofa/pbrpc/boost/asio/stream_socket_service.hpp>
#include <sofa/pbrpc/boost/asio/streambuf.hpp>
#include <sofa/pbrpc/boost/asio/time_traits.hpp>
#include <sofa/pbrpc/boost/asio/version.hpp>
#include <sofa/pbrpc/boost/asio/wait_traits.hpp>
#include <sofa/pbrpc/boost/asio/waitable_timer_service.hpp>
#include <sofa/pbrpc/boost/asio/write.hpp>
#include <sofa/pbrpc/boost/asio/write_at.hpp>

#endif // SOFA_PBRPC_BOOST_ASIO_HPP
