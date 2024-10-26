import serial
import csv
import time

# Configure your COM port and baud rate
COM_PORT = 'COM4'  # Change this to your COM port
BAUD_RATE = 9600    # Change this to your baud rate
CSV_FILE = 'output.csv'

def main():
    # Open the serial port
    ser = serial.Serial(COM_PORT, BAUD_RATE)
    
    # Wait for the connection to establish
    time.sleep(2)

    # Open the CSV file for writing
    with open(CSV_FILE, mode='w', newline='') as csv_file:
        csv_writer = csv.writer(csv_file)

        line_count = 0  # Counter to track the number of lines read

        try:
            print("Listening on", COM_PORT)
            while True:
                # Read a line from the serial port
                line = ser.readline().decode('utf-8').strip()
                print("Received:", line)  # Print to console for debugging
                
                line_count += 1  # Increment the line count
                
                # Skip the first line
                if line_count > 1:
                    csv_writer.writerow(line.split(','))  # Write to CSV

        except KeyboardInterrupt:
            print("Exiting...")
        except Exception as e:
            print("Error:", e)
        finally:
            ser.close()
            print("Serial port closed.")

if __name__ == "__main__":
    main()
