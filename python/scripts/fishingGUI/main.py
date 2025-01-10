import subprocess
import sys

def main():
    files = ("FishingGUI.py", "FishingScript")
    for file in files:
        subprocess.run([sys.executable, file], check = True)

main()
