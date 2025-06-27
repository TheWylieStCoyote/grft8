/* -*- c++ -*- */
/*
 * Copyright 2025 Daniel Paul.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ft8_encoder.h"
#include "message.h"
#include <bitset>
#include <boost/multiprecision/cpp_int.hpp>
#include <regex>
#include <sstream>
#include <string_view>
#include <gmpxx.h>

ft8_encoder::ft8_encoder () : d_logger ("FT8_encoding")
{
  d_logger.info ("Message encoding created");
}
ft8_encoder::ft8_encoder (const message &message) : d_logger ("FT8_encoding")
{
  d_logger.info ("FT8 encoding object constructed");
  bitfields (message);
}

void
ft8_encoder::bitfields (const message &message)
{
  message::message_type type = message.message_type_detection ();

  switch (type)
    {
    case message::message_type::standard: // or euvhf
      encode_standard (message);
      break;
    /*case message::message_type::dxpedition:
      encode_dexpedition (message);
      break;
    case message::message_type::field_day:
      encode_field_day (message);
      break;
    case message::message_type::field_dayx:
      encode_field_dayx (message);
      break;
    case message::message_type::telemetry:
      encode_telemetry (message);
      break;
    case message::message_type::rtty_ru:
      encode_rtty_ru (message);
      break;
    case message::message_type::nonstd_call:
      encode_nonstd_call (message);
      break;
    case message::message_type::euvhfx:
      encode_euvhfx (message);
      break;
    case message::message_type::euvhf:
      encode_standard (message);
      break;
    case message::message_type::unknown:
      d_logger.error ("no message");
      break;
    case message::message_type::free_text:
      encode_free_text (message);
      break;*/
    }
}

void
ft8_encoder::encode_standard (const message &message)
{
  std::string temp_msg = message.get_message ();
  std::bitset<28> c28a = 0, c28b = 0;
  std::bitset<1> r1 = 0, R1 = 0;
  std::bitset<15> g15 = 0;

  c28a = encode_28 (temp_msg, message);
  c28b = encode_28 (temp_msg, message);
  g15 = g4_to_15 (message);
}

uint32_t
ft8_encoder::encode_28 (std::string &temp_msg, const message &message)
{
  if (temp_msg.find ("DE ") != std::string::npos)
    {
      temp_msg
          = std::regex_replace (temp_msg, std::regex ("\\bDE\\b\\s*"), "");
      ;
      return 0;
    }
  if (temp_msg.find ("QRZ ") != std::string::npos)
    {
      temp_msg
          = std::regex_replace (temp_msg, std::regex ("\\bQRZ\\b\\s*"), "");
      return 1;
    }

  std::regex cq_only (R"(^CQ\s*$)");
  if (std::regex_match (temp_msg, cq_only))
    {
      temp_msg
          = std::regex_replace (temp_msg, std::regex ("\\bCQ\\b\\s*"), "");
      return 2;
    }

  std::regex cq_number (R"(CQ\s+(\d{1,3})(?=\s|$))");
  std::smatch number_match;
  if (std::regex_search (temp_msg, number_match, cq_number))
    {
      uint32_t number = std::stoul (number_match[1].str ());
      if (number <= 999)
        {
          temp_msg = std::regex_replace (temp_msg, cq_number, "");
          return 3 + number;
        }
    }

  std::regex cq_one_letter (R"(CQ\s+([A-Z])(?=\s|$))");
  std::smatch one_letter;
  if (std::regex_search (temp_msg, one_letter, cq_one_letter))
    {
      char letter = one_letter[1].str ()[0];
      temp_msg = std::regex_replace (temp_msg, cq_one_letter, "");
      return 1004 + (letter - 'A');
    }

  std::regex cq_two_letter (R"(CQ\s+([A-Z]{2})(?=\s|$))");
  std::smatch two_letter;
  if (std::regex_search (temp_msg, two_letter, cq_two_letter))
    {
      std::string letters = two_letter[1].str ();
      temp_msg = std::regex_replace (temp_msg, cq_two_letter, "");
      uint32_t letter_val = (letters[0] - 'A') * 26
                            + (letters[1] - 'A'); // essnetially base 26
      return 1031 + letter_val;
    }

  std::regex cq_three_letter (R"(CQ\s+([A-Z]{3})(?=\s|$))");
  std::smatch three_letter;
  if (std::regex_search (temp_msg, three_letter, cq_three_letter))
    {
      std::string letters = three_letter[1].str ();
      temp_msg = std::regex_replace (temp_msg, cq_three_letter, "");
      uint32_t letter_val = (letters[0] - 'A') * 26 * 26
                            + (letters[1] - 'A') * 26 + (letters[2] - 'A');
      return 1760 + letter_val;
    }

  std::regex cq_four_letter (R"(CQ\s+([A-Z]{4})(?=\s|$))");
  std::smatch four_letter;
  if (std::regex_search (temp_msg, four_letter, cq_four_letter))
    {
      std::string letters = four_letter[1].str ();
      temp_msg = std::regex_replace (temp_msg, cq_four_letter, "");
      uint32_t letter_val = (letters[0] - 'A') * 26 * 26 * 26
                            + (letters[1] - 'A') * 26 * 26
                            + (letters[2] - 'A') * 26 + (letters[3] - 'A');
      return 21443 + letter_val;
    }

  std::istringstream keywords (temp_msg);
  std::string keyword;
  while (keywords >> keyword)
    {
      if (message.is_callsign (keyword))
        {
          uint32_t encoded_28 = std_call_to_28 (temp_msg);
          size_t pos = temp_msg.find (keyword);
          if (pos != std::string::npos)
            {
              temp_msg.erase (pos, keyword.length ());
            }
          return encoded_28;
        }
    }

  return 0;
}

