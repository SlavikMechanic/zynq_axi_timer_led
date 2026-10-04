## PYNQ-Z1 LED0-LED3
set_property -dict {PACKAGE_PIN R14 IOSTANDARD LVCMOS33} [get_ports {LED[0]}]
set_property -dict {PACKAGE_PIN P14 IOSTANDARD LVCMOS33} [get_ports {LED[1]}]
set_property -dict {PACKAGE_PIN N16 IOSTANDARD LVCMOS33} [get_ports {LED[2]}]
set_property -dict {PACKAGE_PIN M14 IOSTANDARD LVCMOS33} [get_ports {LED[3]}]

## PYNQ-Z1 Buttons BTN0-BTN3
set_property -dict {PACKAGE_PIN D19 IOSTANDARD LVCMOS33} [get_ports {BTN_SW[0]}]
set_property -dict {PACKAGE_PIN D20 IOSTANDARD LVCMOS33} [get_ports {BTN_SW[1]}]
set_property -dict {PACKAGE_PIN L20 IOSTANDARD LVCMOS33} [get_ports {BTN_SW[2]}]
set_property -dict {PACKAGE_PIN L19 IOSTANDARD LVCMOS33} [get_ports {BTN_SW[3]}]

## PYNQ-Z1 Switch SW0
set_property -dict {PACKAGE_PIN M20 IOSTANDARD LVCMOS33} [get_ports {BTN_SW[4]}]