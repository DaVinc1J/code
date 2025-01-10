from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from bs4 import BeautifulSoup

def main():


    url = 'https://beta.panda.co/originals/crash'

    distance = 2
    betAmount = '60.0'

    distanceCount = distance

    lastMultiplier = None

    options = webdriver.FirefoxOptions()

    driver = webdriver.Firefox(options = options)

    driver.get(url)

    print('log in and choose the coin retard :D')
    input('press enter when you are done so i can continue')

    while True:
        divContent = extractDivData(driver.page_source)
        
        if divContent:
            multiplier = extractMultiplier(divContent)

            if multiplier != lastMultiplier:
                
                print(multiplier)

                if multiplier == 1.00:
                    distanceCount = 0

                if distance > distanceCount:

                    betButton = WebDriverWait(driver, 10).until(
                        EC.element_to_be_clickable((By.XPATH, '//*[@id="main-bet-btn"]'))
                    )

                    betInputBox = WebDriverWait(driver, 10).until(
                        EC.visibility_of_element_located((By.XPATH, '//*[@id="bet-amount"]'))
                    )

                    betInputBox.clear()

                    betInputBox.send_keys(betAmount)

                    betButton.click()

                distanceCount += 1

            lastMultiplier = multiplier

def extractDivData(htmlSource):
    soup = BeautifulSoup(htmlSource, 'html.parser')
    historyDiv = soup.find('div', id='crash-history')
    divs = historyDiv.find_all('div', class_=['', 'games-log-badcrash'])
    
    return divs[0] if divs else None

def extractMultiplier(div):
    multiplier_text = div.text.strip().replace('x', '').replace(',', '')
    multiplier = float(multiplier_text)
    
    return multiplier

main()
