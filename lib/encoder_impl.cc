/* -*- c++ -*- */
/*
 * Copyright 2025 Daniel Paul.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "encoder_impl.h"
#include "message.h"
#include "ft8_encoder.h"

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
    d_message_obj(message_text),
    d_sample_idx(0),
    d_waveform_generated(false)
    {
        // Message preprocessing is handled by the message object constructor
        d_logger->info("Encoder message: {}", d_message_obj.get_message());
        generate_waveform();
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
    void encoder_impl::generate_waveform()
    {
        try {
            ft8_encoder encoder;
            std::bitset<77> message_bits = encoder.encode_standard(d_message_obj);
            d_waveform = encoder.encode_ft8_complete(message_bits);
            d_waveform_generated = true;
            d_logger->info("FT8 waveform generated with {} samples", d_waveform.size());
        } catch (const std::exception& e) {
            d_logger->error(e.what());
            d_waveform_generated = false;
        }
    }

    int
    encoder_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      auto out = static_cast<output_type*>(output_items[0]);
      if (! d_waveform_generated || d_waveform.empty()) {
        std::fill(out, out+noutput_items, 0.0f);
        return noutput_items;
      }

      int to_output = 0;
      
      for(int i = 0; i<noutput_items; ++i) {
        if (d_sample_idx < d_waveform.size()){
            out[i] = d_waveform[d_sample_idx];
            d_sample_idx++;
            to_output++;
        } else {
            out[i] = 0.0f;
            to_output++;
        }
      // Tell runtime system how many output items we produced.
      return to_output;
    }
   }
  } /* namespace ft8 */
} /* namespace gr */
