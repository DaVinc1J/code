from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import time  # Import time for sleep

# Replace with your actual login credentials
username = 'your_username'
password = 'your_password'

# Set up the Firefox WebDriver
options = webdriver.FirefoxOptions()
driver = webdriver.Firefox(options=options)

try:
    print("Starting the web driver and opening the page...")
    driver.get('https://beta.panda.co/originals/crash')

    print("Waiting for any blocking overlay to disappear...")
    WebDriverWait(driver, 10).until(
        EC.invisibility_of_element((By.CLASS_NAME, 'loading-full'))
    )
    print("Overlay disappeared.")

    print("Attempting to click the initial login button...")
    first_login_button = WebDriverWait(driver, 10).until(
        EC.element_to_be_clickable((By.XPATH, '//*[@id="top-login-btn"]'))
    )
    first_login_button.click()
    print("Clicked the initial login button.")

    # Wait for the page to load
    time.sleep(2)  # Adjust the sleep duration as needed

    print("Locating the username input field...")
    username_field = WebDriverWait(driver, 10).until(
        EC.presence_of_element_located((By.CSS_SELECTOR, 'input.onenter.popover-input[name="username"]'))
    )
    print("Username field found. Entering the username...")
    username_field.clear()
    username_field.send_keys(username)

    print("Locating the password input field...")
    password_field = WebDriverWait(driver, 10).until(
        EC.presence_of_element_located((By.CSS_SELECTOR, 'input[name="password"]'))
    )
    print("Password field found. Entering the password...")
    password_field.clear()
    password_field.send_keys(password)

    print("Attempting to click the second login button...")
    second_login_button = WebDriverWait(driver, 10).until(
        EC.element_to_be_clickable((By.XPATH, '/html/body/div[2]/div[6]/div/div/div/div[1]/div[6]/button'))
    )
    second_login_button.click()
    print("Clicked the second login button.")

    print("Waiting for the login to complete and verifying the URL...")
    WebDriverWait(driver, 10).until(
        EC.url_contains('/dashboard')  # Adjust this URL part if different for successful login
    )
    print("Logged in successfully!")

except Exception as e:
    print(f"An error occurred: {e}")
finally:
    driver.quit()
