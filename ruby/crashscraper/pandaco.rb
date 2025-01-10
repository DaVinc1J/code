require 'selenium-webdriver'

# Replace with your actual login credentials
username = 'your_username'
password = 'your_password'

# Set up Firefox options
options = Selenium::WebDriver::Firefox::Options.new
options.add_argument('--start-maximized') # Optional: Start maximized for easier interaction

# Initialize the WebDriver
driver = Selenium::WebDriver.for :firefox, options: options

puts 'Starting the web driver and opening the page...'
driver.navigate.to 'https://beta.panda.co/originals/crash'

puts 'Waiting for any blocking overlay to disappear...'
wait = Selenium::WebDriver::Wait.new(timeout: 10) # seconds
wait.until { driver.find_element(:class, 'loading-full').displayed? == false }
puts 'Overlay disappeared.'

puts 'Waiting for the cookie consent button to appear...'
cookie_button = wait.until do
  driver.find_element(:xpath, '/html/body/div[2]/div[5]/div/a')
end

# Scroll into view and click using JavaScript to force the action
puts 'Scrolling cookie consent button into view and clicking it with JavaScript...'
driver.execute_script('arguments[0].scrollIntoView(true);', cookie_button)
driver.execute_script('arguments[0].click();', cookie_button)
puts 'Cookie consent button clicked using JavaScript.'

puts 'Attempting to click the initial login button...'
first_login_button = wait.until do
  driver.find_element(:xpath, '//*[@id="top-login-btn"]')
end
first_login_button.click
puts 'Clicked the initial login button.'

# Check if the input fields are within an iframe and switch if necessary
begin
  puts 'Checking for iframe...'
  iframe = wait.until { driver.find_element(:tag_name, 'iframe') }
  driver.switch_to.frame(iframe)
  puts 'Switched to iframe.'
rescue StandardError
  puts 'No iframe found, proceeding without switching.'
end

puts 'Locating the username input field using the provided XPath...'
username_field = wait.until do
  driver.find_element(:xpath, '/html/body/div[2]/div[4]/div/div/div/div[1]/div[3]/input')
end
puts 'Username field found. Making the username field visible and setting focus...'
driver.execute_script('arguments[0].style.display = "block";', username_field)
driver.execute_script('arguments[0].focus();', username_field)
puts 'Entering the username using JavaScript...'
driver.execute_script('arguments[0].value = arguments[1];', username_field, username)
puts 'Username entered.'

puts 'Locating the password input field...'
password_field = wait.until do
  driver.find_element(:name, 'password') # Update this XPath if necessary
end
puts 'Password field found. Making the password field visible and setting focus...'
driver.execute_script('arguments[0].style.display = "block";', password_field)
driver.execute_script('arguments[0].focus();', password_field)
puts 'Entering the password using JavaScript...'
driver.execute_script('arguments[0].value = arguments[1];', password_field, password)
puts 'Password entered.'

puts 'Switching back to the main content if switched to an iframe...'
driver.switch_to.default_content

puts 'Attempting to click the second login button...'
second_login_button = wait.until do
  driver.find_element(:xpath, '/html/body/div[2]/div[6]/div/div/div/div[1]/div[6]/button')
end
second_login_button.click
puts 'Clicked the second login button.'

puts 'Waiting for the login to complete and verifying the URL...'
wait.until { driver.current_url.include?('/dashboard') } # Adjust this URL part if different for successful login
puts 'Logged in successfully!'
