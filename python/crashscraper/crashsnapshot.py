import numpy as np
from selenium import webdriver
from selenium.webdriver.firefox.options import Options
from selenium.webdriver.firefox.service import Service
from webdriver_manager.firefox import GeckoDriverManager
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from bs4 import BeautifulSoup
import math
import matplotlib.pyplot as plt

def main():
    url = "https://beta.panda.co/history"
    
    print(f"Fetching HTML from {url}")
    htmlSource = displayHtml(url)

    if htmlSource:
        print("HTML source fetched successfully.")
        divContent = extractDivData(htmlSource)
        
        if divContent:
            print("Div content extracted successfully.")
            multipliers = extractMultipliers(divContent)
            calculateCI(multipliers, 100, 1.96)
        else:
            print("No div content extracted.")
    else:
        print("Failed to fetch HTML source.")

def displayHtml(url):
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

        htmlSource = browser.page_source
        return htmlSource

    except Exception as e:
        print(f"Error fetching HTML: {str(e)}")
        return None
    finally:
        if browser:
            browser.quit()

def extractDivData(htmlSource):
    print("Parsing HTML content...")
    soup = BeautifulSoup(htmlSource, 'html.parser')

    print("Looking for 'history-text' div...")
    historyDiv = soup.find('div', class_='history-text')

    if not historyDiv:
        print("Error: Unable to find the 'history-text' div.")
        return None

    print("'history-text' div found. Extracting div content...")

    divs = historyDiv.find_all('div', class_=['gameslog-txt', 'gameslog-txt games-log-badcrash'])

    return divs

def extractMultipliers(divs):
    multipliers = []
    print("\nExtracting only the number part (without 'x'):")
    for div in divs:
        multiplier = float(div.text.strip().replace('x', '').replace(',', ''))
        multipliers.append(multiplier)
        print(f"{multiplier}")
    return multipliers

def calculateCI(multipliers, betAmount, zConf):
    badRolls = [m for m in multipliers if m < 2]
    goodRolls = [m for m in multipliers if m > 2]

    totalRolls = len(multipliers)

    def computeCI(data):
        mean = np.mean(data)
        sdev = np.std(data, ddof=1)
        n = len(data)
        zValue = zConf
        zScore = zValue * (sdev / math.sqrt(n))
        return (mean - zScore, mean + zScore), mean, sdev

    if badRolls:
        badRollCI, badMean, badSdev = computeCI(badRolls)
    else:
        badRollCI, badMean, badSdev = (None, None), None, None

    print("\n--- Confidence Interval and Betting Calculation ---")
    print(f"Total Rolls: {totalRolls}")

    if badRolls:
        print(f"\nBad Rolls (<2): {len(badRolls)}")
        print(f"Confidence Interval for Bad Rolls: {badRollCI[0]:.2f} to {badRollCI[1]:.2f}")

    options = (badRollCI[0], badMean, badRollCI[1])

    calculateBettingOutcomes(options, multipliers, betAmount)
    
def calculateBettingOutcomes(options, multipliers, betAmount):
    print(f"\n--- Betting Simulation ---")
    for option in options:

        if option is not None:

            losingBets = [m for m in multipliers if m < option]
            winningBets = [m for m in multipliers if m >= option]

            totalLosses = len(losingBets) * betAmount

            totalWinnings = sum([(option - 1) * betAmount for multiplier in winningBets])

            profit = totalWinnings - totalLosses

            print("-------------------------")
            print(f"Multiplier: {option:.2f}")
            print(f"Number of Losing Bets: {len(losingBets)}")
            print(f"Number of Winning Bets: {len(winningBets)}")
            print(f"Total Losses: {totalLosses}")
            print(f"Total Winnings: {totalWinnings:.2f}")
            print("**")
            print(f"Multiplier: {option:.2f}")
            print(f"Net Profit: {profit:.2f}")
            print("**")
            print("-------------------------")
        else:
            print("\nInsufficient data to calculate betting outcome.")

    multiplierRange = np.arange(1.01, 2.00, 0.01)
    profits = []

    print(f"\n--- Betting Simulation ---")
    for option in multiplierRange:
        losingBets = [m for m in multipliers if m < option]
        winningBets = [m for m in multipliers if m >= option]

        totalLosses = len(losingBets) * betAmount

        totalWinnings = sum([(option - 1) * betAmount for multiplier in winningBets])

        profit = totalWinnings - totalLosses

        profits.append(profit)

        print(f"Multiplier: {option:.2f}, Net Profit: {profit:.2f}")

    plt.plot(multiplierRange, profits, marker='o')
    plt.title("Profit vs. Multiplier")
    plt.xlabel("Multiplier")
    plt.ylabel("Profit")
    plt.grid(True)
    plt.show()

main()
