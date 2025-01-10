from selenium import webdriver
from selenium.webdriver.common.by import By

options = webdriver.FirefoxOptions()

driver = webdriver.Firefox(options=options)

driver.get('https://beta.panda.co/originals/crash') 

print("Please log in to the website manually.")
input("Press Enter after you have logged in and are ready to continue...")

betInputBox = driver.find_element(By.XPATH, '//*[@id="bet-amount"]')

newBet = '1.0'

betInputBox.clear()

betInputBox.send_keys(newBet)
