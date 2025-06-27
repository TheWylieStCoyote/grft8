/* -*- c++ -*- */
/*
 * Copyright 2025 Daniel Paul.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef FT8_ENCODER_H
#define FT8_ENCODER_H

#include "message.h"
#include <bitset>
#include <gnuradio/logger.h>
#include <string>
#include <vector>

namespace gr
{
class logger;
}

class ft8_encoder
{
public:
  ft8_encoder ();
  ft8_encoder (const message &message);
  
  uint32_t encode_28 (std::string &temp_msg, const message &message);
  uint32_t std_call_to_28 (std::string &msg);
  uint16_t g4_to_15 (const message &message);
private:
  gr::logger d_logger;
  void bitfields (const message &message);

  void encode_standard (const message &message);
  //void encode_dexpedition (const message &message);
  //void encode_field_day (const message &message);
  //void encode_field_dayx (const message &message);
  //void encode_telemetry (const message &message);
  //void encode_rtty_ru (const message &message);
  //void encode_nonstd_call (const message &message);
  //void encode_euvhfx (const message &message);
  //void encode_free_text (const message &message);

  uint64_t nonstd_to_58 (std::string &msg);
  std::bitset<71> free_text_to_f71 (std::string &msg);
 
  uint32_t g6_to_15 (const message &message);

  bool encode_fdclass (char fd_class);
  bool encode_p1 (std::string &msg);
  bool encode_r1 (std::string &msg);
  bool encode_t1 (std::string &msg);
  uint8_t encode_r2 (std::string &msg);
  uint8_t encode_sigreport (std::string &msg);
};

#endif
