import os
import time
from datetime import datetime, timedelta
import zipfile

# Configuration
PROCESSED_DIR = 'processed_packets'
ARCHIVE_DIR = 'archived_packets'
ARCHIVE_INTERVAL = 24 * 60 * 60  # 24 hours in seconds


def zip_processed_files():
    if not os.path.exists(PROCESSED_DIR):
        print(f"Processed directory {PROCESSED_DIR} does not exist.")
        return

    if not os.path.exists(ARCHIVE_DIR):
        os.makedirs(ARCHIVE_DIR)
    
    # Create a zip file with the current date and time as the name
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    zip_filename = os.path.join(ARCHIVE_DIR, f"archive_{timestamp}.zip")
    
    files_to_remove = []

    with zipfile.ZipFile(zip_filename, 'w') as zipf:
        for root, dirs, files in os.walk(PROCESSED_DIR):
            for file in files:
                if file == '.gitkeep':
                    continue
                file_path = os.path.join(root, file)
                zipf.write(file_path, os.path.relpath(file_path, PROCESSED_DIR))
                files_to_remove.append(file_path)
    
    print(f"Created archive {zip_filename}")
    
    # Remove the original files that were added to the zip file
    for file_path in files_to_remove:
        os.remove(file_path)
        print(f"Removed {file_path}")


if __name__ == '__main__':
    while True:
        start = datetime.now()
        print(f"Archiving started at {start}")

        zip_processed_files()

        end = datetime.now()
        print(f"Archiving completed at {end}")
        print(f"Total archiving time: {end - start}")

        next_run = end + timedelta(seconds=ARCHIVE_INTERVAL)
        print(f"Next run will be at {next_run}")
        time.sleep(ARCHIVE_INTERVAL)
