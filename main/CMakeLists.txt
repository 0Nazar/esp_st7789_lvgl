idf_component_register(SRCS 
"main.c"
"components/ui/ui.c"
"components/ui/ui_helpers.c"
"components/ui/screens/ui_Screen1.c"
"components/ui/screens/ui_Screen2.c"
"components/driver/st7789.c"
"components/driver/fontx.c"
"components/ext_devices/encoder/encoder.c"
"components/ext_devices/dh22/DHT.c"
"components/ext_devices/max6675/max6675_init.c"
"components/driver/disp_draw.c"
                    INCLUDE_DIRS "." "components/ui" "components/driver" "components/ext_devices/encoder" "components/ext_devices/dh22" "components/ext_devices/max6675")