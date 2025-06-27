#include "ft8_encoder.h"
#include "message.h"
#include <boost/test/unit_test.hpp>

/*BOOST_AUTO_TEST_CASE(t0_make_instance) {
    ft8_encoder enc;
}*/
/*
BOOST_AUTO_TEST_CASE(t1_check_callsign_encoding) {
    ft8_encoder enc;
    std::string callsign = "K1ABC";
    
    uint32_t result = enc.std_call_to_28(callsign);
    uint32_t should_be_value = 6606708; 
    
    BOOST_CHECK_GT(result, 0); 
    BOOST_CHECK_EQUAL(result, should_be_value); 
}


BOOST_AUTO_TEST_CASE(t2_check_grid_encoding) {
    ft8_encoder enc;
    message msg("K1ABC W1XYZ FN42");
    
    uint16_t result = enc.g4_to_15(msg);
    
    //(F-A)*18*10*10 + (N-A)*10*10 + 4*10 + 2
    // = 5*1800 + 13*100 + 40 + 2 = 9000 + 1300 + 42 = 10342
    uint16_t should_be_value = 10342;
    
    BOOST_CHECK_EQUAL(result, should_be_value);
}

BOOST_AUTO_TEST_CASE(t3_check_cq_encoding) {
    ft8_encoder enc;
    message msg;
    std::string temp_msg = "CQ";
    
    uint32_t result = enc.encode_28(temp_msg, msg);
    uint32_t should_be_value = 2; 
    
    BOOST_CHECK_EQUAL(result, should_be_value);
}*/
