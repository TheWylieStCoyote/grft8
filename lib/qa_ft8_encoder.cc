/* -*- c++ -*- */
/*
 * Copyright 2025 Daniel Paul.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ft8_encoder.h"
#include "message.h"
#include <boost/test/unit_test.hpp>
#include <bitset>
#include <vector>
#include <iostream>

BOOST_AUTO_TEST_CASE(test_encode_standard_and_downstream)
{
    message msg("K1ABC/R W9XYZ/R R EN37");
    ft8_encoder encoder;
    
    std::bitset<77> message_bits = encoder.encode_standard(msg);
    
    BOOST_CHECK_EQUAL(message_bits.size(), 77);
    
    //make sure message isn't zero
    bool has_bits_set = false;
    for (size_t i = 0; i < 77; ++i) {
        if (message_bits[i]) {
            has_bits_set = true;
            break;
        }
    }
    BOOST_CHECK(has_bits_set);
    
    std::bitset<91> crc_bits = encoder.calc_crc(message_bits);
    BOOST_CHECK_EQUAL(crc_bits.size(), 91);
    
    for (size_t i = 0; i < 77; ++i) {
        BOOST_CHECK_EQUAL(message_bits[i], crc_bits[i]);
    }
    
    std::bitset<174> ldpc_bits = encoder.apply_ldpc(crc_bits);
    BOOST_CHECK_EQUAL(ldpc_bits.size(), 174);
    
    for (size_t i = 0; i < 91; ++i) {
        BOOST_CHECK_EQUAL(crc_bits[i], ldpc_bits[i]);
    }
    
    std::vector<int> symbols = encoder.bits_to_fsk8(ldpc_bits);
    BOOST_CHECK_EQUAL(symbols.size(), 79);
    
    for (int symbol : symbols) {
        BOOST_CHECK(symbol >= 0 && symbol <= 7);
    }
    
    std::vector<int> expected_sync = {3,1,4,0,6,5,2};
    for (size_t i = 0; i < 7; ++i) {
        BOOST_CHECK_EQUAL(expected_sync[i], symbols[i]); // First sync
        BOOST_CHECK_EQUAL(expected_sync[i], symbols[i + 32]); // Second sync  
        BOOST_CHECK_EQUAL(expected_sync[i], symbols[i + 72]); // Third sync
    }
    
    std::cout << "Test passed" << std::endl;
}

BOOST_AUTO_TEST_CASE(test_different_messages)
{
    ft8_encoder encoder;
    
    //Test different messages
    message msg1("CQ DL1ABC JO62");
    message msg2("W1AW K1JT 73");
    
    std::bitset<77> bits1 = encoder.encode_standard(msg1);
    std::bitset<77> bits2 = encoder.encode_standard(msg2);
    
    //Dif messages should produce dif bit patterns
    BOOST_CHECK(bits1 != bits2);
    
    // Both should have valid downstream processing
    std::bitset<91> crc1 = encoder.calc_crc(bits1);
    std::bitset<91> crc2 = encoder.calc_crc(bits2);
    
    std::vector<int> symbols1 = encoder.bits_to_fsk8(encoder.apply_ldpc(crc1));
    std::vector<int> symbols2 = encoder.bits_to_fsk8(encoder.apply_ldpc(crc2));
    
    BOOST_CHECK_EQUAL(symbols1.size(), 79);
    BOOST_CHECK_EQUAL(symbols2.size(), 79);
    
    BOOST_CHECK(symbols1 != symbols2);
}
