require 'selenium-webdriver'
require 'nokogiri'
require 'webdrivers/geckodriver'
require 'fileutils'

def main
  url = 'https://beta.panda.co/history'
  copyMultipliers(url)
end

def nextFile
  fileIndex = 1

  fileIndex += 1 while File.exist?("samples/samples-#{fileIndex}.txt")

  "samples/sample-#{fileIndex}.txt"
end

def copyMultipliers(url)
  options = Selenium::WebDriver::Firefox::Options.new
  options.headless!
  browser = Selenium::WebDriver.for :firefox, options: options
  browser.navigate.to url

  Selenium::WebDriver::Wait.new(timeout: 20).until do
    browser.find_element(xpath: "//div[contains(@class, 'gameslog-txt')]")
  end

  filePath = nextFile

  File.open(filePath, 'a') do |file|
    lastMultiplier = nil

    loop do
      htmlSource = browser.page_source
      divContent = extractDivData(htmlSource)

      if divContent
        multiplier = getMultiplier(divContent)

        if multiplier != lastMultiplier
          file.puts multiplier
          lastMultiplier = multiplier
        end
      end

      sleep 4
    end
  end
rescue StandardError => e
  puts "Error fetching HTML: #{e.message}"
ensure
  browser.quit if browser
end

def extractDivData(htmlSource)
  doc = Nokogiri::HTML(htmlSource)

  historyDiv = doc.at_css('div.history-text')

  unless historyDiv
    puts "Error: Unable to find the 'history-text' div."
    return nil
  end

  historyDiv.css('div.gameslog-txt, div.gameslog-txt.games-log-badcrash')
end

def getMultiplier(divs)
  return nil unless divs && !divs.empty?

  lastDiv = divs.first
  lastDiv.text.strip.gsub('x', '').gsub(',', '').to_f
end

main
