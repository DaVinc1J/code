from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from webdriver_manager.firefox import GeckoDriverManager
from bs4 import BeautifulSoup
from datetime import datetime
import time

def main():
    url = "https://beta.panda.co/history"
    distance = 2
    betAmount = 10
    bail = 2.0
    buffer = 5

    global mainBalance
    global backupBalance
    global distanceCount
    global count

    mainBalance = 100
    backupBalance = 100
    distanceCount = distance
    count = 0

    options = Options()
    service = Service(GeckoDriverManager().install())
    browser = webdriver.Firefox(service=service, options=options)
    browser.get(url)

    WebDriverWait(browser, 20).until(
        EC.presence_of_element_located((By.XPATH, "//div[contains(@class, 'gameslog-txt')]"))
    )

    lastMultiplier = None

    while True:
        htmlSource = browser.page_source
        divContent = extractDivData(htmlSource)

        if divContent:
            multiplier = extractMultiplier(divContent)

            if multiplier != lastMultiplier:
                if distance > distanceCount:
                    if multiplier >= bail:
                        if mainBalance >= betAmount:
                            mainBalance -= betAmount
                            mainBalance += bail * betAmount
                        else:
                            mainBalance += (betAmount * buffer)
                            backupBalance -= (betAmount * buffer)
                            mainBalance -= betAmount
                            mainBalance += bail * betAmount
                    else:
                        mainBalance -= betAmount

                if multiplier == 1.00:
                    distanceCount = 0

                distanceCount += 1
                count += 1

                lastMultiplier = multiplier

                with open("data.txt", "w") as file:
                    file.write(f"Main Balance: {mainBalance}\n")
                    file.write(f"Backup Balance: {backupBalance}\n")
                    file.write(f"Count: {count}\n")
                    currentTime = datetime.now().strftime("%H:%M:%S")
                    file.write(f"Time: {currentTime}\n")

        time.sleep(4)

def extractDivData(htmlSource):
    try:
        soup = BeautifulSoup(htmlSource, 'html.parser')
        historyDiv = soup.find('div', class_='history-text')
        if not historyDiv:
            print("Error: Unable to find the 'history-text' div.")
            return None

        divs = historyDiv.find_all('div', class_=['gameslog-txt', 'gameslog-txt games-log-badcrash'])
        return divs[-1] if divs else None
    except Exception as e:
        print(f"Error extracting div data: {e}")
        return None

def extractMultiplier(div):
    try:
        multiplier_text = div.text.strip().replace('x', '').replace(',', '')
        multiplier = float(multiplier_text)
        return multiplier
    except Exception as e:
        print(f"Error extracting multiplier: {e}")
        return None

main()
