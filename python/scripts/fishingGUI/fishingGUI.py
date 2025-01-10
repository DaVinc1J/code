import os

import json

from cryptography.fernet import Fernet

import tkinter as tk

from tkinter import messagebox

import subprocess

import uuid

import keyboard

import psutil



# Encryption key (securely generated and should be kept secret)

ENCRYPTION_KEY = b'm3QoS01K1W0BlT0wO_GxksVYoZmr2KhXGGN7Z_tVYyA='  # Replace with a securely generated key



# Path to the embedded data file (adjust to where PyInstaller packages it)

EMBEDDED_DATA_FILE = "data.enc"  # This file will be hidden or embedded with the executable



# List of scripts - adjust paths as needed

scripts = [

   {"name": "Fishing Script", "path": "FishingScript2.py"},

   # Add more scripts as needed

]



current_script = None  # Currently selected script

process = None  # To store the subprocess reference

toggle_hotkey = 'ctrl+alt+r'  # Default hotkey to start and stop the script



def encrypt_data(data, key):

   """Encrypts data using Fernet encryption."""

   fernet = Fernet(key)

   encrypted_data = fernet.encrypt(data.encode())

   return encrypted_data



def decrypt_data(encrypted_data, key):

   """Decrypts data using Fernet encryption."""

   fernet = Fernet(key)

   decrypted_data = fernet.decrypt(encrypted_data).decode()

   return decrypted_data



def load_embedded_data():

   """Loads the data from the embedded encrypted file."""

   if os.path.exists(EMBEDDED_DATA_FILE):

       try:

           with open(EMBEDDED_DATA_FILE, "rb") as file:

               encrypted_data = file.read()

           data = decrypt_data(encrypted_data, ENCRYPTION_KEY)

           return json.loads(data)

       except Exception as e:

           print(f"Error loading embedded data: {e}")

           return {"used_hardware_ids": [], "valid_keys": []}

   return {"used_hardware_ids": [], "valid_keys": []}



def save_embedded_data(data):

   """Saves data to the embedded encrypted file."""

   try:

       encrypted_data = encrypt_data(json.dumps(data), ENCRYPTION_KEY)

       with open(EMBEDDED_DATA_FILE, "wb") as file:

           file.write(encrypted_data)

   except Exception as e:

       print(f"Error saving embedded data: {e}")



def get_hardware_id():

   """Gets the hardware ID using UUID based on hardware info."""

   hardware_id = str(uuid.UUID(int=uuid.getnode())).replace("-", "")

   return hardware_id



def is_hardware_id_used(hardware_id):

   """Checks if the hardware ID is already in the embedded data."""

   data = load_embedded_data()

   used_ids = data.get("used_hardware_ids", [])

   return hardware_id in used_ids



def add_hardware_id(hardware_id):

   """Adds a hardware ID to the list of used IDs in the embedded data."""

   data = load_embedded_data()

   used_ids = data.get("used_hardware_ids", [])

   used_ids.append(hardware_id)

   data["used_hardware_ids"] = used_ids

   save_embedded_data(data)



def validate_key(entered_key):

   """Validates the license key and hardware ID against the embedded data."""

   hardware_id = get_hardware_id()

   data = load_embedded_data()



   # Check if the hardware ID and entered key combination is already used

   used_combinations = data.get("used_combinations", [])



   # Allow if the entered key and hardware ID combination has already been used on this device

   if {"hardware_id": hardware_id, "key": entered_key} in used_combinations:

       messagebox.showinfo("Success", "Key validated successfully! You can now use the software.")

       root.state('normal')  # Show the main app window

       key_window.destroy()  # Close the key entry window

       return True



   # Validate the entered key and add a new combination if valid

   if entered_key in data.get("valid_keys", []):

       # Add hardware ID and key combination to allow future reuse on this device

       used_combinations.append({"hardware_id": hardware_id, "key": entered_key})

       data["used_combinations"] = used_combinations

       save_embedded_data(data)

       messagebox.showinfo("Success", "Key validated successfully! You can now use the software.")

       root.state('normal')  # Show the main app window

       key_window.destroy()  # Close the key entry window

       return True

   else:

       messagebox.showerror("Error", "Invalid key.")

       return False



def get_last_modified_date(filepath):

   """Get the last modified date of a script file."""

   try:

       timestamp = os.path.getmtime(filepath)

       return time.strftime('%Y-%m-%d', time.localtime(timestamp))  # Only show the date

   except FileNotFoundError:

       return "File not found"



