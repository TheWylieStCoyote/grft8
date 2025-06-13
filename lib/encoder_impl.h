/* -*- c++ -*- */
/*
 * Copyright 2025 Daniel Paul.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_FT8_ENCODER_IMPL_H
#define INCLUDED_FT8_ENCODER_IMPL_H

#include <gnuradio/ft8/encoder.h>
#include <string>

namespace gr {
  namespace ft8 {

    class encoder_impl : public encoder
    {
     private:
        std::string d_message;
        
        void preprocess_message();
        void input_validation();
        void trim();
        void character_validation();
        void message_type_detection();
        bool is_signal_report(const std::string& token);
        bool is_callsign(const std::string& keyword);
        bool is_dxpedition(const std::vector<std::string>& keywords);
        bool is_hex(const std::string& keyword);
        bool is_field_day(const std::vector<std::string>& keywords, bool check_r);
        bool is_field_day_class(const std::string& keyword);
        bool is_nonstd_callsign(const std::string& keyword);
        bool is_telemetry(const std::vector<std::string>& keywords);
        bool is_std(const std::vector<std::string>& keywords);
        bool is_rtty_ru(const std::vector<std::string>& keywords);
        bool is_contest(const std::string& keyword);
        bool is_euvhfx(const std::vector<std::string>& keywords);
        bool is_grid_square(const std::string& keyword);
        bool is_grid_6square(const std::string& keyword);
        bool is_nonstd(const std::vector<std::string>& keywords);
    
    public:
      encoder_impl(std::string message);
      ~encoder_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace ft8
} // namespace gr

#endif /* INCLUDED_FT8_ENCODER_IMPL_H */
