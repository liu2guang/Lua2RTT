cpu = require("cpu") 
pin = require("pin") 

local pin_low  = 0 
local pin_high = 1 
local tick = 100

local stm32_board_leds = 
{
    {index = 1, pin = 140}, -- PG6 Green 
    {index = 2, pin = 176}, -- PD4 Orange
    {index = 3, pin = 177}, -- PD5 Red
    {index = 4, pin = 194}, -- PK3 Blue 
}

-- Init 4 LEDs 
local function stm32_led_init()
    for i, led in pairs(stm32_board_leds) do
        pin.mode(led.pin, 0) 
        pin.write(led.pin, pin_high) 
    end 
end 

local function stm32_led_show(num) 
    for cnt = 1, num do
        for i, led in pairs(stm32_board_leds) do
            pin.write(led.pin, pin_low) 
            cpu.sleep(tick)
        end 

        for i, led in pairs(stm32_board_leds) do
            pin.write(led.pin, pin_high) 
        end 
        cpu.sleep(tick)
    end 
end 

-- Running water light show 
stm32_led_init()
stm32_led_show(5)