def toggle_script():

   """Start or stop the selected script."""

   global process

   if current_script is None:

       messagebox.showwarning("Warning", "Please select a script first.")

       return



   if process is None:  # Start the script if not running

       try:

           process = subprocess.Popen(["python", current_script['path']], shell=True)

           messagebox.showinfo("Execution", f"{current_script['name']} is running.")

       except Exception as e:

           messagebox.showerror("Error", f"Failed to run {current_script['name']}: {str(e)}")

   else:  # Stop the script if running

       try:

           # Terminate the process and all its children using psutil

           parent = psutil.Process(process.pid)

           for child in parent.children(recursive=True):

               child.kill()  # Kill child processes

           parent.kill()  # Kill the main process

           process = None

           messagebox.showinfo("Execution", f"{current_script['name']} has been stopped.")

       except Exception as e:

           messagebox.showerror("Error", f"Failed to stop {current_script['name']}: {str(e)}")



def select_script(event):

   """Update the current script based on user selection from the listbox."""

   global current_script

   selection = script_listbox.curselection()

   if selection:

       index = selection[0]

       current_script = scripts[index]

       update_script_info()



def update_script_info():

   """Display the information about the currently selected script."""

   if current_script:

       last_updated = get_last_modified_date(current_script['path'])

       script_info_label.config(

           text=f"Script: {current_script['name']}\nPath: {current_script['path']}\nLast Updated: {last_updated}"

       )



def change_hotkey():

   """Change the hotkey used to start and stop the script."""

   def set_hotkey():

       new_key = hotkey_entry.get()

       if new_key:

           try:

               global toggle_hotkey

               keyboard.remove_hotkey(toggle_hotkey)

               toggle_hotkey = new_key

               keyboard.add_hotkey(toggle_hotkey, toggle_script)

               messagebox.showinfo("Hotkey Set", f"Hotkey set to: {new_key}")

               hotkey_window.destroy()

           except ValueError:

               messagebox.showerror("Error", "Invalid hotkey.")

       else:

           messagebox.showwarning("Warning", "Please enter a hotkey.")



   hotkey_window = tk.Toplevel(root)

   hotkey_window.title("Set Toggle Hotkey")

   hotkey_label = tk.Label(hotkey_window, text="Enter new hotkey:")

   hotkey_label.pack(pady=5)

   hotkey_entry = tk.Entry(hotkey_window)

   hotkey_entry.pack(pady=5)

   set_button = tk.Button(hotkey_window, text="Set", command=set_hotkey)

   set_button.pack(pady=10)



def setup_hotkeys():

   """Set up the initial hotkeys."""

   keyboard.add_hotkey(toggle_hotkey, toggle_script)



# Main application window (hidden until key is validated)

root = tk.Tk()

root.title("Script Host Interface")

root.geometry("450x300")

root.withdraw()  # Start hidden until key is validated



# License key window

key_window = tk.Toplevel(root)  # Create the key window as a child of root

key_window.title("License Key Validation")

key_window.geometry("300x150")



key_label = tk.Label(key_window, text="Enter your license key:")

key_label.pack(pady=10)



key_entry = tk.Entry(key_window)

key_entry.pack(pady=5)



validate_button = tk.Button(key_window, text="Validate Key", command=lambda: validate_key(key_entry.get().strip()))

validate_button.pack(pady=10)



# Left Panel: Script Selection

left_frame = tk.Frame(root, width=200, bg="#f0f0f0")

left_frame.pack(side="left", fill="y")



script_listbox = tk.Listbox(left_frame, selectmode=tk.SINGLE)

script_listbox.pack(padx=10, pady=10, fill="both", expand=True)

for script in scripts:

   script_listbox.insert(tk.END, script['name'])

script_listbox.bind("<<ListboxSelect>>", select_script)



# Right Panel: Script Info and Controls

right_frame = tk.Frame(root, bg="white")

right_frame.pack(side="right", fill="both", expand=True, padx=10, pady=10)



script_info_label = tk.Label(right_frame, text="Select a script to see details", justify="left", bg="white")

script_info_label.pack(pady=10)



# Toggle button for running and stopping scripts

toggle_button = tk.Button(right_frame, text="Toggle Script", command=toggle_script, width=20, bg="#f0f0f0")

toggle_button.pack(pady=5)



# Button to change hotkey

change_hotkey_button = tk.Button(right_frame, text="Change Toggle Hotkey", command=change_hotkey, bg="#f0f0f0")

change_hotkey_button.pack(pady=5)



# Exit button

exit_button = tk.Button(right_frame, text="Exit", command=root.quit, width=20)

exit_button.pack(pady=10)



# Set up initial hotkeys

setup_hotkeys()



# Clean up hotkeys on exit

root.protocol("WM_DELETE_WINDOW", lambda: [keyboard.unhook_all_hotkeys(), root.quit()])

root.mainloop()
