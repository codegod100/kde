include(ECMAddTests)

ecm_add_test(prefixdevtest.cpp TEST_NAME prefixdevtest LINK_LIBRARIES prefixdevcore Qt6::Test)

# Runs the real plugin against prefix.dev; needs network, so it is opt-in:
# ctest -L online
ecm_add_test(prefixdevrunnertest.cpp TEST_NAME prefixdevrunnertest LINK_LIBRARIES Qt6::Test KF6::Runner)
krunner_configure_test(prefixdevrunnertest krunner_prefixdev)
set_tests_properties(prefixdevrunnertest PROPERTIES LABELS online)
