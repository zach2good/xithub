import os
import shutil
import time
import random
from datetime import datetime, timedelta

# Configuration
INPUT_DIR = 'packets'
PROCESSED_DIR = 'processed_packets'
PROCESS_INTERVAL = 5 * 60  # 5 minutes in seconds
RUNTIME_DATA_DIR = 'runtime_data'
SESSIONS_FILE = os.path.join(RUNTIME_DATA_DIR, 'sessions.txt')
SESSION_MAX_AGE_MINUTES = 2


def generate_session_id(user_id):
    return str(user_id) + str(int(time.time()))


def tap_session(user_id):
    session_exists = False
    session_id = None
    current_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    sessions = []

    if os.path.exists(SESSIONS_FILE):
        with open(SESSIONS_FILE, 'r') as f:
            sessions = f.readlines()

    for i, session in enumerate(sessions):
        parts = session.strip().split(',')
        if parts[0] == str(user_id):
            session_id = parts[1]
            sessions[i] = f"{user_id},{session_id},{parts[2]},{current_time}\n"
            session_exists = True
            break

    if not session_exists:
        session_id = generate_session_id(user_id)
        sessions.append(f"{user_id},{session_id},{current_time},{current_time}\n")

    with open(SESSIONS_FILE, 'w') as f:
        f.writelines(sessions)


def try_cut_sessions():
    current_time = datetime.now()
    sessions = []

    if os.path.exists(SESSIONS_FILE):
        with open(SESSIONS_FILE, 'r') as f:
            sessions = f.readlines()

    updated_sessions = []
    for session in sessions:
        parts = session.strip().split(',')
        last_updated_time = datetime.strptime(parts[3], '%Y-%m-%d %H:%M:%S')
        if current_time - last_updated_time <= timedelta(minutes=SESSION_MAX_AGE_MINUTES):
            updated_sessions.append(session)

    with open(SESSIONS_FILE, 'w') as f:
        f.writelines(updated_sessions)


def process_packet(file_path):
    print(f"Processing file: {file_path}")


    with open(file_path, 'rb') as f:
        packet_data = f.read()
        # TODO
        time.sleep(0.1)

    userId = get_user_id_from_filename(file_path)
    tap_session(userId)

    print(f"Finished processing file: {file_path}")


def move_to_processed(file_path, processed_dir):
    if not os.path.exists(processed_dir):
        os.makedirs(processed_dir)

    filename = os.path.basename(file_path)
    dest_path = os.path.join(processed_dir, filename)
    shutil.move(file_path, dest_path)
    print(f"Moved {file_path} to {dest_path}")


def get_timestamp_from_filename(filename):
    try:
        # filename format is "packet_userId_timestamp.bin"
        parts = filename.split('_')
        timestamp = int(parts[2].split('.')[0])
        return timestamp
    except (IndexError, ValueError):
        return None


def get_user_id_from_filename(filename):
    try:
        # filename format is "packet_userId_timestamp.bin"
        parts = filename.split('_')
        user_id = int(parts[1])
        return user_id
    except (IndexError, ValueError):
        return None


def process_packets():
    if not os.path.exists(INPUT_DIR):
        print(f"Input directory {INPUT_DIR} does not exist.")
        return

    # .bin files
    packet_files = [f for f in os.listdir(INPUT_DIR) if os.path.isfile(os.path.join(INPUT_DIR, f)) and f.endswith('.bin')]

    if not packet_files:
        print(f"No packet files found in {INPUT_DIR}.")
        return

    # Sort packet files based on the timestamp extracted from filenames
    packet_files.sort(key=get_timestamp_from_filename)

    for packet_file in packet_files:
        packet_file_path = os.path.join(INPUT_DIR, packet_file)
        process_packet(packet_file_path)
        move_to_processed(packet_file_path, PROCESSED_DIR)


if __name__ == '__main__':
    while True:
        start = datetime.now()
        print(f"Packet Processing started at {start}")

        process_packets()
        try_cut_sessions()

        end = datetime.now()
        print(f"Packet Processing completed at {end}")
        print(f"Total archiving time: {end - start}")

        next_run = end + timedelta(seconds=PROCESS_INTERVAL)
        print(f"Next run will be at {next_run}")
        time.sleep(PROCESS_INTERVAL)
