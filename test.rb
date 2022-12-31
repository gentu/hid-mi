#!/usr/bin/env ruby
  require 'evdev'
  gamepad = Evdev.new('/dev/xiaomi_gamepad')
  puts gamepad.name
