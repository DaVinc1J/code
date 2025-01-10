require 'selenium-webdriver'
require 'nokogiri'
require 'time'
require 'webdrivers'

def main
  url = 'https://beta.panda.co/history'
  distanceCount = 0
  betAmount = 10
  bail = 2.0
  buffer = 5

  $mainBalance = 100
  $backupBalance = 100
  $distance = 2
  $count = 0

  options = Selenium::WebDriver::Firefox::Options.new
  browser = Selenium::WebDriver.for(:firefox, options: options)
  browser.get(url)

  wait = Selenium::WebDriver::Wait.new(timout: 40)
  wait.until { browser.execute_script("return document.readyState;") == "complete" }

  parentXpath = "/html/body/div[2]/div[4]/div/div/div[2]/div"
  wait.until { browser.find_element(xpath: parentXpath) }

  targetXpath = "/html/body/div[2]/div[4]/div/div/div[2]/div/div[1]"
  wait.until { browser.find_element(xpath: targetXpath) }

  element = browser.find_element(xpath: targetXpath)

  lastMultiplier = nil

  loop do
    htmlSource = browser.page_source
    divContent = getDivContent(htmlSource)

    if divContent
      multiplier = getMultiplier(divContent)

      if multiplier != lastMultiplier
        if $distance > distanceCount
          if multiplier >= bail
            if $mainBalance >= betAmount
              $mainBalance += (1 - bail) * betAmount
            else
              $mainBalance += (betAmount * buffer)
              $backupBalance -= (betAmount * buffer)
              $mainBalance += (1 - bail) * betAmount
            end
          else
            $mainBalance -= betAmount
          end
        end

        if multiplier == 1.00
          distanceCount = 0
        end

        distanceCount += 1
        $count += 1

        lastMultiplier = multiplier

        File.open("data.txt", "w") do |file|
          file.write("Main Balance: #{$mainBalance}\n")
          file.write("Backup Balance: #{$backupBalance}\n")
          file.write("Count: #{$count}\n")
          time = Time.new.strftime("%H:%M:%S")
          file.write("Time: #{time}\n")
        end
      end
    end

    sleep 4
  end
end

def getDivContent(htmlSource)
  begin
    doc = Nokogiri::HTML.parse(htmlSource)
    historyDiv = doc.at_css('.history-text')
    unless historyDiv
      puts "Error: Could not find 'history-text' div"
      return nil
    end

    divs = historyDiv.css('.gameslog-txt, .gameslog-txt.games-log-badcrash')
    divs.last || nil
  rescue => e
    puts "Error extracting div content: #{e}"
    return nil
  end
end

def getMultiplier(div)
  begin
    multiplierString = div.text.strip.gsub('x', '').gsub(',', '')
    multiplier = multiplierString.to_f
    return multiplier
  rescue => e
    puts "Error extracting multiplier: #{e}"
    nil
  end
end

main

  

