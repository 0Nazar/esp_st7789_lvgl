idf_component_register(SRCS 
"main.c"
"components/ui/ui.c"
"components/ui/ui_helpers.c"
"components/ui/screens/ui_Screen1.c"
"components/ui/screens/ui_Screen2.c"
"components/driver/st7789.c"
"components/driver/fontx.c"
"components/driver/encoder/encoder.c"
"components/driver/encoder/EncoderRead.c"
                    INCLUDE_DIRS "." "components/ui" "components/driver" "components/driver/encoder")