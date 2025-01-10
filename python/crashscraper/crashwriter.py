import numpy as np
import os
from selenium import webdriver
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service
from webdriver_manager.firefox import GeckoDriverManager
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from bs4 import BeautifulSoup
import time 

def main():
    url = "https://beta.panda.co/history"
    
    trackAndStoreRecentMultiplier(url)

def nextFilePath():

    fileIndex = 1 

    while os.path.exists(f"samples/sample-{fileIndex}.txt"):
        fileIndex += 1

    return f"samples/sample-{fileIndex}.txt"

def trackAndStoreRecentMultiplier(url):
    try:
        options = Options()
        options.headless = True
        service = Service(GeckoDriverManager().install())
        browser = webdriver.Firefox(service=service, options=options)
        browser.get(url)

        print("Waiting for 'history-text' element to load...")
        WebDriverWait(browser, 20).until(
            EC.presence_of_element_located((By.CLASS_NAME, "history-text"))
        )
        print("'history-text' element loaded.")

        print("Waiting for 'gameslog-txt' or 'gameslog-txt games-log-badcrash' divs to load...")
        WebDriverWait(browser, 20).until(
            EC.presence_of_element_located((By.XPATH, "//div[contains(@class, 'gameslog-txt')]"))
        )
        print("'gameslog-txt' divs loaded.")

        filePath = nextFilePath()

        with open(filePath, 'w') as file:
            lastMultiplier = None

            while True:
                htmlSource = browser.page_source
                divContent = extractDivData(htmlSource)

                if divContent:
                    newMultiplier = extractNewMultiplier(divContent)
                    
                    if newMultiplier != lastMultiplier:
                        print(f"{newMultiplier}")
                        file.write(f"{newMultiplier}\n")
                        file.flush()
                        lastMultiplier = newMultiplier

                time.sleep(4)

    except Exception as e:
        print(f"Error fetching HTML: {str(e)}")
    finally:
        if browser:
            browser.quit()

def extractDivData(htmlSource):
    soup = BeautifulSoup(htmlSource, 'html.parser')

    historyDiv = soup.find('div', class_='history-text')

    if not historyDiv:
        print("Error: Unable to find the 'history-text' div.")
        return None

    divs = historyDiv.find_all('div', class_=['gameslog-txt', 'gameslog-txt games-log-badcrash'])

    return divs

def extractNewMultiplier(divs):
    if divs:
        lastDiv = divs[0]
        multiplier = float(lastDiv.text.strip().replace('x', '').replace(',', ''))
        return multiplier
    return None

main()
