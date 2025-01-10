require 'selenium-webdriver'
require 'fileutils'

screenshot_dir = File.join(Dir.pwd, 'uncroppedScreenshots')
FileUtils.mkdir_p(screenshot_dir)

options = Selenium::WebDriver::Firefox::Options.new
driver = Selenium::WebDriver.for :firefox, options: options

puts 'Navigating to the initial URL...'
url = 'https://snowflake.haesemathematics.com.au/'
driver.navigate.to url

puts 'Website loaded. Please manually open a new tab. Press Enter when ready to switch to the new tab...'
gets

initial_tab = driver.window_handle
sleep 0.5 while driver.window_handles.length == 1

new_tab = driver.window_handles.find { |handle| handle != initial_tab }
driver.switch_to.window(new_tab)

puts 'How many pages would you like to screenshot?'
pages_to_screenshot = gets.chomp.to_i

puts "Starting to take #{pages_to_screenshot} screenshots. Press Enter to begin..."
gets

page_count = 0

begin
  while page_count < pages_to_screenshot
    screenshot_path = File.join(screenshot_dir, "screenshot_#{page_count + 1}.png")
    driver.save_screenshot(screenshot_path)
    page_count += 1
    print "\rScreenshot #{page_count}/#{pages_to_screenshot} taken."
    driver.execute_script('javascript:navPageNext();')
    sleep 1
  end
rescue Selenium::WebDriver::Error::NoSuchElementError
  puts "\nNo more Next button found or an error occurred. Stopping early."
end

driver.quit
puts "\nScreenshots completed."