uint32_t
ft8_encoder::std_call_to_28 (std::string &msg)
{
  constexpr std::string_view a1
      = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // space, numbers, letters
  constexpr std::string_view a2
      = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // numbers, letters
  constexpr std::string_view a3 = "0123456789"; // numbers
  constexpr std::string_view a4
      = " ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // space, letters

  std::string temp_msg = msg;
  temp_msg.resize (6, ' '); // max size 6 chars

  size_t i1 = a1.find (temp_msg[0]);
  size_t i2 = a2.find (temp_msg[1]);
  size_t i3 = a3.find (temp_msg[2]);
  size_t i4 = a4.find (temp_msg[3]);
  size_t i5 = a4.find (temp_msg[4]);
  size_t i6 = a4.find (temp_msg[5]);

  constexpr size_t ntokens = 2063592;
  constexpr size_t max22 = 4194304;

  if (i1 == std::string_view::npos || i3 == std::string_view::npos
      || i3 == std::string_view::npos || i4 == std::string_view::npos
      || i5 == std::string_view::npos || i6 == std::string_view::npos)
    {
      d_logger.error (
          "Invalid character tyring to be encoded in standard message");
    }

  uint32_t n28 = ntokens + max22 + 36 * 10 * 27 * 27 * 27 * i1
                 + 10 * 27 * 27 * 27 * i2 + 27 * 27 * 27 * i3 + 27 * 27 * i4
                 + 27 * i5 + i6;

  return n28;
}

uint64_t
ft8_encoder::nonstd_to_58 (std::string &msg)
{
  constexpr std::string_view a = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ/";
  uint64_t n58 = 0;

  std::string temp_msg = msg;
  temp_msg.resize (11, ' '); // max size 11 chars

  for (char c : temp_msg)
    {
      size_t i = a.find (c);
      if (i == std::string_view::npos)
        {
          d_logger.error ("Invalid character for non-standard message");
        }
      n58 = n58 * a.length () + i;
    }

  return n58;
}

std::bitset<71>
ft8_encoder::free_text_to_f71 (std::string &msg)
{
  constexpr std::string_view a = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ+-./?";
  std::string temp_msg = msg;
  temp_msg.resize (13, ' ');

  mpz_class value = 0;

  for (char c : temp_msg)
    {
      size_t i = a.find (c);
      if (i == std::string_view::npos)
        {
          d_logger.error ("Invalid character for free text message");
          i = 0;
        }
      value = value * 42 + i;
    }

  std::bitset<71> bits;
  
  for (size_t i = 0; i<71; ++i){
    bits[i] = mpz_tstbit(value.get_mpz_t(), i);
  }
    
  return bits;
}

uint16_t
ft8_encoder::g4_to_15 (const message &message)
{ // in original protocol RRR, RR73, and 73 are also encoded here, removed
  // because it is redundant
  std::string temp_msg = message.get_message ();
  std::istringstream keywords (temp_msg);
  std::string keyword;

  while (keywords >> keyword)
    {
      if (message.is_grid_square (keyword))
        {
          uint16_t encoded_15 = (keyword[0] - 'A') * 18 * 10 * 10
                                + (keyword[1] - 'A') * 10 * 10
                                + (keyword[2] - '0') * 10 + (keyword[3] - '0');

          size_t pos = temp_msg.find (keyword);
          if (pos != std::string::npos)
            {
              temp_msg.erase (pos, keyword.length ());
            }
          return encoded_15;
        }
    }
  return 0;
}

uint32_t
ft8_encoder::g6_to_15 (const message &message)
{
  std::string temp_msg = message.get_message ();
  std::istringstream keywords (temp_msg);
  std::string keyword;

  while (keywords >> keyword)
    {
      if (message.is_grid_6square (keyword))
        {
          uint32_t encoded_25 = (keyword[0] - 'A') * 18 * 10 * 10 * 24 * 24
                                + (keyword[1] - 'A') * 10 * 10 * 24 * 24
                                + (keyword[2] - '0') * 10 * 24 * 24
                                + (keyword[3] - '0') * 24 * 24
                                + (keyword[4] - '0') * 24 + (keyword[5] - '0');

          size_t pos = temp_msg.find (keyword);
          if (pos != std::string::npos)
            {
              temp_msg.erase (pos, keyword.length ());
            }
          return encoded_25;
        }
    }
  return 0;
}

bool
ft8_encoder::encode_fdclass (char fd_class)
{
  if (fd_class >= 'A' && fd_class <= 'F')
    {
      return fd_class - 'A';
    }
  return 0;
}

bool
ft8_encoder::encode_p1 (std::string &msg)
{
  size_t pos = msg.find ("/P");
  if (pos != std::string::npos)
    {
      msg.erase (pos);
      return true;
    }
  return 0;
}

bool
ft8_encoder::encode_r1 (std::string &msg)
{
  size_t pos = msg.find ("/R");
  if (pos != std::string::npos)
    {
      msg.erase (pos);
      return true;
    }
  return 0;
}

bool
ft8_encoder::encode_t1 (std::string &msg)
{
  if (msg.substr (0, 3) == "TU")
    {
      msg.erase (0);
      return true;
    }
  return 0;
}

uint8_t
ft8_encoder::encode_r2 (std::string &msg)
{
  if (msg == "RRR")
    return 1;
  if (msg == "RR73")
    return 2;
  if (msg == "73")
    return 3;
  return 0;
}

uint8_t
ft8_encoder::encode_sigreport (std::string &msg)
{
  int db = std::stoi (msg);
  return (db + 30) / 2;
}
// signal reports are only allowed for even numbers,
// handle contest formats??
