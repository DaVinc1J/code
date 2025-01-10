require 'selenium-webdriver'
require 'fileutils'

screenshot_dir = File.join(Dir.pwd, 'customScreenshots')
FileUtils.mkdir_p(screenshot_dir)

options = Selenium::WebDriver::Firefox::Options.new
driver = Selenium::WebDriver.for :firefox, options: options

puts 'Navigating to the initial URL...'
url = 'https://snowflake.haesemathematics.com.au/'
driver.navigate.to url

puts 'Website loaded. Please manually open a new tab. Press Enter when ready to switch to the new tab...'
gets

initial_tab = driver.window_handle

puts 'Waiting for you to open the new tab...'
sleep 0.5 while driver.window_handles.length == 1

new_tab = driver.window_handles.find { |handle| handle != initial_tab }
driver.switch_to.window(new_tab)
puts 'Switched to the new tab.'

loop do
  puts 'Press Enter to take a screenshot or type "exit" to quit the application:'
  input = gets.chomp
  break if input.downcase == 'exit'

  puts 'Enter a custom ID for the screenshot or press Enter to use a timestamp:'
  id_input = gets.chomp
  id = id_input.empty? ? Time.now.to_i.to_s : id_input

  screenshot_path = File.join(screenshot_dir, "screenshot_#{id}.png")
  driver.save_screenshot(screenshot_path)
  puts "Screenshot saved as #{screenshot_path}."
end

driver.quit
