package(default_visibility = ["//visibility:public"])

constraint_value(
    name = "atmega328p",
    constraint_setting = "@platforms//cpu:cpu",
)

constraint_value(
    name = "atmega4809",
    constraint_setting = "@platforms//cpu:cpu",
)

constraint_setting(name = "freq")

constraint_value(
    name = "16MHz",
    constraint_setting = ":freq",
)

constraint_value(
    name = "20MHz",
    constraint_setting = ":freq",
)

platform(
    name = "arduino_nano",
    constraint_values = [
        ":atmega328p",
        ":16MHz",
        "@platforms//os:none",
    ],
)

platform(
    name = "arduino_nano_every",
    constraint_values = [
        ":atmega4809",
        ":20MHz",
        "@platforms//os:none",
    ],
)
