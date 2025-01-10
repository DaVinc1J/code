# Initialize tracking variables
last_crash_game = nil   # Stores the game number of the last 1.0 crash
current_game = 0        # Tracks the current game number
risk_threshold = 80     # Define the number of games after which it becomes risky

# Function to handle game results and decide whether to bet
def game_result(bust, current_game, last_crash_game, risk_threshold)
  current_game += 1

  # If a 1.0 crash happens, reset the last crash tracker
  if bust == 1.0
    last_crash_game = current_game
    return "Game #{current_game}: 1.0 crash! Resuming bets.", current_game, last_crash_game
  end

  # Decide if it's safe to keep betting
  if should_bet?(current_game, last_crash_game, risk_threshold)
    return "Game #{current_game}: Betting continues (bust = #{bust})", current_game, last_crash_game
  else
    return "Game #{current_game}: Too risky! Stopping bets until the next 1.0.", current_game, last_crash_game
  end
end

# Function to determine if it's safe to bet
def should_bet?(current_game, last_crash_game, risk_threshold)
  return true if last_crash_game.nil?  # Always bet if no crash has occurred yet

  # Calculate games since the last 1.0 crash
  games_since_last_crash = current_game - last_crash_game

  # Only bet if the number of games since the last crash is within the risk threshold
  games_since_last_crash <= risk_threshold
end

# Simulate the betting system
results = [
  2.02, 3.50, 1.0, 2.50, 1.85, 1.99, 1.0, 2.10, 2.45, 2.30, 1.0, 1.75, 1.68, 2.02
]

# Process each game result
results.each do |bust|
  message, current_game, last_crash_game = game_result(bust, current_game, last_crash_game, risk_threshold)
  puts message
end
