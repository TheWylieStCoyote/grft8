/* -*- c++ -*- */
/*
 * Copyright 2025 Daniel Paul.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "encoder_impl.h"
#include "message.h"

namespace gr {
  namespace ft8 {

    #pragma message("set the following appropriately and remove this warning")
    using output_type = float;
    encoder::sptr
    encoder::make(std::string message)
    {
      return gnuradio::make_block_sptr<encoder_impl>(message);
    }


    /*
     * The private constructor
     */
    encoder_impl::encoder_impl(std::string message_text)
      : gr::sync_block("encoder",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(output_type))),
    d_message_obj(message_text)
    {
        // Message preprocessing is handled by the message object constructor
        d_logger->info("Encoder initialized with message: {}", d_message_obj.get_message());
    }

    /*
     * Our virtual destructor.
     */
    encoder_impl::~encoder_impl()
    {
    }

    message::message_type encoder_impl::get_message_type()
    {
        return d_message_obj.message_type_detection();
    }

    const std::string& encoder_impl::get_processed_message()
    {
        return d_message_obj.get_message();
    }

    int
    encoder_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      auto out = static_cast<output_type*>(output_items[0]);

      #pragma message("Implement the signal processing in your block and remove this warning")
      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace ft8 */
} /* namespace gr */
