add_executable(test_bus_scan test_bus_scan.c)
target_link_libraries(test_bus_scan pico_stdlib hardware_i2c)
pico_enable_stdio_usb(test_bus_scan 1)
pico_add_extra_outputs(test_bus_scan)

add_executable(test_led test_led.c)
target_link_libraries(test_led led pico_stdlib)
pico_enable_stdio_usb(test_led 1)
pico_add_extra_outputs(test_led)

add_executable(test_vibrator test_vibrator.c)
target_link_libraries(test_vibrator vibrator pico_stdlib)
pico_enable_stdio_usb(test_vibrator 1)
pico_add_extra_outputs(test_vibrator)

add_executable(test_mp2672 test_mp2672.c)
target_link_libraries(test_mp2672 mp2672 pico_stdlib)
pico_enable_stdio_usb(test_mp2672 1)
pico_add_extra_outputs(test_mp2672)

add_executable(test_ltc2943 test_ltc2943.c)
target_link_libraries(test_ltc2943 ltc2943 pico_stdlib)
pico_enable_stdio_usb(test_ltc2943 1)
pico_add_extra_outputs(test_ltc2943)

add_executable(test_charge test_charge.c)
target_link_libraries(test_charge ltc2943 mp2672 pico_stdlib)
pico_enable_stdio_usb(test_charge 1)
pico_add_extra_outputs(test_charge)

add_executable(test_motor test_motor.c)
target_link_libraries(test_motor motor led pico_stdlib)
pico_enable_stdio_usb(test_motor 1)
pico_add_extra_outputs(test_motor)

add_executable(test_electrodes test_electrodes.c)
target_link_libraries(test_electrodes electrodes pico_stdlib)
pico_enable_stdio_usb(test_electrodes 1)
pico_add_extra_outputs(test_electrodes)

add_executable(test_naive_control test_naive_control.c)
target_link_libraries(test_naive_control motor electrodes pico_stdlib)
pico_enable_stdio_usb(test_naive_control 1)
pico_add_extra_outputs(test_naive_control)
